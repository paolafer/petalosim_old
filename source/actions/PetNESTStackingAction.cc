// ----------------------------------------------------------------------------
// petalosim | PetNESTStackingAction.cc
//
// This is the stacking action needed to use NEST.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "PetNESTStackingAction.h"

#include "nexus/FactoryBase.h"


REGISTER_CLASS(PetNESTStackingAction, G4UserStackingAction)

PetNESTStackingAction::PetNESTStackingAction(): NESTStackingAction()
{
}



PetNESTStackingAction::~PetNESTStackingAction()
{
}
