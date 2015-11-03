//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: physics.cc,v 1.27 2009/11/15 14:27:30 maire Exp $
// GEANT4 tag $Name: geant4-09-03-patch-02 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "globals.hh"
#include "physics.hh"
#include "BREMSPEProcess.hh"
#include "BREMSPEMessenger.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eIonisation.hh"
#include "G4eplusAnnihilation.hh"
#include "G4GammaConversion.hh"
#include "G4hIonisation.hh"
#include "G4IonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuIonisation.hh"
#include "G4MuPairProduction.hh"
#include "G4ParticleTypes.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4ProcessManager.hh"
#include "G4ComptonScattering.hh"
#include "G4EmProcessOptions.hh"
#include "G4eMultipleScattering.hh"
#include "G4UrbanMscModel93.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4LossTableManager.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

physics::physics():  G4VUserPhysicsList() ,BREMSPE(0)

{


     
  messenger = new BREMSPEMessenger(this); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

physics::~physics()
{
 delete messenger;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void physics::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  ConstructBosons();
  ConstructLeptons();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void physics::ConstructBosons()
{
 
  // gamma
  G4Gamma::GammaDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void physics::ConstructLeptons()
{
  // leptons
  //  e+/-
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void physics::ConstructProcess()
{
  AddTransportation();
  ConstructEM();

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void physics::ConstructEM()
{
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    // Tip:
    // G4ProcessManager::AddProcess(G4VProcess*, 
    //                              G4int ordAtRestDoIt, 
    //                              G4int ordAlongStepDoIt, 
    //                              G4int ordPostStepDoIt);
    // Integers in AddProcess() indicate the order in which the process 
    // is applied. If value is negative, the process is not added to 
    // the corresponding process vector.
    
   if (particleName == "gamma") {

      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
      G4ComptonScattering* ComptonProcess= new G4ComptonScattering();
      pmanager->AddDiscreteProcess(ComptonProcess);
      pmanager->AddDiscreteProcess(new G4GammaConversion);

    } else if (particleName == "e-") {

      G4eMultipleScattering* msc = new G4eMultipleScattering();
      msc->AddEmModel(0, new G4UrbanMscModel93());
      //      msc->AddEmModel(0, new G4GoudsmitSaundersonMscModel());
      pmanager->AddProcess(msc,                       -1, 1, 1);
      pmanager->AddProcess(new G4eIonisation,         -1, 2, 2);
     G4eBremsstrahlung* bremProcess = new G4eBremsstrahlung();
      BREMSPE = new BREMSPEProcess();
      BREMSPE->RegisterProcess(bremProcess);
      pmanager->AddProcess(BREMSPE,-1,-1, 3);

    }else if (particleName == "e+") {

      G4eMultipleScattering* msc = new G4eMultipleScattering();
      //msc->AddEmModel(0, new G4UrbanMscModel93());
   // msc->AddEmModel(0, new G4GoudsmitSaundersonMscModel());
      pmanager->AddProcess(msc,                       -1, 1, 1);
      pmanager->AddProcess(new G4eIonisation,         -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung,     -1,-3, 3);
      pmanager->AddProcess(new G4eplusAnnihilation,    0,-1, 4);

    }
  }
  G4EmProcessOptions opt;

  
  // Physics tables
  //

  //opt.SetDEDXBinning(77);
  //opt.SetLambdaBinning(77);
  //opt.SetSplineFlag(true);
  opt.SetPolarAngleLimit(0.2);
      G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
  G4LossTableManager::Instance()->SetAtomDeexcitation(de);
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void physics::SetCuts()
{
  SetCutValue(0.1*mm, "e-");
SetCutValue(0.1*mm, "e+");
SetCutValue(1*mm, "gamma");
  if (verboseLevel>0) DumpCutValuesTable();
}
void
physics::SetNSplit(G4int nSplit) 
{
  if (0 == BREMSPE) {
    std::ostringstream o;
    o << "Null BREMSPEProcess - did you construct it yet ?";
    G4Exception
      ("physics::SetNSplit(G4int nSplit)",
       "NullProcess", FatalException, o.str().c_str());
  }
  
  BREMSPE->SetNSplit(nSplit);
}

void
physics::SetSplittingActive(bool isActive) 
{
  if (0 == BREMSPE) {
    std::ostringstream o;
    o << "Null BREMSPEProcess - did you construct it yet ?";
    G4Exception
      ("physics::SetSplittingActive(bool isActive)",
       "NullProcess", FatalException, o.str().c_str());
  }
  
  BREMSPE->SetIsActive(isActive);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

