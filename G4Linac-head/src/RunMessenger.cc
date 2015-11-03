
#include "RunMessenger.hh"
#include "RunAction.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"

RunMessenger::RunMessenger(RunAction *runAction)
:runAction(runAction) 
{  
 G4MoroDir = new G4UIdirectory("/Phsp/");
 G4MoroDir->SetGuidance("Commands related to Phsp ");
 

 PhspNameCmd = new G4UIcmdWithAString("/Phsp/Name",this);
 PhspNameCmd->SetGuidance("name of Phsp file");
 PhspNameCmd->SetParameterName("PhspName",false);
 PhspNameCmd->SetDefaultValue("Phsp");

 PhspZstopmd = new G4UIcmdWithADoubleAndUnit("/Phsp/Zstop",this);
 PhspZstopmd->SetGuidance("Zstop of Phsp file");
 PhspZstopmd->SetParameterName("PhspZstop",false);
 PhspZstopmd->SetUnitCategory("Length");
 PhspZstopmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

RunMessenger::~RunMessenger()
{
 
  delete PhspZstopmd;
  delete PhspNameCmd;
}

void RunMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
 
 if (command ==  PhspZstopmd) {
runAction->SetPhspZstop(PhspZstopmd->GetNewDoubleValue(newValue));
 }
else if (command ==  PhspNameCmd) {
runAction->SetPhspName(newValue);
 }
 else {
   G4cerr << "***** Command is not found !!! " << newValue << G4endl;
 }

}

