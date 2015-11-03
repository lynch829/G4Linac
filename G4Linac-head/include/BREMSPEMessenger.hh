#include "globals.hh"
#include "G4UImessenger.hh"

class physiqueList;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIdirectory;

class BREMSPEMessenger: public G4UImessenger {

public:
  BREMSPEMessenger(physiqueList*);
  virtual ~BREMSPEMessenger();
  void SetNewValue(G4UIcommand*, G4String);
  
private:
  physiqueList* list;   
  G4UIdirectory* listDir;
  G4UIcmdWithAnInteger* nSplitCmd;
  G4UIcmdWithABool* activeCmd;

};


