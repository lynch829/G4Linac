// ========================================== //
//  by Miguel A. Cortes-Giraldo (03/04/2009)  //
// ========================================== //
//
// - 13/04/09: Messenger class added.
// - 17/10/09: Version 1.0
// - 20/11/09: Version 1.1 before publishing:
//        Changed some names by more suitable ones
// - 02/08/10: Version 1.2-dev:
//        Added possibility of applying axial symmetries
// - 17/08/10: Dividing weight by theTimesRecycled+1
//             Sizeof() not used when reserving memory for vectors
// - 19/08/10: 'Bug' fixed in IAEAreader011 exception
// - 23/04/12: Changed error codes
// - 08/05/12: Changed logic to choose the piece of file of parallel runs


#include "G4IAEAphspReader.hh"

#include "iaea_phsp.h"
#include "iaea_record.h"

#include <cstdlib>
#include <vector>

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"
#include "G4Event.hh"
#include "G4IAEAphspReaderMessenger.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
//#include "Randomize.hh"


// ==================================================================================
//  G4IAEAphspReader::G4IAEAphspReader(char* filename, const G4int sourceId)
// ==================================================================================

G4IAEAphspReader::G4IAEAphspReader(char* filename)
{
  theFileName = filename;
  //  theSourceReadId++;

  InitializeMembers();
  InitializeSource(filename);
  //MAC  ReadAndStoreFirstParticle();
}


// ===================================================================================
//  G4IAEAphspReader::G4IAEAphspReader(G4String filename, const G4int sourceId)
// ===================================================================================

G4IAEAphspReader::G4IAEAphspReader(G4String filename)
{
  theFileName = filename;
  char* fn = const_cast<char*>( filename.data() );

  //  theSourceReadId++;

  InitializeMembers();
  InitializeSource(fn);
  //MAC  ReadAndStoreFirstParticle();
}


// =============================================
//  G4IAEAphspReader::~G4IAEAphspReader()
// =============================================

G4IAEAphspReader::~G4IAEAphspReader()
{
  delete theMessenger;

  // clear and delete the vectors
  theParticleTypeVector->clear();
  theEnergyVector->clear();
  thePositionVector->clear();
  theMomentumVector->clear();
  theWeightVector->clear();

  if (theNumberOfExtraFloats > 0)
    {
      theExtraFloatsVector->clear();
      theExtraFloatsTypes->clear();
    }

  if (theNumberOfExtraInts > 0)
    {
      theExtraIntsVector->clear();
      theExtraIntsTypes->clear();
    }

  delete theParticleTypeVector;
  delete theEnergyVector;
  delete thePositionVector;
  delete theMomentumVector;
  delete theWeightVector;

  delete theExtraFloatsVector;
  delete theExtraIntsVector;

  delete theExtraFloatsTypes;
  delete theExtraIntsTypes;

  // IAEA file have to be closed
  const IAEA_I32 sourceRead = static_cast<const IAEA_I32>( theSourceReadId );
  IAEA_I32 result;

  iaea_destroy_source(&sourceRead, &result);

  if (result > 0)
    {
      G4cout << "File " << theFileName << ".IAEAphsp closed successfully!" << G4endl;
    }
  else
    {
      G4Exception("G4IAEAphspReader::~G4IAEAphspReader()",
		  "IAEAreader101", JustWarning, "IAEA file not closed properly");
    }

}


// ===================================================
//  void G4IAEAphspReader::InitializeMembers()
// ===================================================

void G4IAEAphspReader::InitializeMembers()
{
  G4ThreeVector zeroVec;
  G4ThreeVector yAxis(0.0, 1.0, 0.0);
  G4ThreeVector zAxis(0.0, 0.0, 1.0);

  particle_time = 0.0;
  particle_position = zeroVec;

  theOriginalHistories = -1;
  theTotalParticles = -1;
  theExtraFloatsTypes = new std::vector<G4int>;
  theExtraIntsTypes = new std::vector<G4int>;

  theParticleTypeVector = new std::vector<G4int>;
  theEnergyVector = new std::vector<G4double>;
  thePositionVector = new std::vector<G4ThreeVector>;
  theMomentumVector = new std::vector<G4ThreeVector>;
  theWeightVector = new std::vector<G4double>;
  theExtraFloatsVector = new std::vector< std::vector<G4double> >;
  theExtraIntsVector = new std::vector< std::vector<G4long> >;

  theTotalParallelRuns = 1;
  theParallelRun = 1;
  theTimesRecycled = 0;
  theUsedOriginalParticles = 0;
  theCurrentParticle = 0;
  theEndOfFile = false;
  theLastGenerated = true; //MAC to make the file 'restart' in the first event.

  theGlobalPhspTranslation = zeroVec;
  theRotationOrder = 123;
  theAlpha = theBeta = theGamma = 0.;
  theIsocenterPosition = zeroVec;
  theCollimatorAngle = theGantryAngle = 0.;
  theCollimatorRotAxis = zAxis;
  theGantryRotAxis = yAxis;

  theAxialSymmetryX = false;
  theAxialSymmetryY = false;
  theAxialSymmetryZ = false;

  // Messenger class
  theMessenger = new G4IAEAphspReaderMessenger(this);
}


// =========================================================
//  void G4IAEAphspReader::InitializeSource(char* filename)
// =========================================================

void G4IAEAphspReader::InitializeSource(char* filename)
{
  // Now try to open file, and check if all it's OK

  IAEA_I32 sourceRead = static_cast<IAEA_I32>( theSourceReadId );
  const IAEA_I32 accessRead = static_cast<const IAEA_I32>( theAccessRead );
  IAEA_I32 result;

  iaea_new_source( &sourceRead, filename, &accessRead, &result, theFileName.size()+1 );
  if ( sourceRead < 0 ) 
    G4Exception("G4IAEAphspReader::InitializeSource()",
		"IAEAreader102", RunMustBeAborted, "Could not open IAEA source file to read");

  iaea_check_file_size_byte_order(&sourceRead, &result);
  switch (result)
    {
    case -1:
      G4Exception("G4IAEAphspReader::InitializeSource()",
		  "IAEAreader103", RunMustBeAborted, "Header does not exist");
      break;
    case -2:
      G4Exception("G4IAEAphspReader::InitializeSource()",
		  "IAEAreader104", RunMustBeAborted, "Failure of function fseek");
      break;
    case -3:
      G4Exception("G4IAEAphspReader::InitializeSource()",
		  "IAEAreader105", RunMustBeAborted, "There is a file size mismatch");
      break;
    case -4:
      G4Exception("G4IAEAphspReader::InitializeSource()",
		  "IAEAreader106", RunMustBeAborted, "Byte order mismatch");
      break;
    case -5:
      G4Exception("G4IAEAphspReader::InitializeSource()",
		  "IAEAreader107", RunMustBeAborted, "File size and byte order mismatch");
      break;
    default:
      break;
    }


  // Now get the total number of particles stored in file

  IAEA_I32 particleType = -1; // To count all kind of particles
  IAEA_I64 nParticles;
  iaea_get_max_particles(&sourceRead, &particleType, &nParticles);

  if (nParticles < 0)
    {
      G4Exception("G4IAEAphspReader::InitializeSource()",
		  "IAEAreader108", RunMustBeAborted, "Header file does not exist");
    }
  else
    {
      theTotalParticles = static_cast<G4long>(nParticles);
      G4cout << "Total number of particles in file " << filename << ".IAEAphsp" << " = " 
	     << GetTotalParticles() << G4endl;
    }


  // And now store the number of original simulated histories

  IAEA_I64 nHistories;
  iaea_get_total_original_particles(&sourceRead, &nHistories);
  theOriginalHistories = static_cast<G4long>(nHistories);

  G4cout << "Number of original histories in header file " << filename << ".IAEAphsp" << " = " 
	 << nHistories << G4endl;


  // And finally, we take all the information related to the extra variables

  IAEA_I32 nExtraFloat, nExtraInt;
  iaea_get_extra_numbers(&sourceRead, &nExtraFloat, &nExtraInt );
  theNumberOfExtraFloats = static_cast<G4int>( nExtraFloat );
  theNumberOfExtraInts = static_cast<G4int>( nExtraInt );

  G4cout << "The number of Extra Floats is " << theNumberOfExtraFloats
	 << " and the number of Extra Ints is " << theNumberOfExtraInts << G4endl;

  // Initializing vectors

  IAEA_I32 extraFloatsTypes[NUM_EXTRA_FLOAT], extraIntsTypes[NUM_EXTRA_LONG];
  iaea_get_type_extra_variables(&sourceRead, &result, extraIntsTypes, extraFloatsTypes);

  if (theNumberOfExtraFloats > 0)
    {
      theExtraFloatsTypes->reserve( theNumberOfExtraFloats );

      for (G4int j = 0; j < theNumberOfExtraFloats; j++)
	{
	  theExtraFloatsTypes->push_back( static_cast<G4int>(extraFloatsTypes[j]) );
	}
    }

  if (theNumberOfExtraInts > 0)
    {
      theExtraIntsTypes->reserve( theNumberOfExtraInts );

      for (G4int i = 0; i < theNumberOfExtraInts; i++)
	{
	  theExtraIntsTypes->push_back( static_cast<G4int>(extraIntsTypes[i]) );
	}
    }
}


// ===============================================================
//  void G4IAEAphspReader::GeneratePrimaryVertex(G4Event* evt)
// ===============================================================
// This is the pure virtual method inherited from G4VUserPrimaryGeneratorAction
// and it is meant to be a template method.
void G4IAEAphspReader::GeneratePrimaryVertex(G4Event* evt)
{
  if (theLastGenerated) 
    {
      RestartSourceFile();
      ReadAndStoreFirstParticle();
    }

  PrepareThisEvent();

  if (theNStat == 0)
    {
      ReadThisEvent();
      GeneratePrimaryParticles(evt);
    }
}


// ==============================================================================
//  void G4IAEAphspReader::RestartSourceFile()
// ==============================================================================

void G4IAEAphspReader::RestartSourceFile()
{
  // Restart counters and flags
  theCurrentParticle = 0;
  theEndOfFile = false;
  theLastGenerated = false;

  // Clear all the vectors
  theParticleTypeVector->clear();
  theEnergyVector->clear();
  thePositionVector->clear();
  theMomentumVector->clear();
  theWeightVector->clear();
  if (theNumberOfExtraFloats) theExtraFloatsVector->clear();
  if (theNumberOfExtraInts) theExtraIntsVector->clear();

  // Close and open the IAEA source
  char* filename = const_cast<char*>( theFileName.data() );
  IAEA_I32 accessRead = static_cast<IAEA_I32>( theAccessRead );
  IAEA_I32 sourceRead = static_cast<IAEA_I32>(theSourceReadId);
  IAEA_I32 result;
  iaea_destroy_source(&sourceRead, &result);
  iaea_new_source(&sourceRead, filename, &accessRead, &result, theFileName.size() );
}


// ===============================================================
//  void G4IAEAphspReader::ReadAndStoreFirstParticle()
// ===============================================================

void G4IAEAphspReader::ReadAndStoreFirstParticle()
{
  // Particle properties and variable declaration
  IAEA_I32 type, nStat;
  IAEA_Float E, wt, x, y, z, u, v, w;
  IAEA_Float extraFloats[NUM_EXTRA_FLOAT];
  IAEA_I32 extraInts[NUM_EXTRA_LONG]; 

  IAEA_I32 result;
  IAEA_I32 sourceRead = static_cast<IAEA_I32>(theSourceReadId);

  // ------------------------------
  //  Go to the suitable chunk
  // ------------------------------

  const IAEA_I32 parallelRun = static_cast<const IAEA_I32>(theParallelRun);
  const IAEA_I32 totalParallelRuns = static_cast<const IAEA_I32>(theTotalParallelRuns);
  theCurrentParticle = theTotalParticles/theTotalParallelRuns*(theParallelRun-1);

  iaea_set_parallel(&sourceRead, &parallelRun, &parallelRun,
		    &totalParallelRuns, &result);

  /* ---
   * MACG 2012-05-08 This while loopbecomes too slow with large phsp files
   *   It has been substituted by the lines above
   *
  G4int initParticle = theTotalParticles/theTotalParallelRuns*(theParallelRun-1);
  while (theCurrentParticle < initParticle)
    {
      theCurrentParticle++;
      iaea_get_particle(&sourceRead, &nStat, &type,
			&E, &wt, &x, &y, &z, &u, &v, &w, extraFloats, extraInts);
    }
  */

  // -------------------------------------------------
  //  Obtain all the information needed from the file
  // -------------------------------------------------

  // read IAEA particle
  theCurrentParticle++; // to keep the position of this particle in the PSF
  iaea_get_particle(&sourceRead, &nStat, &type, 
		    &E, &wt, &x, &y, &z, &u, &v, &w, extraFloats, extraInts);

  if (nStat == -1)
    {
      G4Exception("G4IAEAphspReader::ReadAndStoreFirstParticle()",
		  "IAEAreader109", RunMustBeAborted, "Cannot find source file");
    }
  else if (nStat == 0)
    {
      theNStat = 1; // needed to set this first particle as new event
    }
  else
    {
      theNStat = nStat;
      // important to calculate correlations properly
    }

  // -------------------------------------------------
  //  Store the information into the data members
  // -------------------------------------------------

  theParticleTypeVector->push_back(static_cast<G4int>(type) );
  theEnergyVector->push_back(static_cast<G4double>(E));

  G4ThreeVector pos, mom;
  pos.set(static_cast<G4double>(x), 
	  static_cast<G4double>(y),
	  static_cast<G4double>(z));

  mom.set(static_cast<G4double>(u),
	  static_cast<G4double>(v),
	  static_cast<G4double>(w));

  thePositionVector->push_back(pos);
  theMomentumVector->push_back(mom);

  theWeightVector->push_back(static_cast<G4double>(wt));

  if (theNumberOfExtraFloats > 0)
    {
      std::vector<G4double> vExtraFloats;
      vExtraFloats.reserve(theNumberOfExtraFloats);

      for (G4int j = 0; j < theNumberOfExtraFloats; j++)
	{
	  vExtraFloats.push_back( static_cast<G4double>(extraFloats[j]) );
	}
      theExtraFloatsVector->push_back(vExtraFloats);
    }

  if (theNumberOfExtraInts > 0)
    {
      std::vector<G4long> vExtraInts;
      vExtraInts.reserve(theNumberOfExtraInts);

      for (G4int i = 0; i < theNumberOfExtraInts; i++)
	{
	  vExtraInts.push_back( static_cast<G4long>(extraInts[i]) );
	}
      theExtraIntsVector->push_back(vExtraInts);
    }

  //  Update theUsedOriginalParticles
  // ---------------------------------

  IAEA_I64 nUsedOriginal;
  iaea_get_used_original_particles(&sourceRead, &nUsedOriginal);
  theUsedOriginalParticles = static_cast<G4long>(nUsedOriginal);

  //  Safety check in case that only one particle is stored
  //  or only one particle can be taken into consideration.
  // -------------------------------------------------------

  G4int endParticle = theTotalParticles/theTotalParallelRuns*theParallelRun;
  if (theCurrentParticle == endParticle) 
    theEndOfFile = true;
}


// ===============================================================
//  void G4IAEAphspReader::PrepareThisEvent()
// ===============================================================

void G4IAEAphspReader::PrepareThisEvent()
{
  theNStat--;  // A new event begins

  // Erase all the elements but the last in the vectors
  // In case that the size of the lists is just 1, there's no need
  // to clean up.

  G4int listSizes = theParticleTypeVector->size();

  if (listSizes > 1)
    {
      theParticleTypeVector->erase(theParticleTypeVector->begin(),
				   theParticleTypeVector->end()-1);
      theEnergyVector->erase(theEnergyVector->begin(),
			     theEnergyVector->end()-1);
      thePositionVector->erase(thePositionVector->begin(),
			       thePositionVector->end()-1);
      theMomentumVector->erase(theMomentumVector->begin(),
			       theMomentumVector->end()-1);
      theWeightVector->erase(theWeightVector->begin(),
			     theWeightVector->end()-1);

      if (theNumberOfExtraFloats > 0)
	theExtraFloatsVector->erase(theExtraFloatsVector->begin(),
				    theExtraFloatsVector->end()-1);

      if (theNumberOfExtraInts > 0)
	theExtraIntsVector->erase(theExtraIntsVector->begin(),
				  theExtraIntsVector->end()-1);
    }
}


// ===============================================================
//  void G4IAEAphspReader::ReadThisEvent()
// ===============================================================

void G4IAEAphspReader::ReadThisEvent()
{
  // Particle properties
  IAEA_I32 type, nStat;
  IAEA_Float E, wt, x, y, z, u, v, w;
  IAEA_I32 extraInts[NUM_EXTRA_LONG]; 
  IAEA_Float extraFloats[NUM_EXTRA_FLOAT];

  IAEA_I32 sourceRead = static_cast<IAEA_I32>(theSourceReadId);

  // the position of the last particle in this chunk
  G4int endParticle = theTotalParticles/theTotalParallelRuns*theParallelRun;

  // -------------------------------------------------
  //  Obtain all the information needed from the file
  // -------------------------------------------------

  while (theNStat == 0 && !theEndOfFile)
    {
      //  Read next IAEA particle
      // -------------------------

      theCurrentParticle++;
      iaea_get_particle(&sourceRead, &nStat, &type, 
			&E, &wt, &x, &y, &z, &u, &v, &w, extraFloats, extraInts);

      if (nStat == -1)
	{
	  G4Exception("G4IAEAphspReader::ReadThisEvent()",
		      "IAEAreader110", RunMustBeAborted, "Cannot find source file");
	}
      else
	{
	  theNStat += nStat;  // statistical book-keeping
	}

      //  Store the information into the data members
      // ---------------------------------------------

      theParticleTypeVector->push_back(static_cast<G4int>(type) );
      theEnergyVector->push_back(static_cast<G4double>(E));

      G4ThreeVector pos, mom;
      pos.set(static_cast<G4double>(x), 
	      static_cast<G4double>(y),
	      static_cast<G4double>(z));

      mom.set(static_cast<G4double>(u),
	      static_cast<G4double>(v),
	      static_cast<G4double>(w));

      thePositionVector->push_back(pos);
      theMomentumVector->push_back(mom);

      theWeightVector->push_back(static_cast<G4double>(wt));

      if (theNumberOfExtraFloats > 0)
	{
	  std::vector<G4double> vExtraFloats;
	  vExtraFloats.reserve(theNumberOfExtraFloats);
	  for (G4int j = 0; j < theNumberOfExtraFloats; j++)
	    {
	      vExtraFloats.push_back( static_cast<G4double>(extraFloats[j]) );
	    }
	  theExtraFloatsVector->push_back(vExtraFloats);
	}

      if (theNumberOfExtraInts > 0)
	{
	  std::vector<G4long> vExtraInts;
	  vExtraInts.reserve(theNumberOfExtraInts);
	  for (G4int i = 0; i < theNumberOfExtraInts; i++)
	    {
	      vExtraInts.push_back( static_cast<G4long>(extraInts[i]) );
	    }
	  theExtraIntsVector->push_back(vExtraInts);
	}

      //  Update theUsedOriginalParticles
      // ---------------------------------

      IAEA_I64 nUsedOriginal;
      iaea_get_used_original_particles(&sourceRead, &nUsedOriginal);
      theUsedOriginalParticles = static_cast<G4long>(nUsedOriginal);

      //  Check whether the end of chunk has been reached
      // -------------------------------------------------
      if (theCurrentParticle == endParticle)
	theEndOfFile = true;
    }
}


// ===============================================================
//  void G4IAEAphspReader::GeneratePrimaryParticles(G4Event* evt)
// ===============================================================

void G4IAEAphspReader::GeneratePrimaryParticles(G4Event* evt)
{
  // -----------------------------------------------------------
  // Only when the EOF has been reached and theNStat is still 0 
  // all the particles must be read.
  // Otherwise, don't read the last particle.
  // -----------------------------------------------------------

  G4int listSize = theParticleTypeVector->size();

  if (theEndOfFile && theNStat == 0)
    {
      // Read all the particles, so this flag switches on
      theLastGenerated = true;

      // Throw a warning due to the following restarting
      G4cout << "WARNING: End of Phase-space file reached during event "
	     << evt->GetEventID() << "."
	     << G4endl;
    }
  else
    {
      // The last particle belongs to a later event
      listSize--;
    }

  // ---------------------------------------------------------------------
  // Generate the random rotations if any rotational symmetry is applied.
  // This is applied when recycling particles.
  // In order to keep correlations, the same rotation must be applied to
  // all the particles of the same original history during same re-use.
  // ---------------------------------------------------------------------

  std::vector<G4double> randomRotations;

  if (theAxialSymmetryZ || theAxialSymmetryX || theAxialSymmetryY)
    {
      randomRotations.reserve(theTimesRecycled+1);
      for (G4int r = 0; r <= theTimesRecycled; r++)
	{
	 // G4double randomAngle=0 ;
	 // randomAngle *= (360.*deg);
	  randomRotations.push_back(12*(r+1));
	}
    }

  // ---------------------------------------------
  // loop over all the particles obtained from PSF
  // ---------------------------------------------
  for (G4int k = 0; k < listSize; k++)
    {
      // First: Particle Definition
      // --------------------------

      G4ParticleDefinition * partDef = 0;
      switch((*theParticleTypeVector)[k])
	{
	case 1:
	  partDef = G4Gamma::Definition();
	  break;
	case 2:
	  partDef = G4Electron::Definition();
	  break;
	case 3:
	  partDef = G4Positron::Definition();
	  break;
	case 4:
	  partDef = G4Neutron::Definition();
	  break;
	case 5:
	  partDef = G4Proton::Definition();
	default:
	  G4cout << "Exception occurred at event " << evt->GetEventID() << "\n"
		 << "reading particle code " << (*theParticleTypeVector)[k] << "." << G4endl;
	  G4Exception("G4IAEAphspReader::GeneratePrimaryParticles()",
		      "IAEAreader111", EventMustBeAborted, "Unknown particle code in IAEA file");
	  return;
	}

      // Second: Particle position, time and momentum
      // --------------------------------------------

      particle_position = (*thePositionVector)[k];
      particle_position *= cm;        // IAEA file stores in cm

      G4double partMass = partDef->GetPDGMass();
      G4double partEnergy = static_cast<G4double>((*theEnergyVector)[k])*MeV + partMass;
      G4double partMom = std::sqrt( partEnergy*partEnergy - partMass*partMass );

      G4ThreeVector partMomVect;
      partMomVect = (*theMomentumVector)[k];
      partMomVect *= partMom;

      // Third: Translation and rotations
      // ---------------------------------

      // Translation is performed before rotations
      particle_position += theGlobalPhspTranslation;
      PerformRotations(partMomVect);

      // -------------------------------------------------
      //  Creation of the new primary particle and vertex
      // -------------------------------------------------

      // loop to take care of recycling
      for (G4int r = 0; r <= theTimesRecycled; r++) 
	{
	  // Apply the rotational symmetries if they are applicable
	  if (theAxialSymmetryZ)
	    {
	      particle_position.rotateZ(randomRotations[r]);
	      partMomVect.rotateZ(randomRotations[r]);
	    }
	  else if (theAxialSymmetryX)
	    {
	      particle_position.rotateX(randomRotations[r]);
	      partMomVect.rotateX(randomRotations[r]);
	    }
	  else if (theAxialSymmetryY)
	    {
	      particle_position.rotateY(randomRotations[r]);
	      partMomVect.rotateY(randomRotations[r]);
	    }

	  // Create the new primary particle
	  G4PrimaryParticle * particle =
	    new G4PrimaryParticle(partDef, partMomVect.x(), partMomVect.y(), partMomVect.z());

	  particle->SetWeight( ((*theWeightVector)[k])/(theTimesRecycled+1) );

	  // Create the new primary vertex and set the primary to it
	  G4PrimaryVertex * vertex = new G4PrimaryVertex(particle_position, particle_time);
	  vertex->SetPrimary(particle);

	  // And finally set the vertex to this event
	  evt->AddPrimaryVertex(vertex);
	}
    }
}


// ==========================================================================
//  void G4IAEAphspReader::PerformRotations(G4ThreeVector & mom)
// ==========================================================================

void G4IAEAphspReader::PerformRotations(G4ThreeVector & mom)
{
  PerformGlobalRotations(mom);
  PerformHeadRotations(mom);
}


// ============================================================================
//  void G4IAEAphspReader::PerformGlobalRotations(G4ThreeVector & momentum)
// ============================================================================

void G4IAEAphspReader::PerformGlobalRotations(G4ThreeVector & momentum)
{
  switch (theRotationOrder)
    {
    case 123:
      particle_position.rotateX(theAlpha);
      particle_position.rotateY(theBeta);
      particle_position.rotateZ(theGamma);
      momentum.rotateX(theAlpha);
      momentum.rotateY(theBeta);
      momentum.rotateZ(theGamma);
      break;

    case 132:
      particle_position.rotateX(theAlpha);
      particle_position.rotateZ(theGamma);
      particle_position.rotateY(theBeta);
      momentum.rotateX(theAlpha);
      momentum.rotateZ(theGamma);
      momentum.rotateY(theBeta);
      break;

    case 213:
      particle_position.rotateY(theBeta);
      particle_position.rotateX(theAlpha);
      particle_position.rotateZ(theGamma);
      momentum.rotateY(theBeta);
      momentum.rotateX(theAlpha);
      momentum.rotateZ(theGamma);
      break;

    case 231:
      particle_position.rotateY(theBeta);
      particle_position.rotateZ(theGamma);
      particle_position.rotateX(theAlpha);
      momentum.rotateY(theBeta);
      momentum.rotateZ(theGamma);
      momentum.rotateX(theAlpha);
      break;

    case 312:
      particle_position.rotateZ(theGamma);
      particle_position.rotateX(theAlpha);
      particle_position.rotateY(theBeta);
      momentum.rotateZ(theGamma);
      momentum.rotateX(theAlpha);
      momentum.rotateY(theBeta);
      break;

    case 321:
      particle_position.rotateZ(theGamma);
      particle_position.rotateY(theBeta);
      particle_position.rotateX(theAlpha);
      momentum.rotateZ(theGamma);
      momentum.rotateY(theBeta);
      momentum.rotateX(theAlpha);
      break;

    default:
      G4cout << "WARNING: invalid combination in G4IAEAphspReader::theRotationOrder\n"
	     << "so no global rotations are performed" << G4endl;
    }
}


// ==========================================================================
//  void G4IAEAphspReader::PerformHeadRotations(G4ThreeVector & mom)
// ==========================================================================

void G4IAEAphspReader::PerformHeadRotations(G4ThreeVector & momentum)
{
  // First we need the position related to the isocenter
  particle_position -= theIsocenterPosition;

  // And now comes both rotations whose axis contain the isocenter

  // First is the rotation of the collimator around its defined axis
  particle_position.rotate(theCollimatorRotAxis, theCollimatorAngle);
  momentum.rotate(theCollimatorRotAxis, theCollimatorAngle);

  // The rotation of the complete machine is applied afterwards
  particle_position.rotate(theGantryRotAxis, theGantryAngle);
  momentum.rotate(theGantryRotAxis, theGantryAngle);

  // And finally, restore the position to the global coordinate system
  particle_position += theIsocenterPosition;
}


// =================================================================================
//  void G4IAEAphspReader::SetCollimatorRotationAxis(const G4ThreeVector & axis)
// =================================================================================

void G4IAEAphspReader::SetCollimatorRotationAxis(const G4ThreeVector & axis)
{
  G4double mod = axis.mag();
  if ( mod > 0 )
    {
      // Normalization
      G4double scale = 1.0/mod;
      G4double ux = scale*axis.getX();
      G4double uy = scale*axis.getY();
      G4double uz = scale*axis.getZ();
      theCollimatorRotAxis.setX(ux);
      theCollimatorRotAxis.setY(uy);
      theCollimatorRotAxis.setZ(uz);
    }
  else
    {
      G4cout << "Trying to set as CollimatorRotationAxis a null vector,\n"
	     << "so the previous value will remain" << G4endl;
    } 
}


// ==============================================================================
//  void G4IAEAphspReader::SetGantryRotationAxis(const G4ThreeVector & axis)
// ==============================================================================

void G4IAEAphspReader::SetGantryRotationAxis(const G4ThreeVector & axis)
{
  G4double mod = axis.mag();
  if ( mod > 0 )
    {
      // Normalization
      G4double scale = 1.0/mod;
      G4double ux = scale*axis.getX();
      G4double uy = scale*axis.getY();
      G4double uz = scale*axis.getZ();
      theGantryRotAxis.setX(ux);
      theGantryRotAxis.setY(uy);
      theGantryRotAxis.setZ(uz);
    }
  else
    {
      G4cout << "Trying to set as GantryRotationAxis a null vector,\n"
	     << "so the previous value will remain" << G4endl;
    } 
}


// ==========================================================================
//  G4long G4IAEAphspReader::GetTotalParticlesOfType(G4String type) const
// ==========================================================================

G4long G4IAEAphspReader::GetTotalParticlesOfType(G4String type) const
{
  IAEA_I32 particleType;

  if (type == "ALL") particleType = -1;
  else if (type == "PHOTON") particleType = 1;
  else if (type == "ELECTRON") particleType = 2;
  else if (type == "POSITRON") particleType = 3;
  else if (type == "NEUTRON") particleType = 4;
  else if (type == "PROTON") particleType = 5;
  else particleType = 0;

  if (particleType == 0) 
    {
      G4Exception("G4IAEAphspReader::GetTotalParticlesOfType()",
		  "IAEAreader112", JustWarning, "Particle type not valid. Return value -1");
      return -1;
    }

  IAEA_I64 nParticles;
  const IAEA_I32 sourceRead = static_cast<const IAEA_I32>( theSourceReadId );
  iaea_get_max_particles(&sourceRead, &particleType, &nParticles);

  if (nParticles < 0)
    {
      G4Exception("G4IAEAphspReader::GetTotalParticlesOfType()",
		  "IAEAreader113", JustWarning, "IAEA header file not found");
    }
  else
    {
      G4cout << "Total number of " << type << " in filename " 
	     << theFileName << ".IAEAphsp" << " = " << nParticles << G4endl;
    }

  return ( static_cast<G4long>(nParticles) );
}


// ==========================================================================
//  G4double G4IAEAphspReader::GetConstantVariable(const G4int index) const
// ==========================================================================

G4double G4IAEAphspReader::GetConstantVariable(const G4int index) const
{
  const IAEA_I32 sourceRead = static_cast<const IAEA_I32>( theSourceReadId );
  const IAEA_I32 idx = static_cast<const IAEA_I32>( index );
  IAEA_Float constVariable;
  IAEA_I32 result;

  iaea_get_constant_variable(&sourceRead, &idx, &constVariable, &result);

  if (result == -1)
    {
      G4Exception("G4IAEAphspReader::GetConstantVariable()",
		  "IAEAreader114", JustWarning, "IAEA header file source not found");
    }
  else if (result == -2)
    {
      G4Exception("G4IAEAphspReader::GetConstantVariable()",
		  "IAEAreader115", JustWarning, "index out of range");
    }
  else if (result == -3)
    {
      G4Exception("G4IAEAphspReader::GetConstantVariable()",
		  "IAEAreader116", JustWarning, "parameter indicated by 'index' is not a constant");
    }

  return ( static_cast<G4double>(constVariable) );
}
