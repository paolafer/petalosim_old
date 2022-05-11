// ----------------------------------------------------------------------------
// petalosim | ChargeSD.h
//
// This class is a sensitive detector for charge.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef CHARGE_SD_H
#define CHARGE_SD_H

#include "ChargeHit.h"

#include <G4VSensitiveDetector.hh>

class G4HCofThisEvent;

class ChargeSD : public G4VSensitiveDetector
{
public:
  /// Constructor providing names for the sensitive detector
  /// and the collection of hits
  ChargeSD(G4String sdname);
  /// The destructor
  ~ChargeSD();

  /// Initialization of the sensitive detector. Invoked at the beginning
  /// of every event. The collection of hits is created here and registered
  /// in the event (so that it can be retrieved thru the G4HCofThisEvent object).
  void Initialize(G4HCofThisEvent *);

  /// Method invoked at the end of every event
  void EndOfEvent(G4HCofThisEvent *);

  /// Return the time binning chosen for the hits
  G4double GetTimeBinning() const;
  /// Set a time binning for the hits
  void SetTimeBinning(G4double);

  /// Return the unique name of the hits collection created
  /// by this sensitive detector. This will be used by the
  /// persistency manager to select the collection.
  static G4String GetCollectionUniqueName();

 private:
  G4bool ProcessHits(G4Step* step, G4TouchableHistory*);

  G4int FindSensorID(const G4VTouchable*);

  ChargeHitsCollection *HC_; ///< Pointer to the collection of hits

  G4double timebinning_; ///< Time bin width

};

inline G4double ChargeSD::GetTimeBinning() const { return timebinning_; }
inline void ChargeSD::SetTimeBinning(G4double tb) { timebinning_ = tb; }

#endif
