
#ifndef physics_h
#define physics_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class BREMSPEProcess;
class BREMSPEMessenger;
class volumeConstruction;
class physics: public G4VUserPhysicsList
{
  public:
    physics();
   ~physics();
void SetNSplit(G4int);
  void SetSplittingActive(bool);
  void SetBREMSSEIsActive(bool);
  protected:
    // Construct particle and physics
    void ConstructParticle();
    void ConstructProcess();
     
    void SetCuts();

  // Methods to onfigure bremsstrahlung splitting
  
   
  protected:
    // these methods Construct particles 
    void ConstructBosons();
    void ConstructLeptons();
   

  protected:
  // these methods Construct physics processes and register them
    void ConstructGeneral();
    void ConstructEM();
    void AddStepMax();


  G4int fNSplit;
  bool fSplittingActive;
  BREMSPEProcess* BREMSPE;
  BREMSPEMessenger* messenger;

volumeConstruction*detector;
  private:

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

 
