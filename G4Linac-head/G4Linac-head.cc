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

/*this code is writed by Jaafar EL bakkali
contact at e-mail : bahmedj@gmail.com
*/

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "volumeConstruction.hh"
#include "physiqueList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "G4SDManager.hh"
#include "globals.hh"
 #ifdef G4VIS_USE
 #include "G4VisExecutive.hh"
 #endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif
#include "Randomize.hh" 
#include "StackingAction.hh"


#include "randomc.h"

int main(int argc,char** argv)
{
  G4RunManager* runManager = new G4RunManager;
  // set mandatory initialization classes
  volumeConstruction* det = new volumeConstruction();
  runManager->SetUserInitialization(det);
  runManager->SetUserInitialization(new physiqueList());
  // set mandatory user action class
  runManager->SetUserAction(new PrimaryGeneratorAction());
  // set user action classes to visualise trajectories
  runManager->SetUserAction(new RunAction());
  runManager->SetUserAction(new EventAction());
  runManager->SetUserAction(new SteppingAction());
  runManager->SetUserAction(new StackingAction());
  // user application setting
  // Initialize G4 kernel
  runManager->Initialize();
   #ifdef G4VIS_USE
   G4VisManager* visManager = new G4VisExecutive;
   visManager->Initialize();
  #endif    
     
  // Get the pointer to the User Interface manager
  //
  G4UImanager * UImanager = G4UImanager::GetUIpointer();  
  // interactive mode : define UI session
  #ifdef G4UI_USE
    G4UIExecutive * ui = new G4UIExecutive(argc,argv);
  #ifdef G4VIS_USE
  UImanager->ApplyCommand("/control/execute writer.mac");

  #endif
  ui->SessionStart();
  delete ui;
  #endif
     
   #ifdef G4VIS_USE
   delete visManager;
    #endif     
   

  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  delete runManager;
  return 0;
}


