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
#include "G4ios.hh"
#include <stdio.h>
#include "EventAction.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4RunManager.hh"
#include "G4IAEAphspWriter.hh"
#include "SteppingAction.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
EventAction::EventAction()

{

}
 
EventAction::~EventAction()
{}

 
void EventAction::BeginOfEventAction(const G4Event* aEvent)
{


G4IAEAphspWriter* IAEAWriter = G4IAEAphspWriter::GetInstance();
IAEAWriter->BeginOfEventAction(aEvent);

   


}

 
void EventAction::EndOfEventAction(const G4Event* )
{
static int n=1;
G4IAEAphspWriter* IAEAWriter = G4IAEAphspWriter::GetInstance();
G4RunManager* runManager = G4RunManager::GetRunManager();
    const G4Event * event = runManager->GetCurrentEvent();
    G4int event_id= event->GetEventID();

if ( event_id==60000*n){IAEAWriter->UpdateHeaders();n++;};
}


