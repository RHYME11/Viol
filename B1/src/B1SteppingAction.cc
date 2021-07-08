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
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class

#include "B1SteppingAction.hh"
#include "B1EventAction.hh"
#include "B1DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include <TH1D.h>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::B1SteppingAction(B1EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolumeInitial(0),
  fScoringVolumeStop(0)
{
  ofile.open("junk.dat");
  ofile << "EventID/I:"
//        << setw(10) << "TrackID"
//        << "particle/C:"
        << "stepnum/I:"
        << "volumeName/I:"
        << "edepStep_keV/D:"
        << "edep_keV/D:"
        << "KEparticle_keV/D:"
        << "globalx_um/D:"
        << "globaly_um/D:"
        << "globalz_um/D" << G4endl;
/*        << setw(10) << "global_t_ns/D:"
        << setw(10) << "steplen_mm/D:"
        << setw(10) << "momentum_keV/D:"
 
*/
  TH1D h("h","h",100,0,100);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{
  ofile.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolumeInitial) { 
    const B1DetectorConstruction* detectorConstruction
      = static_cast<const B1DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolumeInitial = detectorConstruction->GetScoringVolumeInitial();   
  }

  if (!fScoringVolumeStop) {                       
    const B1DetectorConstruction* detectorConstruction
      = static_cast<const B1DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolumeStop = detectorConstruction->GetScoringVolumeStop();
  }

  // get volume of the current step
  G4LogicalVolume* volume 
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();
      
  // check if we are in scoring volume
  //if ((volume != fScoringVolumeInitial) && (volume != fScoringVolumeStop)) return;

  // collect energy deposited in this step
  G4double edepStep = step->GetTotalEnergyDeposit();
  fEventAction->AddEdep(edepStep);  

  // TESTING ZONE

  // Get post-step point
  G4StepPoint* prestep = step->GetPreStepPoint();
  
  G4ThreeVector presteppos = prestep->GetPosition(); // position
  G4double kepart = prestep->GetKineticEnergy();         // particle energy
  G4ThreeVector ppart = prestep->GetMomentum();

  // Tracks????? 
  G4Track* track = step->GetTrack();

  //G4int trackid = track->GetTrackID();
  G4int stepnum = track->GetCurrentStepNumber();
  G4String partname = track->GetParticleDefinition()->GetParticleName();
  G4String typelim = "proton";
  G4int score;

  if (volume == fScoringVolumeInitial){
    score = 1;
  }
  else if (volume == fScoringVolumeStop){
    score = 2;
  }
  else {
    score = 0;
  }

  if (partname == typelim){
    G4cout 
          << G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID()
//          << G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->GetEventID()
//          << setw(10) << trackid
//          << "\t" << partname
          << "\t" << stepnum
          << "\t" << score
          << "\t" << edepStep/CLHEP::keV
          << "\t" << fEventAction->GetEdep()/CLHEP::keV
          << "\t" << kepart/CLHEP::keV
          << "\t" << presteppos.x()/CLHEP::um
          << "\t" << presteppos.y()/CLHEP::um
          << "\t" << presteppos.z()/CLHEP::um << G4endl;

  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

