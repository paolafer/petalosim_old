// ----------------------------------------------------------------------------
// petalosim | PetaloPhysics.h
//
// This class registers any new physics process defined in nexus.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef PETALO_PHYSICS_H
#define PETALO_PHYSICS_H

#include <G4VPhysicsConstructor.hh>

class G4GenericMessenger;
class WavelengthShifting;
class PositronAnnihilation;

class PetaloPhysics : public G4VPhysicsConstructor
{
public:
  /// Constructor
  PetaloPhysics();
  /// Destructor
  ~PetaloPhysics();

  /// Construct all required particles (Geant4 mandatory method)
  virtual void ConstructParticle();
  /// Construct all required physics processes (Geant4 mandatory method)
  virtual void ConstructProcess();

private:
  G4bool risetime_; ///< Rise time for LYSO

  G4bool noCompt_; ///< Switch on/off Compton scattering

  G4bool nest_; ///< Switch on/off NEST

  G4GenericMessenger *msg_;
  WavelengthShifting *wls_;
  PositronAnnihilation* pos_annihil_;
};

#endif
