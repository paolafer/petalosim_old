// ----------------------------------------------------------------------------
// petalosim | PetNESTStackingAction.h
//
// This is the stacking action needed to use NEST.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef PET_NEST_STACKING_ACTION_H
#define PET_NEST_STACKING_ACTION_H

#include <G4UserStackingAction.hh>

#include <NESTStackingAction.hh>

// General-purpose user stacking action

class PetNESTStackingAction : public NESTStackingAction
{
public:
  /// Constructor
  PetNESTStackingAction();
  /// Destructor
  ~PetNESTStackingAction();

};

#endif
