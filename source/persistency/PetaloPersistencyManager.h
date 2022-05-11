// ----------------------------------------------------------------------------
// petalosim | PetaloPersistencyManager.h
//
// This class writes all the relevant information of the simulation
// to an ouput file.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef P_PERSISTENCY_MANAGER_H
#define P_PERSISTENCY_MANAGER_H

#include "nexus/PersistencyManagerBase.h"
#include <G4VPersistencyManager.hh>
#include <vector>

class G4GenericMessenger;
class G4TrajectoryContainer;
class G4HCofThisEvent;
class G4VHitsCollection;

class HDF5Writer;

class PetaloPersistencyManager : public PersistencyManagerBase
{
public:
  PetaloPersistencyManager();
  ~PetaloPersistencyManager();
  PetaloPersistencyManager(const PetaloPersistencyManager &);
  /// Create the singleton instance of the persistency manager
  //static void Initialize(G4String init_macro, std::vector<G4String>& macros,
  //                       std::vector<G4String>& delayed_macros);

  /// Set whether to store or not the current event
  void StoreCurrentEvent(G4bool);
  void InteractingEvent(G4bool);
  void StoreSteps(G4bool);

  void SetElectricField(G4double);

  ///
  virtual G4bool Store(const G4Event *);
  virtual G4bool Store(const G4Run *);
  virtual G4bool Store(const G4VPhysicalVolume *);

  virtual G4bool Retrieve(G4Event *&);
  virtual G4bool Retrieve(G4Run *&);
  virtual G4bool Retrieve(G4VPhysicalVolume *&);

public:
  void OpenFile(G4String);
  void CloseFile();

private:
  void StoreTrajectories(G4TrajectoryContainer *);
  void StoreHits(G4HCofThisEvent *);
  void StoreIonizationHits(G4VHitsCollection *);
  void StoreSensorHits(G4VHitsCollection *);
  void StoreChargeHits(G4VHitsCollection *);
  void StoreSteps();

  void SaveConfigurationInfo(G4String history);

private:
  G4GenericMessenger *msg_; ///< User configuration messenger

  std::vector<G4String> secondary_macros_;

  G4bool ready_;           ///< Is the PetaloPersistencyManager ready to go?
  G4bool store_evt_;       ///< Should we store the current event?
  G4bool store_steps_;     ///< Should we store the steps for the current event?
  G4bool interacting_evt_; ///< Has the current event interacted in ACTIVE?

  G4double efield_; ///< Value of the electric field used in NEST

  G4String event_type_; ///< event type: bb0nu, bb2nu, background or not set

  std::vector<G4int> sns_posvec_;
  std::vector<G4int> charge_posvec_;

  G4int saved_evts_;                      ///< number of events to be saved
  G4int interacting_evts_;                ///< number of events interacting in ACTIVE
  G4double pmt_bin_size_, sipm_bin_size_; ///< bin width of sensors

  G4int nevt_;       ///< Event ID
  G4int start_id_;   ///< ID for the first event in file
  G4bool first_evt_; ///< true only for the first event of the run

  G4int thr_charge_;
  G4double tof_time_;
  G4bool sns_only_;
  G4bool save_tot_charge_;
  HDF5Writer *h5writer_; ///< Event writer to hdf5 file

  G4double bin_size_, tof_bin_size_, wire_bin_size_;
};

// INLINE DEFINITIONS //////////////////////////////////////////////

inline void PetaloPersistencyManager::StoreCurrentEvent(G4bool sce)
{
  store_evt_ = sce;
}
inline void PetaloPersistencyManager::StoreSteps(G4bool ss)
{
  store_steps_ = ss;
}
inline void PetaloPersistencyManager::InteractingEvent(G4bool ie)
{
  interacting_evt_ = ie;
}
inline void PetaloPersistencyManager::SetElectricField(G4double efield)
{
  efield_ = efield;
}
inline G4bool PetaloPersistencyManager::Store(const G4VPhysicalVolume *)
{
  return false;
}
inline G4bool PetaloPersistencyManager::Retrieve(G4Event *&)
{
  return false;
}
inline G4bool PetaloPersistencyManager::Retrieve(G4Run *&)
{
  return false;
}
inline G4bool PetaloPersistencyManager::Retrieve(G4VPhysicalVolume *&)
{
  return false;
}

#endif
