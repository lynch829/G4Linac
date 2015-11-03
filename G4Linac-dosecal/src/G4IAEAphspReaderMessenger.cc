// ============================= //
//  by Miguel A. Cortes-Giraldo  //
// ============================= //
//
// - 17/10/09: version 1.0
// - 20/11/09: version 1.1 before publishing:
//        changed some names by more suitable ones
// - 02/08/10: version 1.2-dev:
//        added possibility of applying axial symmetries


#include "G4IAEAphspReaderMessenger.hh"
#include "G4IAEAphspReader.hh"

#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"

G4IAEAphspReaderMessenger::G4IAEAphspReaderMessenger(G4IAEAphspReader* reader)
  :theIAEAphspReader(reader)
{
  phaseSpaceDir = new G4UIdirectory("/IAEAphsp/");
  phaseSpaceDir -> SetGuidance("Commands for the IAEA phase-space file management.");

  nOfParallelRunsCmd = new G4UIcmdWithAnInteger("/IAEAphsp/numberOfParallelRuns", this);
  nOfParallelRunsCmd -> SetGuidance("Select the number of fragments in which the phase-space file is divided into.");
  nOfParallelRunsCmd -> SetParameterName("choice", false);
  nOfParallelRunsCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  parallelRunCmd = new G4UIcmdWithAnInteger("/IAEAphsp/parallelRun", this);
  parallelRunCmd -> SetGuidance("Select the fragment where particles are taken from.");
  parallelRunCmd -> SetParameterName("choice", false);
  parallelRunCmd -> SetRange("choice > 0");
  parallelRunCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  timesRecycledCmd = new G4UIcmdWithAnInteger("/IAEAphsp/recycling", this);
  timesRecycledCmd -> SetGuidance("Select the number of times that each particle is recycled.");
  timesRecycledCmd -> SetGuidance("(Be careful: 1 means that each particle is used twice.)");
  timesRecycledCmd -> SetParameterName("choice", false);
  timesRecycledCmd -> SetRange("choice >= 0");
  timesRecycledCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  phspGlobalTranslationCmd = new G4UIcmdWith3VectorAndUnit("/IAEAphsp/translate", this);
  phspGlobalTranslationCmd -> SetGuidance("Set the translation components.");
  phspGlobalTranslationCmd -> SetParameterName("x0", "y0", "z0", false);
  phspGlobalTranslationCmd -> SetDefaultUnit("cm");
  phspGlobalTranslationCmd -> SetUnitCandidates("mm cm m");
  phspGlobalTranslationCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  phspRotationOrderCmd = new G4UIcmdWithAnInteger("/IAEAphsp/rotationOrder", this);
  phspRotationOrderCmd -> SetGuidance("Select the order in which the rotations are performed.");
  phspRotationOrderCmd -> SetGuidance("1 means X axis, 2 means Y axis and 3 means Z axis.");
  phspRotationOrderCmd -> SetGuidance("The argument must be a 3-digit integer without repetition.");
  phspRotationOrderCmd -> SetParameterName("choice", false);
  phspRotationOrderCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  rotXCmd = new G4UIcmdWithADoubleAndUnit("/IAEAphsp/rotateX", this);
  rotXCmd -> SetGuidance("Set the rotation angle around the global X axis.");
  rotXCmd -> SetParameterName("angle", false);
  rotXCmd -> SetDefaultUnit("deg");
  rotXCmd -> SetUnitCandidates("deg rad");
  rotXCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  rotYCmd = new G4UIcmdWithADoubleAndUnit("/IAEAphsp/rotateY", this);
  rotYCmd -> SetGuidance("Set the rotation angle around the global Y axis.");
  rotYCmd -> SetParameterName("angle", false);
  rotYCmd -> SetDefaultUnit("deg");
  rotYCmd -> SetUnitCandidates("deg rad");
  rotYCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  rotZCmd = new G4UIcmdWithADoubleAndUnit("/IAEAphsp/rotateZ", this);
  rotZCmd -> SetGuidance("Set the rotation angle around the global Z axis.");
  rotZCmd -> SetParameterName("angle", false);
  rotZCmd -> SetDefaultUnit("deg");
  rotZCmd -> SetUnitCandidates("deg rad");
  rotZCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  isocenterPosCmd = new G4UIcmdWith3VectorAndUnit("/IAEAphsp/isocenterPosition", this);
  isocenterPosCmd -> SetGuidance("Set the isocenter position.");
  isocenterPosCmd -> SetParameterName("Xic", "Yic", "Zic", false);
  isocenterPosCmd -> SetDefaultUnit("cm");
  isocenterPosCmd -> SetUnitCandidates("mm cm m");
  isocenterPosCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  collimatorRotAxisCmd = new G4UIcmdWith3Vector("/IAEAphsp/collimatorRotationAxis", this);
  collimatorRotAxisCmd -> SetGuidance("Set the rotation axis of the collimator.");
  collimatorRotAxisCmd -> SetGuidance("It has to be a unit vector.");
  collimatorRotAxisCmd -> SetParameterName("Ucol", "Vcol", "Wcol", false);
  collimatorRotAxisCmd -> SetRange("Ucol != 0 || Vcol != 0 || Wcol != 0");
  collimatorRotAxisCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  collimatorAngleCmd = new G4UIcmdWithADoubleAndUnit("/IAEAphsp/collimatorAngle", this);
  collimatorAngleCmd -> SetGuidance("Set the rotation angle of the phase space plane around the rotation axis of the collimator.");
  collimatorAngleCmd -> SetParameterName("angle", false);
  collimatorAngleCmd -> SetDefaultUnit("deg");
  collimatorAngleCmd -> SetUnitCandidates("deg rad");
  collimatorAngleCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  gantryRotAxisCmd = new G4UIcmdWith3Vector("/IAEAphsp/gantryRotationAxis", this);
  gantryRotAxisCmd -> SetGuidance("Set the rotation axis of the gantry.");
  gantryRotAxisCmd -> SetGuidance("It has to be a unit vector.");
  gantryRotAxisCmd -> SetParameterName("Ugan", "Vgan", "Wgan", false);
  gantryRotAxisCmd -> SetRange("Ugan != 0 || Vgan != 0 || Wgan != 0");
  gantryRotAxisCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  gantryAngleCmd = new G4UIcmdWithADoubleAndUnit("/IAEAphsp/gantryAngle", this);
  gantryAngleCmd -> SetGuidance("Set the rotation angle of the phase space plane around the gantry rotation axis.");
  gantryAngleCmd -> SetParameterName("angle", false);
  gantryAngleCmd -> SetDefaultUnit("deg");
  gantryAngleCmd -> SetUnitCandidates("deg rad");
  gantryAngleCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  axialSymmetryXCmd = new G4UIcmdWithABool("/IAEAphsp/axialSymmetryX", this);
  axialSymmetryXCmd -> SetGuidance("Command to take into account rotational symmetry around X");
  axialSymmetryXCmd -> SetParameterName("choice", true);
  axialSymmetryXCmd -> SetDefaultValue(true);
  axialSymmetryXCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  axialSymmetryYCmd = new G4UIcmdWithABool("/IAEAphsp/axialSymmetryY", this);
  axialSymmetryYCmd -> SetGuidance("Command to take into account rotational symmetry around Y");
  axialSymmetryYCmd -> SetParameterName("choice", true);
  axialSymmetryYCmd -> SetDefaultValue(true);
  axialSymmetryYCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);

  axialSymmetryZCmd = new G4UIcmdWithABool("/IAEAphsp/axialSymmetryZ", this);
  axialSymmetryZCmd -> SetGuidance("Command to take into account rotational symmetry around Z");
  axialSymmetryZCmd -> SetParameterName("choice", true);
  axialSymmetryZCmd -> SetDefaultValue(true);
  axialSymmetryZCmd -> AvailableForStates(G4State_PreInit, G4State_Idle);
}


G4IAEAphspReaderMessenger::~G4IAEAphspReaderMessenger()
{ 
  delete phaseSpaceDir;
  delete nOfParallelRunsCmd;
  delete parallelRunCmd;
  delete timesRecycledCmd;
  delete phspGlobalTranslationCmd;
  delete phspRotationOrderCmd;
  delete rotXCmd;
  delete rotYCmd;
  delete rotZCmd;
  delete isocenterPosCmd;
  delete collimatorRotAxisCmd;
  delete collimatorAngleCmd;
  delete gantryRotAxisCmd;
  delete gantryAngleCmd;
  delete axialSymmetryXCmd;
  delete axialSymmetryYCmd;
  delete axialSymmetryZCmd;
}


void G4IAEAphspReaderMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if( command == nOfParallelRunsCmd )
    { theIAEAphspReader->SetTotalParallelRuns( nOfParallelRunsCmd->GetNewIntValue(newValue) ); }

  else if( command == parallelRunCmd )
    { theIAEAphspReader->SetParallelRun( parallelRunCmd->GetNewIntValue(newValue) ); }

  else if( command == timesRecycledCmd )
    { theIAEAphspReader->SetTimesRecycled( timesRecycledCmd->GetNewIntValue(newValue) ); }

  else if( command == phspGlobalTranslationCmd )
    { theIAEAphspReader->SetGlobalPhspTranslation( phspGlobalTranslationCmd->GetNew3VectorValue(newValue) ); }

  else if( command == phspRotationOrderCmd )
    { theIAEAphspReader->SetTimesRecycled( phspRotationOrderCmd->GetNewIntValue(newValue) ); }

  else if( command == rotXCmd )
    { theIAEAphspReader->SetRotationX( rotXCmd->GetNewDoubleValue(newValue) ); }

  else if( command == rotYCmd )
    { theIAEAphspReader->SetRotationY( rotYCmd->GetNewDoubleValue(newValue) ); }

  else if( command == rotZCmd )
    { theIAEAphspReader->SetRotationZ( rotZCmd->GetNewDoubleValue(newValue) ); }

  else if( command == isocenterPosCmd )
    { theIAEAphspReader->SetIsocenterPosition( isocenterPosCmd->GetNew3VectorValue(newValue) ); }

  else if( command == collimatorRotAxisCmd )
    { theIAEAphspReader->SetCollimatorRotationAxis( collimatorRotAxisCmd->GetNew3VectorValue(newValue) ); }

  else if( command == collimatorAngleCmd )
    { theIAEAphspReader->SetCollimatorAngle( collimatorAngleCmd->GetNewDoubleValue(newValue) ); }

  else if( command == gantryRotAxisCmd )
    { theIAEAphspReader->SetGantryRotationAxis( gantryRotAxisCmd->GetNew3VectorValue(newValue) ); }

  else if( command == gantryAngleCmd )
    { theIAEAphspReader->SetGantryAngle( gantryAngleCmd->GetNewDoubleValue(newValue) ); }

  else if( command == axialSymmetryXCmd )
    { theIAEAphspReader->SetAxialSymmetryX( axialSymmetryXCmd->GetNewBoolValue(newValue) );}

  else if( command == axialSymmetryYCmd )
    { theIAEAphspReader->SetAxialSymmetryY( axialSymmetryYCmd->GetNewBoolValue(newValue) );}

  else if( command == axialSymmetryZCmd )
    { theIAEAphspReader->SetAxialSymmetryZ( axialSymmetryZCmd->GetNewBoolValue(newValue) );}
}
