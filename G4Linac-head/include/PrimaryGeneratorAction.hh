
#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1
#include <sstream>
#include <fstream>
#include <limits>
#include <iostream>
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Electron.hh"



class G4MoroGeneratorMessenger;
class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;

 
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
   ~PrimaryGeneratorAction();
 FILE *f; 
void SetSpotSize(G4double );
void SetEnergy(G4double );
void SetSigma(G4double );
void SetSourceZpos(G4double );
void SetPhspZstop(G4double );
void SetGeneratorType(const G4String& generator );   
void SetPhspName(const G4String& generator );   
  public:
struct rec  

{        
    
    double x,y;  

}; 
void GeneratePrimaries(G4Event*);

double ** sobol_points(unsigned N);
G4double  Qnormale(G4double ,G4double,G4double ,G4double );
private:
G4ParticleGun* particleGun;
   G4GeneralParticleSource* gen;
 int Max_points;
rec r;
double **P;
double SpotSize;
double energy,x,y,Source_Zpos;
double sigma;
G4String  generator;
protected:
  G4MoroGeneratorMessenger* messenger;
};



#endif

