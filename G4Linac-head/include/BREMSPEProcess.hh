#ifndef BREMSPEPROCESS_HH
#define BREMSPEPROCESS_HH 1
#include "G4WrapperProcess.hh"

class BREMSPEProcess : public G4WrapperProcess {
  
  public:
  BREMSPEProcess();	
  virtual ~BREMSPEProcess();	
  G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step);
  static void SetNSplit(G4int);
  static void SetIsActive(G4bool);
  static G4bool GetIsActive();
  static G4int GetNSplit();
  static G4int GetNSecondaries();

  private:
  static G4int fNSplit;
  static G4int fNSecondaries;
  static G4bool fActive;

};

#endif
