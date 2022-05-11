// ----------------------------------------------------------------------------
// petalosim | ChargeSD.cc
//
// This class is a sensitive detector for charge.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "ChargeSD.h"

#include <NESTProc.hh>

#include <G4SDManager.hh>

using namespace CLHEP;

ChargeSD::ChargeSD(G4String sdname) : G4VSensitiveDetector(sdname),
                                      timebinning_(1.*microsecond)
{
  // Register the name of the collection of hits
  collectionName.insert(GetCollectionUniqueName());
}

ChargeSD::~ChargeSD()
{
}

G4String ChargeSD::GetCollectionUniqueName()
{
  return "ChargeHitsCollection";
}

void ChargeSD::Initialize(G4HCofThisEvent *HCE)
{
  // Create a new collection of PMT hits
  HC_ = new ChargeHitsCollection(this->GetName(), this->GetCollectionName(0));

  G4int HCID = G4SDManager::GetSDMpointer()->
    GetCollectionID(this->GetName() + "/" + this->GetCollectionName(0));

  HCE->AddHitsCollection(HCID, HC_);
}

G4bool ChargeSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{
  // Check whether the track is a thermal electron (NEST particle for ie-)
  G4ParticleDefinition *pdef = step->GetTrack()->GetDefinition();
  if (pdef != NEST::NESTThermalElectron::Definition())
    return false;

  const G4VTouchable* touchable =
      step->GetPreStepPoint()->GetTouchable();

  G4int sns_id = FindSensorID(touchable);

  ChargeHit *hit = 0;
  for (size_t i=0; i<HC_->entries(); i++) {
    if ((*HC_)[i]->GetSensorID() == sns_id) {
      hit = (*HC_)[i];
      break;
    }
  }

  // If no hit associated to this sensor exists already,
  // create it and set main properties
  if (!hit) {
    hit = new ChargeHit();
    hit->SetSensorID(sns_id);
    hit->SetBinSize(timebinning_);
    hit->SetPosition(touchable->GetTranslation());
    HC_->insert(hit);
  }

  G4double time = step->GetPostStepPoint()->GetGlobalTime();
  hit->Fill(time);

  return true;
}

G4int ChargeSD::FindSensorID(const G4VTouchable* touchable)
{
  G4int snsid = touchable->GetCopyNumber(0);
  return snsid;
}

void ChargeSD::EndOfEvent(G4HCofThisEvent * /*HCE*/)
{
}
