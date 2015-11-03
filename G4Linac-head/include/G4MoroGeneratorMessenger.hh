#include "globals.hh"
#include "G4UImessenger.hh"
class PrimaryGeneratorAction;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class  G4UIcmdWithAString;
class G4MoroGeneratorMessenger: public G4UImessenger {

  public:
  G4MoroGeneratorMessenger(PrimaryGeneratorAction*);
  virtual ~G4MoroGeneratorMessenger();
  void SetNewValue(G4UIcommand*, G4String);
  private:
  PrimaryGeneratorAction* G4Moro;   
  G4UIdirectory* G4MoroDir ;
  G4UIcmdWithADoubleAndUnit* EnergyCmd;
  G4UIcmdWithADoubleAndUnit* SigmaCmd;
  G4UIcmdWithADoubleAndUnit* SourceZposCmd;
  G4UIcmdWithADoubleAndUnit* SpotSIzeCmd;
  G4UIcmdWithAString* GeneratorCmd;
};








