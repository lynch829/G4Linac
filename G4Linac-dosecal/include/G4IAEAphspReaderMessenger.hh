// ============================= //
//  by Miguel A. Cortes-Giraldo  //
// ============================= //
//
// - 17/10/09: version 1.0
// - 20/11/09: version 1.1 before publishing:
//        changed some names by more suitable ones
// - 02/08/10: version 1.2-dev:
//        added possibility of applying axial symmetries

#ifndef G4IAEAphspReaderMessenger_h
#define G4IAEAphspReaderMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4IAEAphspReader;

class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcommand;
class G4UIdirectory;

class G4IAEAphspReaderMessenger: public G4UImessenger
{
public:
  G4IAEAphspReaderMessenger(G4IAEAphspReader* );
  ~G4IAEAphspReaderMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:

  G4IAEAphspReader* theIAEAphspReader;
  // Pointer to the IAEA phase-space reader.
  
  G4UIdirectory* phaseSpaceDir;
  // Control of the phase space

  G4UIcmdWithAnInteger* nOfParallelRunsCmd;
  // UI command to define the number of fragments defined in the file.

  G4UIcmdWithAnInteger* parallelRunCmd;
  // UI command to choose the specific fragment where the particles are taken from.

  G4UIcmdWithAnInteger* timesRecycledCmd;
  // UI command to set the number of times each particle is recycled (not repeated).

  G4UIcmdWith3VectorAndUnit* phspGlobalTranslationCmd;
  // UI command to set the three-vector to move the phase-space plane globally.

  G4UIcmdWithAnInteger* phspRotationOrderCmd;
  // UI command to set the order in which the rotations are performed.

  G4UIcmdWithADoubleAndUnit* rotXCmd;
  // UI command to set the rotation angle around X axis.

  G4UIcmdWithADoubleAndUnit* rotYCmd;
  // UI command to set the rotation angle around Y axis.

  G4UIcmdWithADoubleAndUnit* rotZCmd;
  // UI command to set the rotation angle around Z axis.

  G4UIcmdWith3VectorAndUnit* isocenterPosCmd;
  // UI command to set where the isocenter is.

  G4UIcmdWith3Vector* collimatorRotAxisCmd;
  // UI command to set the rotation axis of the collimator.

  G4UIcmdWithADoubleAndUnit* collimatorAngleCmd;
  // UI command to set the rotation angle of the treatment head.

  G4UIcmdWith3Vector* gantryRotAxisCmd;
  // UI command to set the rotation axis of the gantry.

  G4UIcmdWithADoubleAndUnit* gantryAngleCmd;
  // UI command to set the rotation angle of the gantry.

  G4UIcmdWithABool* axialSymmetryXCmd;
  // UI command to turn on/off the rotational symmetry around X.

  G4UIcmdWithABool* axialSymmetryYCmd;
  // UI command to turn on/off the rotational symmetry around Y.

  G4UIcmdWithABool* axialSymmetryZCmd;
  // UI command to turn on/off the rotational symmetry around Z.
};
#endif

