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

#include "G4SystemOfUnits.hh"
#include "G4VTouchable.hh"

#include<TH1D.h>

using std::cout;
using std::setw;
using std::setprecision;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    B1SteppingAction::B1SteppingAction(B1EventAction* eventAction)
: G4UserSteppingAction(),
    fEventAction(eventAction),
    fScoringVolumeEnv(0),
    fScoringVolume1(0),
    fScoringVolume2(0)
{
    filecount = 0;
    counter = 0; 
    OpenOfile();
    

    //outfile = TFile::Open("output.root");
    //G4Step* step;
    //tree->Branch("step",&step);    


}

void B1SteppingAction::OpenOfile(){ 
    std::string name = "run_";
    name.append(std::to_string(filecount++));
    name.append(".dat");
    ofile.open(name);
    //ofile << " " << setw(5) << "trackid" << " " 
    ofile  << setw(5) << "EventID/I:" 
        << setw(5) << "particle/C:"
        << setw(10) << "volumeName/I:" 
        << setw(10) << "edepStep_keV/D:"
        << setw(10) << "KEparticle_keV/D:"
        << setw(10) << "global_t_ns/D:" 
        << setw(10) << "steplen_mm/D:" 
        << setw(10) << "momentum_keV/D:"
        //<< " " << setw(10) << "momentum.x/keV" << " "
        //<< " " << setw(10) << "momentum.y/mm" << " "
        //<< " " << setw(10) << "momentum.z/mm" << " " 
        << setw(10) << "globalx_um/D:" 
        << setw(10) << "globaly_um/D:" 
        << setw(10) << "globalz_um/D"  <<G4endl;
        //<< " " << setw(10) << "local.x()/mm" << " "
        //<< " " << setw(10) << "local.y()/mm" << " "
        //<< " " << setw(10) << "local.z()/mm" << " " << G4endl;

}

void B1SteppingAction::CloseOfile(){ 
    ofile.close();
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{
    //ofile.close();
    CloseOfile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step* step)
{
    if (!fScoringVolumeEnv) { 
        const B1DetectorConstruction* detectorConstruction
            = static_cast<const B1DetectorConstruction*>
            (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        fScoringVolumeEnv = detectorConstruction->GetScoringVolumeEnv();   
    }
    if (!fScoringVolume1) { 
        const B1DetectorConstruction* detectorConstruction
            = static_cast<const B1DetectorConstruction*>
            (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        fScoringVolume1 = detectorConstruction->GetScoringVolume1();   
    }
    if (!fScoringVolume2) { 
        const B1DetectorConstruction* detectorConstruction
            = static_cast<const B1DetectorConstruction*>
            (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        fScoringVolume2 = detectorConstruction->GetScoringVolume2();   
    }

    // get volume of the current step
    G4LogicalVolume* volume 
        = step->GetPreStepPoint()->GetTouchableHandle()
        ->GetVolume()->GetLogicalVolume();

    // check if we are in scoring volume
    //if (volume != fScoringVolume) return;

    // collect energy deposited in this step
    G4double edepStep = step->GetTotalEnergyDeposit();
    fEventAction->AddEdep(edepStep);  

    // TESTING ZONE


    // Get post-step point
    G4StepPoint* poststep = step->GetPostStepPoint();

    G4ThreeVector poststeppos = poststep->GetPosition(); // position
    G4double epart = poststep->GetTotalEnergy();         // particle energy
    G4ThreeVector ppart = poststep->GetMomentum();

    G4Track* track = step->GetTrack();
    //track->SetStepLength(0.005*mm);

    G4int trackid = track->GetTrackID();
    G4String partname = track->GetParticleDefinition()->GetParticleName();
    G4double KEparticle = track->GetKineticEnergy();  // get KE of particle along the track in each step
    G4double steplength = track->GetStepLength();
    G4String typelim = "proton";

    G4int EventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    G4TouchableHandle handle = step->GetPreStepPoint()->GetTouchableHandle();
    G4ThreeVector pos = handle->GetHistory()->GetTopTransform().TransformPoint(step->GetPostStepPoint()->GetPosition());

    G4int volumeName = 0;
    if(volume==fScoringVolumeEnv) {
        volumeName = 3; // envelope = vacuum
    }else if(volume==fScoringVolume1){
        volumeName = 1; // shape1 = Al
    }else if(volume==fScoringVolume2){
        volumeName = 2;  // shape2 = vacuum
    }else{
        volumeName = 0; // world = vacuum
    }

    if (fabs(EventID-counter)>50000){
        CloseOfile();
        OpenOfile();
        counter = EventID;
    }
    //ofile << " " << setw(5) << trackid << " " 
    ofile << " " << setw(5) << EventID << " " 
        << " " << setw(10) << partname << " "
        //<< " " << setw(10) <<volume->GetName() << " "
        << " " << setw(10) <<volumeName<< " "
        << " " << setw(10) << edepStep/keV << " "
        << " " << setw(10) << KEparticle/keV << " "
        << " " << setw(10) << track->GetGlobalTime()/ns << " "
        << " " << setw(10) << steplength/micrometer  << " "
        << " " << setw(10) << track->GetMomentum().mag()/keV << " "
        //<< " " << setw(10) << track->GetMomentum().x()/keV << "keV "
        //<< " " << setw(10) << track->GetMomentum().y()/keV << "keV "
        //<< " " << setw(10) << track->GetMomentum().z()/keV << "keV "
        << " " << setw(10) << poststeppos.x()/micrometer  << " "
        << " " << setw(10) << poststeppos.y()/micrometer  << " "
        << " " << setw(10) << poststeppos.z()/micrometer  << " "<<G4endl; 
        //<< " " << setw(10) << pos.x()/micrometer << " "
        //<< " " << setw(10) << pos.y()/micrometer << " "
        //<< " " << setw(10) << pos.z()/micrometer << " " << G4endl;
    //if(KEparticle<1*keV) ofile << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

