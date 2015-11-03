
#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class RunMessenger;
class RunAction : public G4UserRunAction
{
  public:
    RunAction();
   ~RunAction();

  public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SetPhspName(const G4String& PhspName);
    void SetPhspZstop(G4double PhspZstop);
    double msecs();
private:
double PhspZstop;
G4String PhspName;
 RunMessenger* messenger;
        double m1,m2;

};

#endif





