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
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"
#include "B2bChamberParameterisation.hh"


#include "G4RunManager.hh"

#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"

#include "G4SystemOfUnits.hh"

#include "G4OpBoundaryProcess.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolumeEnv(0),
  fScoringVolume1(0),
  fScoringVolume2(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  G4double atomicNumber = 7.;
  G4double massOfMole = 28.02*g/mole;
  G4double density = 1.e-25*g/cm3;
  G4double temperature = 300*kelvin;
  G4double pressure = 3.e-18*pascal;

  
  // Envelope parameters
  //
  //G4double env_sizeXY = 20*cm, env_sizeZ = 30*cm;
  G4double env_sizeXY = 200*cm, env_sizeZ = 300*cm;
  G4Material* env_mat = new G4Material("interGalactic", atomicNumber,
                                       massOfMole, density, kStateGas,
                                       temperature, pressure);
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double world_sizeXY = 12*env_sizeXY;
  G4double world_sizeZ  = 12*env_sizeZ;
  G4Material* world_mat = new G4Material("interGalactic", atomicNumber,
                                         massOfMole, density, kStateGas,
                                         temperature, pressure);
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
  //     
  // Envelope
  //  
  G4Box* solidEnv =    
    new G4Box("Envelope",                    //its name
        0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ); //its size
      
  G4LogicalVolume* logicEnv =                         
    new G4LogicalVolume(solidEnv,            //its solid
                        env_mat,             //its material
                        "Envelope");         //its name
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
 
  //     
  // Shape 1
  //  
  /*G4double shape1_density = 2.7*g/cm3;
  G4double shape1_a = 26.98*g/mole;
  G4double z;
  G4Material* shape1_mat = new G4Material("Aluminium","Al", z=13, shape1_a, shape1_density);
*/
  //G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_Al");

  G4Material* shape1_mat = new G4Material("interGalactic", atomicNumber,
                                         massOfMole, density, kStateGas,
                                         temperature, pressure);

/***************************    Parameterised Test      ****************************************/
/*
  G4double thickness1 = 10; //unit:um
  G4Tubs* chamberS = 
                    new G4Tubs("tracker",0,3*cm,(thickness1/2)*micrometer, 0*deg, 360.*deg);
  G4LogicalVolume* logicShape1 = 
                    new G4LogicalVolume(chamberS, shape1_mat, "foilAl_log");
  G4int NbOfChambers = 10; 
  G4double chamberSpacing = 0*micrometer; // from chamber center to center
  G4double chamberWidth = 6.0*cm; //width of the chambers
  G4double targetLength = 5.0*cm; //full length of Target
  G4double trackerLength = (NbOfChambers+1)*chamberSpacing; 
  G4double trackerSize = 0.5*trackerLength;  

  //G4double firstPosition = trackerSize + chamberSpacing;
  G4double firstPosition = -4.5*micrometer;
  //G4double firstLength = trackerLength/10;
  G4double firstLength = 1*micrometer;
  //G4double lastLength = trackerLength;
  G4double lastLength = 1*micrometer;
  G4VPVParameterisation* chamberParam = 
                    new ChamberParameterisation(
                                                NbOfChambers,
                                                firstPosition,
                                                chanmberSpacing,
                                                chamberWidth,
                                                firstLength,
                                                lastLength);

  new G4PVParameterised("Chamber",
                        logicShape1,
                        logicEnv,
                        kZAxis,
                        NbOfChambers,
                        chamberParam,
                        checkOverlaps);



*/

/*********************************************************************/
/****************************** Replicas Test ************************/
// cannot track ////

/*
  G4double thickness1 = 10; // thickness of Al foil, unit: um
  G4double bin1 = 1000; //bin number;  
  G4double binlen1 = thickness1/bin1; // length of one bin

  G4double foilAl_x = 6*cm;
  G4double foilAl_y = 6*cm;
  G4double foilAl_z =  (binlen1/2)*micrometer;  // thickness for one bin
  G4Box *foilAl = new G4Box("foilAl", foilAl_x, foilAl_y, foilAl_z);                   
  //G4Box *foilAl = new G4Box("foilAl", 0,0,0);                   
  
  G4LogicalVolume* logicShape1 =                         
    new G4LogicalVolume(foilAl,         //its solid
                        shape1_mat,          //its material
                        "foilAl_log");           //its name

  G4PVReplica ("foilAl_phy",
                  logicShape1,
                  logicEnv,
                  kZAxis,
                  10,
                  1*micrometer);

*/

/************************************************************************/

/************************ Dumb Way *************************************/

  G4double thickness1 = 15; // thickness of Al foil, unit: um
  G4double bin1 = 1; //bin number;  
  G4double binlen1 = thickness1/bin1; // length of one bin

  G4double foilAl_x = 6*cm;
  G4double foilAl_y = 6*cm;
  G4double foilAl_z =  (binlen1/2)*micrometer;  // thickness for one bin
  G4Box *foilAl = new G4Box("foilAl", foilAl_x, foilAl_y, foilAl_z);                   
  
  G4LogicalVolume* logicShape1 =                         
    new G4LogicalVolume(foilAl,         //its solid
                        shape1_mat,          //its material
                        "foilAl_log");           //its name




  for(int i=0;i<bin1;i++){
    G4ThreeVector pos1 = G4ThreeVector(0*cm, 0*cm, -(thickness1/2-binlen1/2-i*binlen1)*micrometer);
    new G4PVPlacement(0,                       //no rotation
                    pos1,                    //at position
                    logicShape1,           //its logical volume
                    "foilAl_phy",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
  }   


/********************************************************************************/



  //     
  // Shape 2
  //

  G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_Ta");
        
  G4double thickness2 = 100; // thickness of Al foil, unit: um
  G4double bin2 = 1; //bin number;  
  G4double binlen2 = thickness2/bin2; // length of one bin

  G4double foilTa_x = 6*cm;
  G4double foilTa_y = 6*cm;
  G4double foilTa_z =  (binlen2/2)*micrometer;  // thickness for one bin
  G4Box *foilTa = new G4Box("foilTa", foilTa_x, foilTa_y, foilTa_z);                   

                
  G4LogicalVolume* logicShape2 =                         
    new G4LogicalVolume(foilTa,         //its solid
                        shape2_mat,          //its material
                        "foilTa_log");           //its name
  for(int j=0;j<bin2;j++){
    G4ThreeVector pos2 = G4ThreeVector(0*cm, 0*cm, (thickness1/2+10000-(thickness2/2-binlen2/2-j*binlen2))*micrometer); // distance between Al and Ta is 10000um = 1cm
    new G4PVPlacement(0,                       //no rotation
                      pos2,                    //at position
                      logicShape2,             //its logical volume
                      "foilTa_phy",                //its name
                      logicEnv,                //its mother  volume
                      false,                   //no boolean operation
                      0,                       //copy number
                      true);
                      //checkOverlaps);          //overlaps checking
   }                

  // Set Shape2 as scoring volume
  //
  fScoringVolume1 = logicShape1;
  fScoringVolume2 = logicShape2;
  fScoringVolumeEnv = logicEnv;

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
