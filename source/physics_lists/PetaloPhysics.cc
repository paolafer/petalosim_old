// ----------------------------------------------------------------------------
// petalosim | PetaloPhysics.cc
//
// This class registers any new physics process defined in petalosim.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "PetaloPhysics.h"
#include "WavelengthShifting.h"
#include "PositronAnnihilation.h"

#include "NESTProc.hh"
#include "PetaloDetector.hh"

#include <G4Scintillation.hh>
#include <G4GenericMessenger.hh>
#include <G4OpticalPhoton.hh>
#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessTable.hh>
#include <G4StepLimiter.hh>
#include <G4FastSimulationManagerProcess.hh>
#include <G4PhysicsConstructorFactory.hh>

/// Macro that allows the use of this physics constructor
/// with the generic physics list
G4_DECLARE_PHYSCONSTR_FACTORY(PetaloPhysics);

PetaloPhysics::PetaloPhysics() : G4VPhysicsConstructor("PetaloPhysics"),
                                 risetime_(false), noCompt_(false), nest_(false)
{
  msg_ = new G4GenericMessenger(this, "/PhysicsList/Petalo/",
                                "Control commands of the nexus physics list.");

  msg_->DeclareProperty("scintRiseTime", risetime_,
                        "Must be true if LYSO is used");

  msg_->DeclareProperty("offCompt", noCompt_,
                        "If true, switches off Compton Scattering.");

  msg_->DeclareProperty("nest", nest_,
                        "If true, NEST is used for scintillation.");
}

PetaloPhysics::~PetaloPhysics()
{
  delete msg_;
  delete wls_;
}

void PetaloPhysics::ConstructParticle()
{
  G4OpticalPhoton::Definition();
  NEST::NESTThermalElectron::Definition();
}

void PetaloPhysics::ConstructProcess()
{
  G4ProcessManager *pmanager = 0;

  // Add our own wavelength shifting process for the optical photon
  pmanager = G4OpticalPhoton::Definition()->GetProcessManager();
  if (!pmanager)
  {
    G4Exception("ConstructProcess()", "[PetaloPhysics]", FatalException,
                "G4OpticalPhoton without a process manager.");
  }

  wls_ = new WavelengthShifting();
  pmanager->AddDiscreteProcess(wls_);

  pmanager = G4Positron::Definition()->GetProcessManager();

  // Remove Geant4 annihilation process
  G4VProcess* eplusAnnihilation =
    G4ProcessTable::GetProcessTable()->FindProcess("annihil", G4Positron::Definition());
  pmanager->RemoveProcess(eplusAnnihilation);
  // Add our custom-made process
  pos_annihil_ = new PositronAnnihilation();
  pmanager->AddDiscreteProcess(pos_annihil_);
  pmanager->SetProcessOrderingToFirst(pos_annihil_, idxAtRest);

  // Add rise time to scintillation
  if (risetime_)
  {
    pmanager = G4Electron::Definition()->GetProcessManager();
    G4Scintillation *theScintillationProcess =
        (G4Scintillation *)G4ProcessTable::GetProcessTable()->FindProcess("Scintillation",
                                                                          G4Electron::Definition());
    theScintillationProcess->SetFiniteRiseTime(true);
  }

  if (noCompt_)
  {
    pmanager = G4Gamma::Definition()->GetProcessManager();
    G4VProcess *cs =
      G4ProcessTable::GetProcessTable()->FindProcess("compt", G4Gamma::Definition());
    pmanager->RemoveProcess(cs);
  }


  if (nest_) {
    PetaloDetector* petalo = new PetaloDetector();
    NEST::NESTcalc* petaloCalc = new NEST::NESTcalc(petalo);
    NEST::NESTProc* theNESTScintillationProcess =
      new NEST::NESTProc("S1", fElectromagnetic, petaloCalc, petalo);
    theNESTScintillationProcess->SetDetailedSecondaries(true); // this is to use the full scintillation spectrum of LXe.
    //    theNESTScintillationProcess->SetStackElectrons(false); false if only light is collected
    
    auto aParticleIterator = GetParticleIterator();
    aParticleIterator->reset();
    while ((*aParticleIterator)()) {
      G4ParticleDefinition* particle = aParticleIterator->value();
      if (theNESTScintillationProcess->IsApplicable(*particle)) {
        pmanager = particle->GetProcessManager();
        pmanager->AddProcess(theNESTScintillationProcess, ordDefault+1, ordInActive, ordDefault+1);
      }
    }
  }
  
}
