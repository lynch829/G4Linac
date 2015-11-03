#include "PrimaryGeneratorAction.hh"
#include "G4Electron.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include <sstream>
#include <fstream>
#include <limits>
#include <iostream>
#include <fstream>
#include <stdio.h> 
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4MoroGeneratorMessenger.hh"
#include "G4IAEAphspWriter.hh"




PrimaryGeneratorAction::PrimaryGeneratorAction()

{

messenger = new G4MoroGeneratorMessenger(this); 




G4int n_particle = 1;
particleGun = new G4ParticleGun(n_particle);
G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
G4String particleName;
particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="e-"));
particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));

 Max_points=10;
P = sobol_points(Max_points); 
gen = new G4GeneralParticleSource;

}


// LOI NORMALE
G4double PrimaryGeneratorAction::Qnormale(G4double m,G4double s ,G4double x,G4double yy)
       {
          G4double y;
         const G4double eps = 1.0e-6;
  const G4double twopi = 6.2831854;
   G4double r1 = x;
  r1 = r1 > eps ? r1 : eps;
  G4double r2 = yy;
  r2 = r2 > eps ? r2 : eps;
  r2 = r2 < 1.0 - eps ? r2 : 1.0 - eps; 

 y= std::sin(twopi * r1) * std::sqrt(-2.0 * std::log(r2)); 

          return m + (s*y);
       }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


PrimaryGeneratorAction::~PrimaryGeneratorAction()
{

delete gen;
 delete messenger;
  delete particleGun;


}

double ** PrimaryGeneratorAction::sobol_points(unsigned N)
{

//4-dimension
unsigned D=4;

  // L = max number of bits needed 
  unsigned L = (unsigned)ceil(log((double)N)/log(2.0)); 

  // C[i] = index from the right of the first zero bit of i
  unsigned *C = new unsigned [N];
  C[0] = 1;
  for (unsigned i=1;i<=N-1;i++) {
    C[i] = 1;
    unsigned value = i;
    while (value & 1) {
      value >>= 1;
      C[i]++;
    }
  }
  
  // POINTS[i][j] = the jth component of the ith point
  //                with i indexed from 0 to N-1 and j indexed from 0 to D-1
  double **POINTS = new double * [N];
  for (unsigned i=0;i<=N-1;i++) POINTS[i] = new double [D];
  for (unsigned j=0;j<=D-1;j++) POINTS[0][j] = 0; 

  // ----- Compute the first dimension -----
  
  // Compute direction numbers V[1] to V[L], scaled by pow(2,32)
  unsigned *V = new unsigned [L+1]; 
  for (unsigned i=1;i<=L;i++) V[i] = 1 << (32-i); // all m's = 1

  // Evalulate X[0] to X[N-1], scaled by pow(2,32)
  unsigned *X = new unsigned [N];
  X[0] = 0;
  for (unsigned i=1;i<=N-1;i++) {
    X[i] = X[i-1] ^ V[C[i-1]];
    POINTS[i][0] = (double)X[i]/pow(2.0,32); // *** the actual points
    //        ^ 0 for first dimension
  }
  
  // Clean up
  delete [] V;
  delete [] X;
  
  
  // ----- Compute the remaining dimensions -----
  for (unsigned j=1;j<=D-1;j++) {
    
    // Read in parameters from file 
    unsigned d=2, s=1;
    unsigned a=0;
    unsigned *m = new unsigned [s+1];
     m[1]=1;
    
    // Compute direction numbers V[1] to V[L], scaled by pow(2,32)
    unsigned *V = new unsigned [L+1];
    if (L <= s) {
      for (unsigned i=1;i<=L;i++) V[i] = m[i] << (32-i); 
    }
    else {
      for (unsigned i=1;i<=s;i++) V[i] = m[i] << (32-i); 
      for (unsigned i=s+1;i<=L;i++) {
	V[i] = V[i-s] ^ (V[i-s] >> s); 
	for (unsigned k=1;k<=s-1;k++) 
	  V[i] ^= (((a >> (s-1-k)) & 1) * V[i-k]); 
      }
    }
    
    // Evalulate X[0] to X[N-1], scaled by pow(2,32)
    unsigned *X = new unsigned [N];
    X[0] = 0;
    for (unsigned i=1;i<=N-1;i++) {
      X[i] = X[i-1] ^ V[C[i-1]];
      POINTS[i][j] = (double)X[i]/pow(2.0,32); // *** the actual points
      //        ^ j for dimension (j+1)
   }
    
    // Clean up
    delete [] m;
    delete [] V;
    delete [] X;
  }
  delete [] C;
  
  return POINTS;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
struct rec r1,r2;
int id;
id=anEvent->GetEventID();

if (this->generator=="QMC"){
r1.x=P[id][0];
r1.y=P[id][1];
r2.x=P[id][2];
r2.y=P[id][3];
 x=Qnormale(0, this->SpotSize,  r1.x, r1.y);
 y=Qnormale(0, this->SpotSize,  r1.x,  r2.x);
 energy=Qnormale(this->energy, this->sigma, r1.x, r2.y);
particleGun->SetParticlePosition(G4ThreeVector(x,y, this->Source_Zpos));
particleGun->SetParticleEnergy(energy);
particleGun->GeneratePrimaryVertex(anEvent);
}/*
 x= Qnormale(0, this->SpotSize, G4UniformRand(), G4UniformRand());
 y=Qnormale(0, this->SpotSize,  G4UniformRand(),  G4UniformRand());
energy=Qnormale(this->energy, this->sigma,  G4UniformRand(),G4UniformRand());
particleGun->SetParticlePosition(G4ThreeVector(x,y, this->Source_Zpos));
particleGun->SetParticleEnergy(energy);
particleGun->GeneratePrimaryVertex(anEvent);*/
if (this->generator=="MC")
{

gen->GeneratePrimaryVertex(anEvent);}
}






void PrimaryGeneratorAction::SetSourceZpos(G4double Source_Zpos)
{ 
this->Source_Zpos=Source_Zpos;
}

void PrimaryGeneratorAction::SetSpotSize(G4double spotsize)
{ 
this->SpotSize=spotsize;
}
void PrimaryGeneratorAction::SetEnergy(G4double energy)
{ 
this->energy=energy;
}
void PrimaryGeneratorAction::SetSigma(G4double sigma)
{ this->sigma=sigma;}
void PrimaryGeneratorAction::SetGeneratorType(const G4String& generator) 
{this->generator=generator; }
