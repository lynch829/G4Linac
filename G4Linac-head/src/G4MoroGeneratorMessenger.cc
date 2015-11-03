#include "G4MoroGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
G4MoroGeneratorMessenger::G4MoroGeneratorMessenger(PrimaryGeneratorAction *G4Moro)
  :G4Moro(G4Moro) 
{  
 G4MoroDir = new G4UIdirectory("/G4MoroGenerator/");
 G4MoroDir->SetGuidance("Commands to choice primary generator");
 EnergyCmd = new G4UIcmdWithADoubleAndUnit("/G4MoroGenerator/Energy",this);  
 EnergyCmd->SetGuidance("beam energy ");
 EnergyCmd->SetParameterName("Energy",false);
 EnergyCmd->SetDefaultUnit("MeV");
 EnergyCmd->SetUnitCategory("Energy");
 EnergyCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 SourceZposCmd = new G4UIcmdWithADoubleAndUnit("/G4MoroGenerator/SourceZpos",this);  
 SourceZposCmd->SetGuidance("source Z position ");
 SourceZposCmd->SetParameterName("SourceZpos",false);
 SourceZposCmd->SetDefaultUnit("cm");
 SourceZposCmd->SetUnitCategory("Length");
 SourceZposCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


 SigmaCmd = new G4UIcmdWithADoubleAndUnit("/G4MoroGenerator/Sigma",this);  
 SigmaCmd->SetGuidance("beam sigma ");
 SigmaCmd->SetUnitCategory("Energy");
 SigmaCmd->SetParameterName("Sigma",false);
 SigmaCmd->SetDefaultUnit("MeV");
 SigmaCmd->SetUnitCategory("Energy");
 SigmaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 SpotSIzeCmd = new G4UIcmdWithADoubleAndUnit("/G4MoroGenerator/SpotSize",this);  
 SpotSIzeCmd->SetGuidance("beam Spot size ");
 SpotSIzeCmd->SetParameterName("SpotSize",false);
 SpotSIzeCmd->SetUnitCategory("Length");
 SpotSIzeCmd->SetDefaultUnit("mm");
 SpotSIzeCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

 GeneratorCmd = new G4UIcmdWithAString("/G4MoroGenerator/generator",this);
 GeneratorCmd->SetGuidance("MC or QMC");
 GeneratorCmd->SetParameterName("generator",false);
 GeneratorCmd->SetDefaultValue("MC");

 
 
}

G4MoroGeneratorMessenger::~G4MoroGeneratorMessenger()
{
  delete EnergyCmd;
  delete SpotSIzeCmd;
  delete SigmaCmd;
  delete G4MoroDir;
  delete GeneratorCmd;
  delete SourceZposCmd;
}

void G4MoroGeneratorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
 if (command == SpotSIzeCmd ) { 
  G4Moro->SetSpotSize(SpotSIzeCmd->GetNewDoubleValue(newValue));
 } else if (command ==  SigmaCmd) {
  G4Moro->SetSigma(SigmaCmd->GetNewDoubleValue(newValue));
 }else if (command ==  EnergyCmd) {
  G4Moro->SetEnergy(EnergyCmd->GetNewDoubleValue(newValue));
 }else if (command ==  GeneratorCmd) {
  G4Moro->SetGeneratorType(newValue);
 }else if (command ==  SourceZposCmd) {
  G4Moro->SetSourceZpos(SourceZposCmd->GetNewDoubleValue(newValue));
 }

 else {
   G4cerr << "***** Command is not found !!! " << newValue << G4endl;
 }

}

