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

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4VSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"
#include "G4Circle.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include <stdio.h>
#include "PhantomSD.hh"
#include "volumeConstruction.hh"
#include "G4RunManager.hh"
#include <vector>
#include "G4MPImanager.hh"
using namespace std;

PhantomSD::PhantomSD(G4String name, G4ThreeVector halfSize, G4int NumberOfVoxelsAlongX, G4int NumberOfVoxelsAlongY, G4int NumberOfVoxelsAlongZ)
: G4VSensitiveDetector(name),voxels(0)
{   
  this->nTotalEvents=0;
  this->halfSize=halfSize;
  this->voxelVolume=0.;
  this->voxelMass=0.;
  this->NumberOfVoxelsAlongX=NumberOfVoxelsAlongX;
  this->NumberOfVoxelsAlongY=NumberOfVoxelsAlongY;
  this->NumberOfVoxelsAlongZ=NumberOfVoxelsAlongZ;
  this->halfXVoxelDimensionX=this->halfSize.getX()/this->NumberOfVoxelsAlongX;
  this->halfXVoxelDimensionY=this->halfSize.getY()/this->NumberOfVoxelsAlongY;
  this->halfXVoxelDimensionZ=this->halfSize.getZ()/this->NumberOfVoxelsAlongZ;
  this->CentreVoxelID_x=22;// 45/2+1
  this->CentreVoxelID_y=22;// 45/2+1
  this->VoxelID_At_10_z=19;/// 
//definir le volume du voxel 
  this->voxelVolume=8*this->halfXVoxelDimensionX*this->halfXVoxelDimensionY*this->halfXVoxelDimensionZ;
//construction et initialisation des tableaux des  voxels   
this->voxels=new Svoxel**[this->NumberOfVoxelsAlongX];
		for (int ix=0; ix< this->NumberOfVoxelsAlongX; ix++)
		{
			this->voxels[ix]=new Svoxel*[this->NumberOfVoxelsAlongY];
			for (int iy=0; iy< this->NumberOfVoxelsAlongY; iy++)
			{
				this->voxels[ix][iy]=new Svoxel[this->NumberOfVoxelsAlongZ];
				for (int iz=0; iz< this->NumberOfVoxelsAlongZ; iz++)
				{
					this->voxels[ix][iy][iz].volumeName="noData";
					this->voxels[ix][iy][iz].depEnergy=0.;
					this->voxels[ix][iy][iz].depEnergy2=0.;


					this->voxels[ix][iy][iz].halfSize.set(this->halfXVoxelDimensionX, this->halfXVoxelDimensionY, this->halfXVoxelDimensionZ);
					this->voxels[ix][iy][iz].pos.set(2.*(ix)*this->halfXVoxelDimensionX  -this->halfSize.getX()+this->halfXVoxelDimensionX, 
			2.*(iy)*this->halfXVoxelDimensionY  -this->halfSize.getY()+this->halfXVoxelDimensionY, 
			2.*(iz)*this->halfXVoxelDimensionZ  -this->halfSize.getZ()+this->halfXVoxelDimensionZ);
					this->voxels[ix][iy][iz].nEvents=0;
					this->voxels[ix][iy][iz].stddev=0.;
				}
			}
		}

	
}

PhantomSD::~PhantomSD()
{
delete this->voxels;
}

void PhantomSD::Initialize(G4HCofThisEvent*HCE)
{
//G4RunManager* runManager = G4RunManager::GetRunManager();
//const G4Event * event = runManager->GetCurrentEvent();
///G4int event_id= event->GetEventID();
 //if ( event_id% 3!=0 or event_id% 10!=0 ){runManager->AbortEvent();}


}


G4bool PhantomSD::ProcessHits(G4Step* aStep,
G4TouchableHistory*ROhist )
{
 if (!ROhist) return false ;
 // Check the volume
  if(aStep -> GetPreStepPoint() -> GetPhysicalVolume() -> 
  GetName() != "pLoGicWater" )
  return false;
  G4String particleName = aStep -> GetTrack() -> GetDynamicParticle() -> 
                           GetDefinition() -> GetParticleName();
  G4double edep = aStep -> GetTotalEnergyDeposit();
  edep*=MeV;
// Check that the energy deposit is not null
  if(edep == 0.)
  return false;
  if(edep != 0)                       
	    {               
  //---- definition de nombre de voxels le long de trois axes x,y,z------------------
  // chaque voxel à un épaisseur de 0.5 cm 
  G4int j = ROhist -> GetReplicaNumber(0);
  G4int k = ROhist -> GetReplicaNumber(1);
  G4int i = ROhist -> GetReplicaNumber(2);
  // Retrieve the coordinates of the center of the voxel where
  // the energy deposit is located
  x = ( - this->NumberOfVoxelsAlongX + 1+ 2*i )*this->halfXVoxelDimensionX;
  y = ( - this->NumberOfVoxelsAlongY+ 1+ 2*j )* this->halfXVoxelDimensionY;
  z = ( - this->NumberOfVoxelsAlongZ+ 1+ 2*k)*this->halfXVoxelDimensionZ;
   //extraire la densite de l'eau
   this->density=aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetDensity();
   //extraire la masse de voxel
   this->voxelMass=this->voxelVolume*this->density;
    //definition du parametre  dose = edep/masse
    edep/=this->voxelMass;
    //extraire les valeurs de dose dans chaque cellules
    this->voxels[i][j][k].depEnergy+=edep;
    this->voxels[i][j][k].depEnergy2+=edep*edep;
    this->voxels[i][j][k].nEvents++;
    G4double events =voxels[i][j][k].nEvents;

    if(events>1){
    G4double dp =voxels[i][j][k].depEnergy;
    G4double dp2=voxels[i][j][k].depEnergy2;
    G4double dpm=dp/events;
    G4double dp2m=dp2/events;
    G4double sigma=sqrt((dp2m-dpm*dpm)/(events-1));
    this->voxels[i][j][k].stddev=100*sigma/dpm;
     }
    this->nTotalEvents++;
     }
  return true;
}

void PhantomSD::EndOfEvent(G4HCofThisEvent*)
{
G4RunManager* runManager = G4RunManager::GetRunManager();
G4int event_numbers= runManager->GetCurrentRun()->GetNumberOfEventToBeProcessed() ;
const G4Event * event = runManager->GetCurrentEvent();
G4int event_id= event->GetEventID();
static int n=1;




if((event_id==10000*n) or (event_id==event_numbers-1) )
{
n++;
this->saveDataInVoxels_pdd();
this->saveDataInVoxels_profile();

} 
}

void PhantomSD::clear()
{} 

void PhantomSD::DrawAll()
{}

void PhantomSD::PrintAll()
{}


void PhantomSD::saveDataInVoxels_pdd()
{
this->saveHeaderDataInVoxels_pdd();
ofstream pddFile;
G4int rank= G4MPImanager::GetManager()-> GetRank();  

G4String fileName="";
if (rank==0) {fileName="pdd0";}
if (rank==1) {fileName="pdd1";}
if (rank==2) {fileName="pdd2";}
if (rank==3) {fileName="pdd3";}
if (rank==4) {fileName="pdd4";}
if (rank==5) {fileName="pdd5";}
if (rank==6) {fileName="pdd6";}


pddFile.open(fileName, std::ios::app);
int iz;
for(iz=0; iz<NumberOfVoxelsAlongZ; iz++){

G4double posWaterZ=0.25;
pddFile << iz<<'\t'; 
pddFile<<setw(10)<<posWaterZ+this->halfSize.getZ()/cm+(this->voxels[this->CentreVoxelID_x][this->CentreVoxelID_y][iz].pos.getZ()/cm 
)<<'\t'; 
pddFile<<setw(10)<< this->voxels[this->CentreVoxelID_x][this->CentreVoxelID_y][iz].depEnergy/(MeV/g) << '\t';
pddFile<<setw(10)<<  this->voxels[this->CentreVoxelID_x][this->CentreVoxelID_y][iz].depEnergy / this->voxels[this->CentreVoxelID_x][this->CentreVoxelID_y][this->VoxelID_At_10_z].depEnergy<<'\t';
pddFile <<setw(10)<< this->voxels[this->CentreVoxelID_x][this->CentreVoxelID_y][iz].stddev<<'\n';
}
pddFile.close();
}

void PhantomSD::saveHeaderDataInVoxels_pdd()
{
//rempli le ichier de sortie par les donnes de chaque voxels 
ofstream pddFile;
G4int rank= G4MPImanager::GetManager()-> GetRank();  
G4String fileName="";

if (rank==0) {fileName="pdd0";}
if (rank==1) {fileName="pdd1";}
if (rank==2) {fileName="pdd2";}
if (rank==3) {fileName="pdd3";}
if (rank==4) {fileName="pdd4";}
if (rank==5) {fileName="pdd5";}
if (rank==6) {fileName="pdd6";}
if (rank==7) {fileName="pdd7";}


pddFile.open(fileName, std::ios::out);
pddFile << "==========================================================================================="<< G4endl;
pddFile << "ERSN/LMR, University Abdelmalek Essaadi Faculty of Sciences - Tetouan, Morocco "<< G4endl;
pddFile<< "Code developed by Jaafar EL bakkali  "<< G4endl;
pddFile << "SIMULATION INFOS : CALCULATE PDD" << G4endl;
pddFile << "TOTAL NUMBERS OF EVENTS IN VOXELISED VOLUME : "<<this->nTotalEvents<<G4endl;
pddFile << "VOXELISED PHANTOM SIZE : " <<halfSize.getX()/cm<<"," <<halfSize.getY()/cm<<"," <<halfSize.getZ()/cm<< G4endl;
pddFile << "NUMBER OF VOXELS (X,Y,Z) : " <<  NumberOfVoxelsAlongX<< "," << NumberOfVoxelsAlongY<<","<< NumberOfVoxelsAlongZ<<G4endl;
pddFile << "VOXEL VOLUME : " << this->voxelVolume/(cm*cm*cm)  << " CM_3 " << G4endl;
pddFile << "VOXEL MASSE : " << this->voxelVolume/(g)  << " GRAMME " << G4endl;
pddFile << "==========================================================================================="<< G4endl;
pddFile << " ID "<< " XC_PHANTOM(CM)"<<"        EDEP(MEV/G)"<<" EDEPN10        ERROR%" << G4endl;
pddFile.close();
}
void PhantomSD::saveDataInVoxels_profile()
{
this->saveHeaderDataInVoxels_profile();
std::ofstream profileFile;
G4int rank= G4MPImanager::GetManager()-> GetRank();  

G4String fileName="";


if (rank==0) {fileName="prf0";}
if (rank==1) {fileName="prf1";}
if (rank==2) {fileName="prf2";}
if (rank==3) {fileName="prf3";}
if (rank==4) {fileName="prf4";}
if (rank==5) {fileName="prf5";}
if (rank==6) {fileName="prf6";}
if (rank==7) {fileName="prf7";}



profileFile.open(fileName, std::ios::app);
int ix,iz;
G4double Posx;
for (iz=0; iz<NumberOfVoxelsAlongZ; iz++){
for (ix=0; ix<NumberOfVoxelsAlongX; ix++){
profileFile <<std::setw(10)<< iz <<'\t'<< std::setw(10)<<ix<<" " <<this->voxels[ix][this->CentreVoxelID_y][iz].pos.getX()/cm<<'\t';  
profileFile<<std::setw(10)<<this->voxels[ix][this->CentreVoxelID_y][iz].depEnergy/this->voxels[this->CentreVoxelID_x][this->CentreVoxelID_y][iz].depEnergy<< '\t';
//profileFile <<std::setw(10)<< this->voxels[ix][this->CentreVoxelID_y][VoxelID_At_10_z].nEvents <<'\t';
profileFile<<std::setw(10) << this->voxels[ix][this->CentreVoxelID_y][iz].stddev<<'\n';
}
}
profileFile.close();

}

void PhantomSD::saveHeaderDataInVoxels_profile()
{
G4int rank= G4MPImanager::GetManager()-> GetRank();  
std::ofstream profileFile;
G4String fileName="";


if (rank==0) {fileName="prf0";}
if (rank==1) {fileName="prf1";}
if (rank==2) {fileName="prf2";}
if (rank==3) {fileName="prf3";}
if (rank==4) {fileName="prf4";}
if (rank==5) {fileName="prf5";}
if (rank==6) {fileName="prf6";}
if (rank==7) {fileName="prf7";}



profileFile.open(fileName, std::ios::out);
profileFile << "==========================================================================================="<< G4endl;
profileFile<< "ERSN/LMR, University Abdelmalek Essaadi Faculty of Sciences - Tetouan, Morocco "<< G4endl;
profileFile<< "Code developed by Jaafar EL bakkali  "<< G4endl;
profileFile << "SIMULATION INFOS : CALCULATE PROFILE " << G4endl;
profileFile << "TOTAL NUMBERS OF EVENTS IN VOXELISED VOLUME :"<<this->nTotalEvents<<G4endl;
profileFile << " VOXELISED PHANTOM SIZE :" <<halfSize.getX()/cm<<"," <<halfSize.getY()/cm<<"," <<halfSize.getZ()/cm<< G4endl;
profileFile << " NUMBER OF VOXELS (X,Y,Z) : " <<  NumberOfVoxelsAlongX<< "," << NumberOfVoxelsAlongY<<","<< NumberOfVoxelsAlongZ<<G4endl;
profileFile << "VOXEL VOLUME : " << this->voxelVolume/(cm*cm*cm)  << " CM_3 " << G4endl;
profileFile << "VOXEL MASSE : " << this->voxelVolume/(g)  << " GRAMME " << G4endl;
profileFile << "==========================================================================================="<< G4endl;
profileFile<< "    IDz            IDx "<< "    XC_PHANTOM(CM)"<<"    EDEP(MEV/G)"<<"      ERROR%" << G4endl;
profileFile.close();

}
