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


#ifndef G4IAEAphspWriter_hh
#define G4IAEAphspWriter_hh 1

#include "G4ThreeVector.hh"
#include "globals.hh"

#include <set>
#include <vector>

class G4Event;
class G4Run;
class G4Step;


class G4IAEAphspWriter
{
private:
  static G4IAEAphspWriter *theInstance;
  G4IAEAphspWriter();

public:
  ~G4IAEAphspWriter();

public:
  static G4IAEAphspWriter * GetInstance();

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);
  void BeginOfEventAction(const G4Event*);
  void UserSteppingAction(const G4Step*);

private:
  void StoreIAEAParticle(const G4Step* aStep, const G4int zStopIdx);

public:
  void SetZStop(G4double zstop);
  void UpdateHeaders();

public: // GET-SET methods

  void SetFileName(G4String value) {theFileName = value;}

  G4String GetFileName() const {return theFileName;}
  std::vector<G4double>* GetZStopVector() const {return theZStopVector;}


private: // DATA MEMBERS

  // ----------------
  // FILE PROPERTIES
  // ----------------

  G4String theFileName;
  // Must include the path but not any of the IAEA extensions.

  std::vector<G4double>* theZStopVector;
  // Vector which stores the Zstops. They must be defined in the user code.

  static const G4int theAccessWrite = 2;
  // Value needed in the IAEA routines to open a source for writing.

  // --------------------
  // COUNTERS AND FLAGS
  // --------------------

  std::vector<G4int>* theIncrNumberVector;
  // Book-keeping of the number of event without particles crossing the plane.

  std::vector< std::set<G4int>* >* thePassingTracksVector;
  // Each set is meant to store the track ID of every particle
  // crossing one of the planes.

  G4int theFilesAlreadyOpen;
  // This is a counter to consider whether there is another IAEA file
  // open, e. g. a IAEA PSF which data are taken from.

  // --------------------
  // STEP PROPERTIES
  // --------------------

  G4ThreeVector preR;
  G4ThreeVector postR;
  // Current pre- and post-step points

};

#endif
