
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
// $Id: ExPrimaryGeneratorAction.cc,v 1.7 2006/06/29 17:48:13 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-02 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"
#include "G4Electron.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "PhantomSD.hh"
#include "G4IAEAphspReader.hh"
#include "G4MPImanager.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()

{
G4int rank= G4MPImanager::GetManager()-> GetRank();  

G4String fileName="VARIAN-S-BREMSPE";
//if (rank==0){fileName="5.6-0.42-0.0";}
//if (rank==1){fileName="5.6-0.42-0.25";}
//if (rank==2){fileName="5.6-0.42-0.75";}
//if (rank==3){fileName="5.6-0.42-1";}
//if (rank==4){fileName="withoutacessory";}
//if (rank==5){fileName="phsp4";}
//if (rank==6){fileName="5.6-0.5-0.5";}

theIAEAReader = new G4IAEAphspReader(fileName);
   // phase-space plane shift
G4ThreeVector psfShift(0., 0.,-50*cm);//lespace de phase est 
theIAEAReader->SetGlobalPhspTranslation(psfShift);
theIAEAReader->SetTimesRecycled(0);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
if (theIAEAReader) delete theIAEAReader;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
 theIAEAReader->GeneratePrimaryVertex(anEvent);

}

