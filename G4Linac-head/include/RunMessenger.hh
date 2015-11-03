
#ifndef RunMessenger_h
#define RunMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class RunAction;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class  G4UIcmdWithAString;
class RunMessenger: public G4UImessenger {

public:
  
  // Constructor
  RunMessenger(RunAction*);
  
  // Destructor
  virtual ~RunMessenger();
  
  // Modifier
  void SetNewValue(G4UIcommand*, G4String);
    RunAction* runAction;   
private:

  // Data members

  G4UIdirectory* G4MoroDir ;
  G4UIcmdWithAString* PhspNameCmd;
  G4UIcmdWithADoubleAndUnit* PhspZstopmd;
};




#endif





