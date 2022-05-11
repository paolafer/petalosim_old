// ----------------------------------------------------------------------------
// petalosim | ChargeHit.h
//
// This class describe the charge read by a generic charge detector.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef CHARGE_HIT_H
#define CHARGE_HIT_H

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4ThreeVector.hh>

class ChargeHit: public G4VHit
{
 public:
  /// Constructor
  ChargeHit();
  /// Destructor
  ~ChargeHit();

  /// Returns the time bin size
  G4double GetBinSize() const;
  /// Sets the time bin size.
  void SetBinSize(G4double);

  /// Returns the detector ID code
  G4int GetSensorID() const;
  /// Sets the detector ID code
  void  SetSensorID(G4int);

  G4ThreeVector GetPosition() const;
  void SetPosition(G4ThreeVector);

  /// Adds counts to a given time bin
  void Fill(G4double time, G4int counts=1);

  /// Get the map
  const std::map<G4double, G4int>& GetChargeWaveform() const;

 private:
  G4double bin_size_;      ///< Size of time bin
  G4int sns_id_;           ///< Detector ID number
  G4ThreeVector position_; ///< Detector position

  /// Map with number of ionization e- detected per time bin
  std::map<G4double, G4int> map_;
};

typedef G4THitsCollection<ChargeHit> ChargeHitsCollection;

inline ChargeHit::ChargeHit() {}
inline ChargeHit::~ChargeHit() {}

inline G4double ChargeHit::GetBinSize() const { return bin_size_; }
inline void ChargeHit::SetBinSize(G4double bin_size) { bin_size_ = bin_size; }

inline G4int ChargeHit::GetSensorID() const { return sns_id_; }
inline void ChargeHit::SetSensorID(G4int id) { sns_id_ = id; }

inline G4ThreeVector ChargeHit::GetPosition() const { return position_; }
inline void ChargeHit::SetPosition(G4ThreeVector xyz)
{ position_ = xyz; }

inline void ChargeHit::Fill(G4double time, G4int counts)
{G4double time_bin = floor(time/bin_size_) * bin_size_;
  map_[time_bin] += counts;
}

inline const std::map<G4double, G4int>& ChargeHit::GetChargeWaveform() const
{ return map_; }


#endif
