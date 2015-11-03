
#ifndef volumeConstruction_h
#define volumeConstruction_h  1
#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Box;
class G4Colour;
class G4Region;
class G4Material;
class G4VisAttributes;

class  volumeConstruction:  public G4VUserDetectorConstruction
   {      public:
      volumeConstruction();
     ~volumeConstruction();

      public:

      G4VPhysicalVolume* Construct();

         G4VPhysicalVolume*  pMere;

        G4Material * MaterialName(const G4String materialName);
	G4VisAttributes *AttributName(const G4String attributName);
void WinTitaniumConstructor();
	void targetConstructor();
	void primaryCollimatorConstructor();
	void secondaryCollimatorConstructor();
	void flatteningFilteConstructor();
	void ionizationChamberConstructor();
	void AlPlaqueConstructor();
        void jawsConstructor();

        void KillerPlanConstructor();
G4double dec;
	private:
        
       G4String Flag;
  G4String sensitiveDetectorName; 
G4LogicalVolume* KillerPlanLog;

//Solid

// Logical volumes
  G4VPhysicalVolume* KillerPlanPhys;
  G4LogicalVolume* vacum;
  G4LogicalVolume* WinTitanium;
  G4LogicalVolume* Target;
  G4LogicalVolume* CollimatorA1;
  G4LogicalVolume* CollimatorA2;
  G4LogicalVolume* ColliSec;
  G4LogicalVolume* Cone;
  G4LogicalVolume* LoGiCPlqAl;
  G4LogicalVolume* LoGiCJawX1_1;
  G4LogicalVolume* LoGiCJawX1_2;
  G4LogicalVolume* LoGiCJawX1_3;
  G4LogicalVolume* LoGiCJawX1_4;
  G4LogicalVolume* LoGiCJawX1_5;
  G4LogicalVolume* LoGiCJawX1_6;
  G4LogicalVolume* LoGiCJawY1_1;
  G4LogicalVolume* LoGiCJawY1_2;
  G4LogicalVolume* LoGiCJawY1_3;
  G4LogicalVolume* LoGiCJawY1_4;
  G4LogicalVolume* LoGiCJawY1_5;
  G4LogicalVolume* LoGicChI_ElementTypeA1;
  G4LogicalVolume* LoGicChI_ElementTypeA2;
  G4LogicalVolume* LoGicChI_ElementTypeB1;
  G4LogicalVolume* LoGicChI_ElementTypeB2;
  
 // Physical volumes
  //
  G4VPhysicalVolume* pvacum;
  G4VPhysicalVolume* pWinTitanium;
  G4VPhysicalVolume* pTarget;
  G4VPhysicalVolume* pCollimatorA1;
  G4VPhysicalVolume* pCollimatorA2;
  G4VPhysicalVolume* pColliSec;
  G4VPhysicalVolume* pPhantom_log;
  G4VPhysicalVolume* pCone;
  G4VPhysicalVolume* pLoGiCPlqAl;
  G4VPhysicalVolume* pLoGicChI_ElementTypeA1;
  G4VPhysicalVolume* pLoGicChI_ElementTypeA2;
  G4VPhysicalVolume* pLoGicChI_ElementTypeB1;
  G4VPhysicalVolume* pLoGicChI_ElementTypeB2;
  G4VPhysicalVolume* pLoGiCJawX1_1;
  G4VPhysicalVolume* pLoGiCJawX1_2;
  G4VPhysicalVolume* pLoGiCJawX1_3;
  G4VPhysicalVolume* pLoGiCJawX1_4;
  G4VPhysicalVolume* pLoGiCJawX1_5;
  G4VPhysicalVolume* pLoGiCJawX1_6;
  G4VPhysicalVolume* pLoGiCJawY1_1;
  G4VPhysicalVolume* pLoGiCJawY1_2;
  G4VPhysicalVolume* pLoGiCJawY1_3;
  G4VPhysicalVolume* pLoGiCJawY1_4;
  G4VPhysicalVolume* pLoGiCJawY1_5;
  G4VPhysicalVolume* pLoGiCJawX2_1;
  G4VPhysicalVolume* pLoGiCJawX2_2;
  G4VPhysicalVolume* pLoGiCJawX2_3;
  G4VPhysicalVolume* pLoGiCJawX2_4;
  G4VPhysicalVolume* pLoGiCJawX2_5;
  G4VPhysicalVolume* pLoGiCJawX2_6;
  G4VPhysicalVolume* pLoGiCJawY2_1;
  G4VPhysicalVolume* pLoGiCJawY2_2;
  G4VPhysicalVolume* pLoGiCJawY2_3;
  G4VPhysicalVolume* pLoGiCJawY2_4;
  G4VPhysicalVolume* pLoGiCJawY2_5;
  G4VPhysicalVolume* pChI_ElementTypeB1_layer1;
  G4VPhysicalVolume* pChI_ElementTypeB2_layer1;
  G4VPhysicalVolume* pChI_ElementTypeA2_layer2;
  G4VPhysicalVolume* pChI_ElementTypeA1_layer2;
  G4VPhysicalVolume* pChI_ElementTypeB1_layer3;
  G4VPhysicalVolume* pChI_ElementTypeB2_layer3;
  G4VPhysicalVolume* pChI_ElementTypeB1_layer4;
  G4VPhysicalVolume* pChI_ElementTypeB2_layer4;
  G4VPhysicalVolume* pChI_ElementTypeA1_layer5;
  G4VPhysicalVolume* pChI_ElementTypeA2_layer5;
  G4VPhysicalVolume* pChI_ElementTypeB2_layer6; 
  G4VPhysicalVolume* pChI_ElementTypeB1_layer6;
  };
#endif
