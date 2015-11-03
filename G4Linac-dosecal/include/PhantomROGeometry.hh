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
// $Id: PhantomROGeometry.hh,v 1.7 2006/06/29 15:47:45 gunter Exp $
// GEANT4 tag $Name: geant4-09-03-patch-02 $
//
//    ************************************
//    *                                  *
//    *    PhantomROGeometry.hh   *
//    *                                  *
//    ************************************
//
// The phantom is devided in voxels. 
// The energy deposit is integrated over the voxels
//

#ifndef PhantomROGeometry_h
#define PhantomROGeometry_h 

#include "G4VReadOutGeometry.hh"

class PhantomROGeometry : public G4VReadOutGeometry
{
public:
  PhantomROGeometry(G4String aString,
			  G4double phantomDimX,
			  G4double phantomDimY,
			  G4double phantomDimZ,
			  G4int numberOfVoxelsX,
			  G4int numberOfVoxelsY,
			  G4int numberOfVoxelsZ);

  ~PhantomROGeometry();

private:
  G4VPhysicalVolume* Build();

private:
  const G4double phantomSizeX;
  const G4double phantomSizeY;
  const G4double phantomSizeZ;
  const G4int numberOfVoxelsAlongX;
  const G4int numberOfVoxelsAlongY;
  const G4int numberOfVoxelsAlongZ; 
  G4VPhysicalVolume *ROPhantomYDivisionPhys;
};
#endif
