// ----------------------------------------------------------------------------
// petalosim | PetKDBFixedPitch.h
//
// Kapton dice board with a fixed distance among SiPMs.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef PET_KDBFixedPitch_H
#define PET_KDBFixedPitch_H

#include "nexus/GeometryBase.h"
#include <G4Material.hh>
#include <vector>

class G4GenericMessenger;

class SiPMpetVUV;

using namespace nexus;

/// Geometry of the Kapton Dice Boards (KDBFixedPitch) used in the NEW detector

class PetKDBFixedPitch : public GeometryBase
{
public:
  /// Constructor
  PetKDBFixedPitch();

  /// Destructor
  ~PetKDBFixedPitch();

  // Dimension setter
  void SetXYsize(G4double xysize);
  void SetPitchSize(G4double pitch);

  G4ThreeVector GetDimensions() const;
  const std::vector<std::pair<int, G4ThreeVector> > &GetPositions();

  /// Builder
  virtual void Construct();

  void SetMaterial(G4Material &mat);

private:
  G4ThreeVector dimensions_;
  std::vector<std::pair<int, G4ThreeVector> > positions_;

  // Visibility of the shielding
  G4bool visibility_;

  // SiPM pitch
  G4double sipm_pitch_;

  // Reflectivity of the board in LXe
  G4double refl_;

  // xy dimensions
  G4double xysize_;

  // Messenger for the definition of control commands
  G4GenericMessenger *msg_;

  SiPMpetVUV *sipm_;
};

#endif
