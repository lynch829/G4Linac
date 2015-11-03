
#include "StackingMessenger.hh"
#include "StackingAction.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"

StackingMessenger::StackingMessenger(StackingAction * stacka)
  :stacka(stacka) 
{  
 stackaDir = new G4UIdirectory("/GAMMATHEC/");

 // GAMMATHEC command


 activeCmd = new G4UIcmdWithABool("/GAMMATHEC/active",this);  
 activeCmd->SetGuidance("Activate or  desactivate GAMMATHEC");
 activeCmd->SetParameterName("active",false);
 activeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

StackingMessenger::~StackingMessenger()
{

  delete activeCmd;
  delete stackaDir;
}

void StackingMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
 if (command == activeCmd) {
   stacka->SetIsActive(activeCmd->GetNewBoolValue(newValue));
 }
 
}

