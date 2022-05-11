// ----------------------------------------------------------------------------
// petalosim | PetaloPersistencyManager.cc
//
// This class writes all the relevant information of the simulation
// to an ouput file.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "PetaloPersistencyManager.h"
#include "HDF5Writer.h"
#include "ChargeSD.h"

#include "nexus/Trajectory.h"
#include "nexus/TrajectoryMap.h"
#include "nexus/IonizationSD.h"
#include "nexus/SensorSD.h"
#include "nexus/NexusApp.h"
#include "nexus/DetectorConstruction.h"
#include "nexus/SaveAllSteppingAction.h"
#include "nexus/FactoryBase.h"

#include <G4GenericMessenger.hh>
#include <G4Event.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <G4RunManager.hh>
#include <G4Run.hh>
#include <G4OpticalPhoton.hh>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "CLHEP/Units/SystemOfUnits.h"

using namespace nexus;
using namespace CLHEP;

REGISTER_CLASS(PetaloPersistencyManager, PersistencyManagerBase)

PetaloPersistencyManager::PetaloPersistencyManager():
  PersistencyManagerBase(), msg_(0), ready_(false), store_evt_(true),
  store_steps_(false), interacting_evt_(false), save_int_e_numb_(false),
  event_type_("other"), saved_evts_(0), interacting_evts_(0),
  nevt_(0), start_id_(0), first_evt_(true),
  thr_charge_(0), tof_time_(50.*nanosecond), sns_only_(false),
  save_tot_charge_(true), h5writer_(0)
{
  msg_ = new G4GenericMessenger(this, "/nexus/persistency/");
  msg_->DeclareMethod("outputFile", &PetaloPersistencyManager::OpenFile, "");
  msg_->DeclareProperty("eventType", event_type_,
                        "Type of event: bb0nu, bb2nu or background.");
  msg_->DeclareProperty("start_id", start_id_,
                        "Starting event ID for this job.");
  msg_->DeclareProperty("thr_charge", thr_charge_, "Threshold for the charge saved in file.");
  msg_->DeclareProperty("sns_only", sns_only_, "If true, no true information is saved.");
  msg_->DeclareProperty("save_tot_charge", save_tot_charge_, "If true, total charge is saved.");

  G4GenericMessenger::Command& time_cmd =
    msg_->DeclareProperty("tof_time", tof_time_, "Time saved in tof table per sensor");
  time_cmd.SetUnitCategory("Time");
  time_cmd.SetParameterName("tof_time", false);
  time_cmd.SetRange("tof_time>0.");

  init_macro_ = "";
  macros_.clear();
  delayed_macros_.clear();
  secondary_macros_.clear();
}



PetaloPersistencyManager::~PetaloPersistencyManager()
{
  delete msg_;
  delete h5writer_;
}



void PetaloPersistencyManager::OpenFile(G4String filename)
{
  h5writer_ = new HDF5Writer();
  G4String hdf5file = filename + ".h5";
  h5writer_->Open(hdf5file, store_steps_);
  return;
}



void PetaloPersistencyManager::CloseFile()
{
  h5writer_->Close();
  return;
}



G4bool PetaloPersistencyManager::Store(const G4Event* event)
{
  if (interacting_evt_) {
    interacting_evts_++;
  }

  if (!store_evt_) {
    TrajectoryMap::Clear();
    if (store_steps_) {
      SaveAllSteppingAction* sa = (SaveAllSteppingAction*)
        G4RunManager::GetRunManager()->GetUserSteppingAction();
      sa->Reset();
    }
    return false;
  }

  saved_evts_++;


  if (first_evt_) {
    first_evt_ = false;
    nevt_ = start_id_;
  }

  if (store_steps_)
    StoreSteps();

  if (sns_only_ == false) {
    StoreTrajectories(event->GetTrajectoryContainer());
  }

  StoreHits(event->GetHCofThisEvent());

  nevt_++;

  TrajectoryMap::Clear();
  StoreCurrentEvent(true);

  return true;
}



void PetaloPersistencyManager::StoreTrajectories(G4TrajectoryContainer* tc)
{

  // If the pointer is null, no trajectories were stored in this event
  if (!tc) return;

  // Loop through the trajectories stored in the container
  for (size_t i=0; i<tc->entries(); ++i) {
    Trajectory* trj = dynamic_cast<Trajectory*>((*tc)[i]);
    if (!trj) continue;

    G4bool save_opt_phot = false;
    std::ifstream init_read(init_macro_, std::ifstream::in);

    while (init_read.good()) {
      std::string key, value;
      std::getline(init_read, key, ' ');
      std::getline(init_read, value);
      if ((key == "/nexus/RegisterTrackingAction") && (value == "OpticalTrackingAction")) {
        save_opt_phot = true;
        break;
      }
    }

    if ((trj->GetParticleDefinition() == G4OpticalPhoton::Definition()) &&
        (save_opt_phot == false)) {
      continue;
    }

    G4int trackid = trj->GetTrackID();

    G4double length = trj->GetTrackLength();

    G4ThreeVector ini_xyz = trj->GetInitialPosition();
    G4double ini_t = trj->GetInitialTime();
    G4ThreeVector final_xyz = trj->GetFinalPosition();
    G4double final_t = trj->GetFinalTime();

    G4String ini_volume = trj->GetInitialVolume();
    G4String final_volume = trj->GetFinalVolume();

    G4double mass = trj->GetParticleDefinition()->GetPDGMass();
    G4ThreeVector ini_mom = trj->GetInitialMomentum();
    G4double energy = sqrt(ini_mom.mag2() + mass*mass);
    G4ThreeVector final_mom = trj->GetFinalMomentum();

    float kin_energy = energy - mass;
    char primary = 0;
    G4int mother_id = 0;
    if (!trj->GetParentID()) {
      primary = 1;
    } else {
      mother_id = trj->GetParentID();
    }

    h5writer_->WriteParticleInfo(nevt_, trackid, trj->GetParticleName().c_str(),
				 primary, mother_id,
				 (float)ini_xyz.x(), (float)ini_xyz.y(),
                                 (float)ini_xyz.z(), (float)ini_t,
				 (float)final_xyz.x(), (float)final_xyz.y(),
                                 (float)final_xyz.z(), (float)final_t,
				 ini_volume.c_str(), final_volume.c_str(),
				 (float)ini_mom.x(), (float)ini_mom.y(),
                                 (float)ini_mom.z(), (float)final_mom.x(),
                                 (float)final_mom.y(), (float)final_mom.z(),
				 kin_energy, length,
                                 trj->GetCreatorProcess().c_str(),
				 trj->GetFinalProcess().c_str());

  }
}



void PetaloPersistencyManager::StoreHits(G4HCofThisEvent* hce)
{
  if (!hce) return;

  G4SDManager* sdmgr = G4SDManager::GetSDMpointer();
  G4HCtable* hct = sdmgr->GetHCtable();

  // Loop through the hits collections
  for (int i=0; i<hct->entries(); i++) {

    // Collection are identified univocally (in principle) using
    // their id number, and this can be obtained using the collection
    // and sensitive detector names.
    G4String hcname = hct->GetHCname(i);
    G4String sdname = hct->GetSDname(i);
    int hcid = sdmgr->GetCollectionID(sdname+"/"+hcname);

    // Fetch collection using the id number
    G4VHitsCollection* hits = hce->GetHC(hcid);

    if (hcname == IonizationSD::GetCollectionUniqueName()) {
      if (sns_only_ == false) {
	StoreIonizationHits(hits);
      }
    }
    else if (hcname == SensorSD::GetCollectionUniqueName())
      StoreSensorHits(hits);
    else if (hcname == ChargeSD::GetCollectionUniqueName())
      StoreChargeHits(hits);
    else {
      G4String msg =
        "Collection of hits '" + sdname + "/" + hcname
        + "' is of an unknown type and will not be stored.";
      G4Exception("StoreHits()", "[PetaloPersistencyManager]", JustWarning, msg);
    }
  }
}



void PetaloPersistencyManager::StoreIonizationHits(G4VHitsCollection* hc)
 {
   IonizationHitsCollection* hits =
     dynamic_cast<IonizationHitsCollection*>(hc);
   if (!hits) return;

   std::string sdname = hits->GetSDname();

   for (size_t i=0; i<hits->entries(); i++) {

     IonizationHit* hit = dynamic_cast<IonizationHit*>(hits->GetHit(i));
     if (!hit) continue;

     G4int trackid = hit->GetTrackID();
     G4ThreeVector hit_pos = hit->GetPosition();

     h5writer_->WriteHitInfo(nevt_, trackid,
			     hit_pos[0], hit_pos[1], hit_pos[2],
			     hit->GetTime(), hit->GetEnergyDeposit(),
			     sdname.c_str());
   }

 }



void PetaloPersistencyManager::StoreSensorHits(G4VHitsCollection* hc)
{
  std::map<G4int, SensorHit*> mapOfHits;

  SensorHitsCollection* hits = dynamic_cast<SensorHitsCollection*>(hc);
  if (!hits) return;

  std::vector<G4int > sensor_ids;
  for (size_t i=0; i<hits->entries(); i++) {

    SensorHit* hit = dynamic_cast<SensorHit*>(hits->GetHit(i));
    if (!hit) continue;

    int s_id  = hit->GetPmtID();
    mapOfHits[s_id] = hit;

    const std::map<G4double, G4int>& wvfm = hit->GetHistogram();
    std::map<G4double, G4int>::const_iterator it;

    G4double amplitude = 0.;
    for (it = wvfm.begin(); it != wvfm.end(); ++it) {
      amplitude = amplitude + (*it).second;
    }
    if (hit->GetPmtID() >= 0) {
      G4int sens_id;
      sens_id = hit->GetPmtID();

      if (amplitude > thr_charge_){
        sensor_ids.push_back(sens_id);
      }
    } else if (hit->GetPmtID()<0) {
      tof_bin_size_ = hit->GetBinSize();
    }
  }

  for (G4int s_id: sensor_ids){
    std::string sdname = hits->GetSDname();

    SensorHit* hit = mapOfHits[s_id];
    G4ThreeVector xyz = hit->GetPosition();

    const std::map<G4double, G4int>& wvfm = hit->GetHistogram();
    std::map<G4double, G4int>::const_iterator it;

    if (save_tot_charge_ == true) {
      G4double charge = 0.;
      for (it = wvfm.begin(); it != wvfm.end(); ++it) {
        charge = charge + (*it).second;
      }
        h5writer_->WriteSensorDataInfo(nevt_, (unsigned int)hit->GetPmtID(), charge);
    }

    if (hit->GetPmtID() >= 0) {
      std::vector<G4int>::iterator pos_it =
	std::find(sns_posvec_.begin(), sns_posvec_.end(), hit->GetPmtID());
      if (pos_it == sns_posvec_.end()) {
	h5writer_->WriteSensorPosInfo((unsigned int)hit->GetPmtID(), sdname.c_str(),
                                      (float)xyz.x(), (float)xyz.y(), (float)xyz.z());
	sns_posvec_.push_back(hit->GetPmtID());
      }
    }


    // TOF
    SensorHit* hitTof = mapOfHits[-s_id];
    const std::map<G4double, G4int>& wvfmTof = hitTof->GetHistogram();

    double binsize_tof = hitTof->GetBinSize();

    for (it = wvfmTof.begin(); it != wvfmTof.end(); ++it) {

      if (((*it).first) <= tof_time_){
        unsigned int time_bin_tof = (unsigned int)((*it).first/binsize_tof+0.5);
        unsigned int charge_tof = (unsigned int)((*it).second+0.5);
        h5writer_->WriteSensorTofInfo(nevt_, hitTof->GetPmtID(), time_bin_tof, charge_tof);
      }
      else {
        break;
      }
    }

    /*
    const std::map<G4double, G4double>&  wvls= hit->GetWavelengths();
    std::map<G4double, G4double>::const_iterator w;

    std::vector<double > value;
    int count =0;
    std::ostringstream strs;
    strs << hit->GetPmtID();
    std::string sens_id = strs.str();
    //   G4cout << "Longitud = " << wvls.size() << G4endl;
    for (w = wvls.begin(); w != wvls.end(); ++w) {
      if (count < 100) {
	value.clear();
	std::ostringstream strs2;
	strs2 << count;
	std::string order = strs2.str();
	std::string key = sens_id + '_' + order;
	value.push_back(w->first/CLHEP::picosecond);
	value.push_back(w->second/CLHEP::nanometer);
	//	G4cout << key << ", " << value[0] << ", " << value[1] << G4endl;
	ievt->fstore(key, value);
	count++;
      }
    }
    */
  }
}


void PetaloPersistencyManager::StoreChargeHits(G4VHitsCollection* hc)
{
  ChargeHitsCollection* hits = dynamic_cast<ChargeHitsCollection*>(hc);
  if (!hits) return;

  std::vector<G4int > sensor_ids;
  for (size_t i=0; i<hits->entries(); i++) {

    ChargeHit* hit = dynamic_cast<ChargeHit*>(hits->GetHit(i));
    if (!hit) continue;

    const std::map<G4double, G4int>& wvfm = hit->GetChargeWaveform();
    std::map<G4double, G4int>::const_iterator it;

    G4double charge = 0.;
    for (it = wvfm.begin(); it != wvfm.end(); ++it) {
      charge += (*it).second;
    }

    h5writer_->WriteChargeDataInfo(nevt_, (unsigned int)hit->GetSensorID(), charge);

    std::vector<G4int>::iterator pos_it =
      std::find(charge_posvec_.begin(), charge_posvec_.end(), hit->GetSensorID());
    if (pos_it == charge_posvec_.end()) {
      std::string sdname = hits->GetSDname();
      G4ThreeVector xyz  = hit->GetPosition();
      G4cout << "Writing " << hit->GetSensorID() << G4endl;
      h5writer_->WriteSensorPosInfo((unsigned int)hit->GetSensorID(), sdname.c_str(),
                                    (float)xyz.x(), (float)xyz.y(), (float)xyz.z());
      charge_posvec_.push_back(hit->GetSensorID());
    }
  }
}

void PetaloPersistencyManager::StoreSteps()
{
  SaveAllSteppingAction* sa = (SaveAllSteppingAction*)
    G4RunManager::GetRunManager()->GetUserSteppingAction();

  StepContainer<G4String> initial_volumes = sa->get_initial_volumes();
  StepContainer<G4String>   final_volumes = sa->get_final_volumes  ();
  StepContainer<G4String>      proc_names = sa->get_proc_names     ();

  StepContainer<G4ThreeVector> initial_poss = sa->get_initial_poss();
  StepContainer<G4ThreeVector>   final_poss = sa->get_final_poss  ();

  for (auto it = initial_volumes.begin(); it != initial_volumes.end(); ++it) {
    std::pair<G4int, G4String> key           = it->first;
    G4int                      track_id      = key.first;
    G4String                   particle_name = key.second;

    for (size_t step_id=0; step_id < it->second.size(); ++step_id) {
      h5writer_->WriteStep(nevt_, track_id, particle_name, step_id,
                           initial_volumes[key][step_id],
                             final_volumes[key][step_id],
                                proc_names[key][step_id],
                           initial_poss   [key][step_id].x(),
                           initial_poss   [key][step_id].y(),
                           initial_poss   [key][step_id].z(),
                             final_poss   [key][step_id].x(),
                             final_poss   [key][step_id].y(),
                             final_poss   [key][step_id].z());
    }
  }
  sa->Reset();
}

G4bool PetaloPersistencyManager::Store(const G4Run*)
{

  // Store the number of events to be processed
  NexusApp* app = (NexusApp*) G4RunManager::GetRunManager();
  G4int num_events = app->GetNumberOfEventsToBeProcessed();

  G4String key = "num_events";
  h5writer_->WriteRunInfo(key, std::to_string(num_events).c_str());
  key = "saved_events";
  h5writer_->WriteRunInfo(key, std::to_string(saved_evts_).c_str());
  key = "tof_bin_size";
  h5writer_->WriteRunInfo(key, (std::to_string(tof_bin_size_/picosecond)+" ps").c_str());

  if (save_int_e_numb_) {
     key = "interacting_events";
     h5writer_->WriteRunInfo(key,  std::to_string(interacting_evts_).c_str());
   }

  SaveConfigurationInfo(init_macro_);
  for (unsigned long i=0; i<macros_.size(); i++) {
    SaveConfigurationInfo(macros_[i]);
  }
  for (unsigned long i=0; i<delayed_macros_.size(); i++) {
    SaveConfigurationInfo(delayed_macros_[i]);
  }
  for (unsigned long i=0; i<secondary_macros_.size(); i++) {
    SaveConfigurationInfo(secondary_macros_[i]);
  }

  return true;
}

void PetaloPersistencyManager::SaveConfigurationInfo(G4String file_name)
{
  std::ifstream history(file_name, std::ifstream::in);
  while (history.good()) {

    G4String line;
    std::getline(history, line);
    if (line[0] == '#')
      continue;

    std::stringstream ss(line);
    G4String key, value;
    std::getline(ss, key, ' ');
    std::getline(ss, value);

    if (key != "") {
      auto found_binning = key.find("binning");
      auto found_other_macro = key.find("/control/execute");
      if ((found_binning == std::string::npos) &&
          (found_other_macro == std::string::npos)) {
        if (key[0] == '\n') {
          key.erase(0, 1);
        }
	h5writer_->WriteRunInfo(key.c_str(), value.c_str());
      }

      if (found_other_macro != std::string::npos)
        secondary_macros_.push_back(value);
    }

  }

  history.close();
}
