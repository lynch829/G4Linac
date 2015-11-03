


#include "globals.hh"
#include "G4UImessenger.hh"

class StackingAction;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIdirectory;

class StackingMessenger: public G4UImessenger {

public:
  
  // Constructor
  StackingMessenger(StackingAction*);
  
  // Destructor
  virtual ~StackingMessenger();
  
  // Modifier
  void SetNewValue(G4UIcommand*, G4String);
  
private:

  // Data members
  StackingAction* stacka;   
  G4UIdirectory* stackaDir;
  G4UIcmdWithABool* activeCmd;

};










