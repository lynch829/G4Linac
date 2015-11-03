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
//    ************************************
//    *                                  *
//    *   PhantomROGeometry.cc    *
//    *                                  *
//    ************************************
//
//
#include "PhantomROGeometry.hh"
#include "DummySD.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4Box.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"

PhantomROGeometry::PhantomROGeometry(G4String aString,
                                                 G4double phantomDimX,
						 G4double phantomDimY,
                                                 G4double phantomDimZ,
                                                 G4int numberOfVoxelsX,
						 G4int numberOfVoxelsY,
                                                 G4int numberOfVoxelsZ):
  G4VReadOutGeometry(aString),
  phantomSizeX(phantomDimX),
  phantomSizeY(phantomDimY),
  phantomSizeZ(phantomDimZ),
  numberOfVoxelsAlongX(numberOfVoxelsX),
  numberOfVoxelsAlongY(numberOfVoxelsY),
  numberOfVoxelsAlongZ(numberOfVoxelsZ)
{
}

PhantomROGeometry::~PhantomROGeometry()
{
}

G4VPhysicalVolume* PhantomROGeometry::Build()
{
 
  // A dummy material is used to fill the volumes of the readout geometry.
  // (It will be allowed to set a NULL pointer in volumes of such virtual
  // division in future, since this material is irrelevant for tracking.)

  G4Material* dummyMat = new G4Material(name="dummyMat", 
					1., 1.*g/mole, 1.*g/cm3);
G4RotationMatrix rotMatrixph;   // unit rotation matrix
G4double angleph = 0*deg;   // rotational angle
rotMatrixph.rotateY(angleph);  // rot matrix

  G4double worldSizeX = 25*cm;
  G4double worldSizeY = 25*cm;
  G4double worldSizeZ = 100*cm;

  G4double halfPhantomSizeX = phantomSizeX;
  G4double halfPhantomSizeZ = phantomSizeZ;
  G4double halfPhantomSizeY = phantomSizeY;

  // variables for x division ...
  G4double halfVoxelSizex = halfPhantomSizeX/numberOfVoxelsAlongX;
  G4double halfVoxelSizey = halfPhantomSizeY/numberOfVoxelsAlongY;
  G4double halfVoxelSizez = halfPhantomSizeZ/numberOfVoxelsAlongZ;
  G4double voxelThicknessx = 2*halfVoxelSizex;
  G4double voxelThicknessy = 2*halfVoxelSizey;
  G4double voxelThicknessz = 2*halfVoxelSizez;
  // world volume of ROGeometry ...
  G4Box *ROWorld = new G4Box("ROWorld",
			     worldSizeX,
			     worldSizeY,
			     worldSizeZ);

  G4LogicalVolume *ROWorldLog = new G4LogicalVolume(ROWorld,
						    dummyMat,
						    "ROWorldLog",
						    0,0,0);

  G4VPhysicalVolume *ROWorldPhys = new G4PVPlacement(0,
						     G4ThreeVector(),
						     "ROWorldPhys",
						     ROWorldLog,
						     0,false,0);
  
  // Phantom ROGeometry ... 
  G4Box *ROPhantom = new G4Box("ROPhantom", 
			       halfPhantomSizeX, 
			       halfPhantomSizeY, 
			       halfPhantomSizeZ);

  G4LogicalVolume *ROPhantomLog = new G4LogicalVolume(ROPhantom,
						      dummyMat,
						      "ROPhantomLog",
						      0,0,0);

   G4VPhysicalVolume *ROPhantomPhys = new G4PVPlacement(0,
						       G4ThreeVector(0.0*cm,0.0*cm,20*cm),
						       "PhantomPhys",
                                                       ROPhantomLog,
                                                       ROWorldPhys,
                                                       false,0);
  // 11.75+0.75
 
  // X division first... 
  G4Box *ROPhantomXDivision = new G4Box("ROPhantomXDivision",
					halfVoxelSizex,
					halfPhantomSizeY,
					halfPhantomSizeZ);

  G4LogicalVolume *ROPhantomXDivisionLog = new G4LogicalVolume(ROPhantomXDivision,
							       dummyMat,
							       "ROPhantomXDivisionLog",
							       0,0,0);

  G4VPhysicalVolume *ROPhantomXDivisionPhys = new G4PVReplica("ROPhantomXDivisionPhys",
                                                              ROPhantomXDivisionLog,
                                                              ROPhantomPhys,
                                                              kXAxis,
                                                              numberOfVoxelsAlongX,
                                                              voxelThicknessx);
  // ...then Z division
  
  G4Box *ROPhantomZDivision = new G4Box("ROPhantomZDivision",
					halfVoxelSizex,
					halfPhantomSizeY, 
					halfVoxelSizez);

  G4LogicalVolume *ROPhantomZDivisionLog = new G4LogicalVolume(ROPhantomZDivision,
							       dummyMat,
							       "ROPhantomZDivisionLog",
							       0,0,0);

  G4VPhysicalVolume *ROPhantomZDivisionPhys = new G4PVReplica("ROPhantomZDivisionPhys",
							      ROPhantomZDivisionLog,
							      ROPhantomXDivisionPhys,
							      kZAxis,
							      numberOfVoxelsAlongZ,
							      voxelThicknessz);
  // ...then Y  division

  G4Box *ROPhantomYDivision = new G4Box("ROPhantomYDivision",
					halfVoxelSizex, 
					halfVoxelSizey,
					halfVoxelSizez);

  G4LogicalVolume *ROPhantomYDivisionLog = new G4LogicalVolume(ROPhantomYDivision,
							       dummyMat,
							       "ROPhantomYDivisionLog",
							       0,0,0);
 
  ROPhantomYDivisionPhys = new G4PVReplica("ROPhantomYDivisionPhys",
							      ROPhantomYDivisionLog,
							      ROPhantomZDivisionPhys,
							      kYAxis,
							      numberOfVoxelsAlongY,
							      voxelThicknessy);
  DummySD *dummySD = new DummySD;
  ROPhantomYDivisionLog -> SetSensitiveDetector(dummySD);

  return ROWorldPhys;
}



