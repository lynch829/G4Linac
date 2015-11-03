#ifndef volumeConstruction_h
#define volumeConstruction_h  1
#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Box;
class G4Colour;
class PhantomSD;
class PhantomROGeometry;
class G4Region;
class G4Material;
class G4VisAttributes;
class  volumeConstruction:  public G4VUserDetectorConstruction
   {      public:
      volumeConstruction(G4String&);
     ~volumeConstruction();

      public:

         G4VPhysicalVolume* Construct();
         G4VPhysicalVolume*  pMere;
         G4Material * MaterialName(const G4String materialName);
	 G4VisAttributes *AttributName(const G4String attributName);

	private:
  PhantomSD* phantomSD;//pointer to sensitive detector
  PhantomROGeometry* phantomROGeometry;//pointer to ROGeometry
  G4Box*              Phantom;  //pointer to solid phantom
  G4LogicalVolume*    PhantomLog; //pointer to logic phantom
  G4VPhysicalVolume*  PhantomPhys; //pointer to physical phantom
  G4double phantomSizeX; //Phantom XSize
  G4double phantomSizeY; //Phantom YSize
  G4double phantomSizeZ; //Phantom ZSize  
  G4int numberOfVoxelsAlongX; //Number of voxels along x axis
  G4int numberOfVoxelsAlongY; //Number of voxels along y axis 
  G4int numberOfVoxelsAlongZ; //Number of voxels along z axis 
  G4String sensitiveDetectorName; 

  G4LogicalVolume* vacum;
  G4LogicalVolume*  LoGicWater;
  G4LogicalVolume* Phantom_log;
 // Physical volumes
  G4VPhysicalVolume* pvacum;
  G4VPhysicalVolume* pPhantom_log;
  G4VPhysicalVolume*  pLoGicWater;
  };
#endif
