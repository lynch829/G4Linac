/*
 * Copyright (C) 2009 International Atomic Energy Agency
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *-----------------------------------------------------------------------------
 *
 *   AUTHORS:
 *
 *   Miguel Antonio Cortes Giraldo
 *   e-mail: miancortes -at- us.es
 *   University of Seville, Spain
 *   Dep. Fisica Atomica, Molecular y Nuclear
 *   Ap. 1065, E-41080 Seville, SPAIN
 *   Phone: +34-954550928; Fax: +34-954554445
 *
 *   Jose Manuel Quesada Molina, PhD
 *   e-mail: quesada -at- us.es
 *   University of Seville, Spain
 *   Dep. Fisica Atomica, Molecular y Nuclear
 *   Ap. 1065, E-41080 Seville, SPAIN
 *   Phone: +34-954559508; Fax: +34-954554445
 *
 *   Roberto Capote Noy, PhD
 *   e-mail: R.CapoteNoy -at- iaea.org (rcapotenoy -at- yahoo.com)
 *   International Atomic Energy Agency
 *   Nuclear Data Section, P.O.Box 100
 *   Wagramerstrasse 5, Vienna A-1400, AUSTRIA
 *   Phone: +431-260021713; Fax: +431-26007
 *
 **********************************************************************************
 * For documentation
 * see http://www-nds.iaea.org/phsp
 *
 * - 07/12/2009: public version 1.0
 *
 **********************************************************************************/


#include "G4IAEAphspWriter.hh"

G4IAEAphspWriter * G4IAEAphspWriter::theInstance = 0;

G4IAEAphspWriter * G4IAEAphspWriter::GetInstance()
{
  if (!theInstance) theInstance = new G4IAEAphspWriter();
  return theInstance;
}


#include "G4Event.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "globals.hh"

#include "iaea_phsp.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <vector>



G4IAEAphspWriter::G4IAEAphspWriter()
{
  theFileName = "PSF";  // default name
  theZStopVector = new std::vector<G4double>;
  theIncrNumberVector = new std::vector<G4int>;
  thePassingTracksVector = new std::vector< std::set<G4int>* >;
  theFilesAlreadyOpen = 0;
}


G4IAEAphspWriter::~G4IAEAphspWriter()
{
  if (theZStopVector) delete theZStopVector;
  if (theIncrNumberVector) delete theIncrNumberVector;
  if (thePassingTracksVector) delete thePassingTracksVector;
}


//==================================================
// void G4IAEAphspWriter::SetZStop(G4double zstop)
//==================================================
void G4IAEAphspWriter::SetZStop(G4double zstop)
{
  theZStopVector->push_back(zstop);
  G4cout << "Registered phase-space plane z = " << zstop/cm
	 << " cm." << G4endl;
}


//=============================================================
// void G4IAEAphspWriter::BeginOfRunAction(const G4Run* aRun)
//=============================================================
void G4IAEAphspWriter::BeginOfRunAction(const G4Run* aRun)
{
  // Sort the Zstops from lower to higher in order to optimize
  // the 'while' loop at UserSteppingAction.
  std::sort(theZStopVector->begin(), theZStopVector->end());
  G4int sizeVector = theZStopVector->size();

  theIncrNumberVector->reserve(sizeVector);
  for (G4int i = 0; i < sizeVector; i++)
    theIncrNumberVector->push_back(0);

  thePassingTracksVector->reserve(sizeVector);
  for (G4int i = 0; i < sizeVector; i++)
    {
      std::set<G4int> * aSet = new std::set<G4int>;
      thePassingTracksVector->push_back(aSet);
    }

  // And now open all the files intended to store
  // the phase spaces following the IAEA format.

  const IAEA_I32 accessWrite = static_cast<const IAEA_I32>( theAccessWrite );
  G4int runID = aRun->GetRunID();
  IAEA_I64 nEvts = static_cast<IAEA_I64>(aRun->GetNumberOfEventToBeProcessed());
  for (G4int ii = 0; ii < sizeVector; ii++)
    {
      // Set the source ID and file name in a unique way
      std::stringstream sstr;
      sstr << (*theZStopVector)[ii];
      G4String zStopID(sstr.str());
      G4String fullName = theFileName + "_" + zStopID;

      // This part is only added when running several runs
      // during the simulation.
      if (runID > 0)
	{
	  std::stringstream sstr2;
	  sstr2 << runID;
	  G4String runIDStr(sstr2.str());
	  fullName += "_";
	  fullName += runIDStr;
	}

      // Create the file to store the IAEA phase space

      IAEA_I32 id = static_cast<IAEA_I32>(ii);
      char* filename = const_cast<char*>(fullName.data());
      IAEA_I32 result;
      iaea_new_source( &id, filename, &accessWrite, &result, fullName.size()+1 );
      // This difference tells about the number of IAEA files already open.
      theFilesAlreadyOpen = id - ii;

      // Set the global information and options.

      // Set constant Z
      IAEA_I32 varIdx = 2;  // 0=x, 1=y, 2=z, 3=u, 4=v, 5=w, 6=wt
      IAEA_Float varValue = static_cast<IAEA_Float>((*theZStopVector)[ii]/cm);
      iaea_set_constant_variable(&id, &varIdx, &varValue);

      // Number of original histories
      iaea_set_total_original_particles(&id, &nEvts);

      // Extra variables
      IAEA_I32 extraFloats = 0;
      IAEA_I32 extraInts = 1;
      iaea_set_extra_numbers(&id, &extraFloats, &extraInts);

      // Extra variables types
      IAEA_I32 longIdx = 0;
      IAEA_I32 longType = 1; // incremental history number
      iaea_set_type_extralong_variable(&id, &longIdx, &longType);

    }
}


//================================================================
// void G4IAEAphspWriter::BeginOfEventAction(const G4Event*)
//================================================================
void G4IAEAphspWriter::BeginOfEventAction(const G4Event*)
{
  // Update all the incremental history numbers.
  std::vector<G4int>::iterator it;
  for (it = theIncrNumberVector->begin(); it != theIncrNumberVector->end(); it++)
    (*it)++;

  // Remove the track ID's stored during the previous event.
  std::vector< std::set<G4int>* >::iterator is;
  for (is = thePassingTracksVector->begin(); is != thePassingTracksVector->end(); is++)
    {
      (*is)->clear();
    }
}



//================================================================
// void G4IAEAphspWriter::UpdateHeaders()
//================================================================
void G4IAEAphspWriter::UpdateHeaders()
{
  G4int size = theZStopVector->size();
  IAEA_I32 result;
  for (G4int ii = 0; ii < size; ii++)
    {
      const IAEA_I32 sourceID = static_cast<const IAEA_I32>(ii+theFilesAlreadyOpen);
      iaea_update_header(&sourceID, &result);

      if (result == -1)
	{
	  G4Exception("G4IAEAphspWriter::UpdateHeaders()\nIAEA source does not exist");
	}
    }
}



//================================================================
// void G4IAEAphspWriter::UserSteppingAction(const G4Step* aStep)
//================================================================
void G4IAEAphspWriter::UserSteppingAction(const G4Step* aStep)
{
  postR = aStep->GetPostStepPoint()->GetPosition();
  preR = aStep->GetPreStepPoint()->GetPosition();
  G4double postZ = postR.z();
  G4double preZ = preR.z();

  G4int i = 0;
  G4int size = theZStopVector->size();
  G4double zStop = (*theZStopVector)[i];
  while (postZ >= zStop && i < size)
    {
      if (preZ < zStop)
	{
	  // Check that this track has not crossed the
	  // i-th plane before.
	  G4int trackID = aStep->GetTrack()->GetTrackID();
	  std::set<G4int>::iterator it;
	  it = (*thePassingTracksVector)[i]->find(trackID);

	  if ( it == (*thePassingTracksVector)[i]->end() )
	    {
	      // Not passed before, so store the particle.
	      StoreIAEAParticle(aStep, i);
	      (*theIncrNumberVector)[i] = 0; // reset this counter

	      (*thePassingTracksVector)[i]->insert(trackID);
	    }
	}
      i++;
      zStop = (*theZStopVector)[i];
    }
}

//===============================================================================
// void G4IAEAphspWriter::StoreIAEAParticle(const G4Step* aStep, G4int zStopIdx)
//===============================================================================
void G4IAEAphspWriter::StoreIAEAParticle(const G4Step* aStep, const G4int zStopIdx)
{
  IAEA_I32 sourceID = static_cast<IAEA_I32>(zStopIdx+theFilesAlreadyOpen); // beware
  G4double zStop = (*theZStopVector)[zStopIdx];

  // The particle type and kinetic energy
  // -------------------------------------
  const G4Track* aTrack = aStep->GetTrack();
  G4int PDGCode = aTrack->GetDefinition()->GetPDGEncoding();
  IAEA_I32 partType;
  G4double postE = aStep->GetPostStepPoint()->GetKineticEnergy();
  G4double preE = aStep->GetPreStepPoint()->GetKineticEnergy();
  IAEA_Float kinEnergyMeV;
  G4double postZ = postR.z();
  G4double preZ = preR.z();

  switch(PDGCode)
    {
    case 22:
      partType = 1;  // gamma
      kinEnergyMeV = static_cast<IAEA_Float>(preE/MeV);
      break;
    case 11:
      partType = 2;  // electron
      kinEnergyMeV = static_cast<IAEA_Float>( preE+(postE-preE)*(zStop-preZ)/(postZ-preZ) );
      break;
    case -11:
      partType = 3;  // positron
      kinEnergyMeV = static_cast<IAEA_Float>( preE+(postE-preE)*(zStop-preZ)/(postZ-preZ) );
      break;
    case 2112:
      partType = 4;  // neutron
      kinEnergyMeV = static_cast<IAEA_Float>(preE/MeV);
      break;
    case 2122:
      partType = 5;  // proton
      kinEnergyMeV = static_cast<IAEA_Float>( preE+(postE-preE)*(zStop-preZ)/(postZ-preZ) );
      break;
    default:
      G4cout << "ERROR: Particle code not supported at event ID "
	     << G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID()
	     << "." << G4endl;
      G4Exception("Particle code not supported in IAEA format");
    }

  // Track weight
  IAEA_Float wt = static_cast<IAEA_Float>(aTrack->GetWeight());

  // Position
  G4double postX = postR.x();
  G4double preX = preR.x();
  G4double postY = postR.y();
  G4double preY = preR.y();
  IAEA_Float x = static_cast<IAEA_Float>( (preX+(postX-preX)*(zStop-preZ)/(postZ-preZ))/cm );
  IAEA_Float y = static_cast<IAEA_Float>( (preY+(postY-preY)*(zStop-preZ)/(postZ-preZ))/cm );
  IAEA_Float z = static_cast<IAEA_Float>( zStop/cm );

  // Momentum direction
  G4ThreeVector momDir = aStep->GetPreStepPoint()->GetMomentumDirection();
  IAEA_Float u = static_cast<IAEA_Float>(momDir.x());
  IAEA_Float v = static_cast<IAEA_Float>(momDir.y());
  IAEA_Float w = static_cast<IAEA_Float>(momDir.z());

  // Extra variables
  IAEA_Float extraFloat = -1; // no extra floats stored
  IAEA_I32 extraInt = static_cast<IAEA_I32>((*theIncrNumberVector)[zStopIdx]);
  IAEA_I32 nStat = extraInt;

  // And finally store the particle following the IAEA routines
  iaea_write_particle(&sourceID, &nStat, &partType,
		      &kinEnergyMeV, &wt, &x, &y, &z, &u, &v, &w, &extraFloat, &extraInt);

}


//===========================================================
// void G4IAEAphspWriter::EndOfRunAction(const G4Run* )
//===========================================================
void G4IAEAphspWriter::EndOfRunAction(const G4Run* )
{
  // Close the IAEA files

  G4int size = theZStopVector->size();
  for (G4int ii = 0; ii < size; ii++)
    {
      const IAEA_I32 sourceID = static_cast<const IAEA_I32>(ii+theFilesAlreadyOpen);
      IAEA_I32 result;
      iaea_print_header(&sourceID, &result);
      if (result < 0)
	{
	  G4Exception("G4IAEAphspWriter::EndOfRunAction()\n phsp source does not exist");
	}

      iaea_destroy_source(&sourceID, &result);
      if (result > 0)
	{
	  G4cout << "Phase-space file at z = " << (*theZStopVector)[ii]/cm 
		 << " cm closed successfully!" 
		 << G4endl;    
	}
      else
	{
	  G4Exception("IAEA file not closed properly");
	}
    }

  // Clear the vectors at the end of the run

  theZStopVector->clear();
  theIncrNumberVector->clear();

  std::vector< std::set<G4int>* >::iterator is;
  for (is = thePassingTracksVector->begin();
       is != thePassingTracksVector->end(); is++)
    delete (*is);

  thePassingTracksVector->clear();
}
