// ----------------------------------------------------------------------------
// petalosim | FullRingInfinity.h
//
// This class implements the geometry of a cylindric ring of LXe,
// of configurable length and diameter.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#ifndef FULL_RING_INF_H
#define FULL_RING_INF_H

#include "nexus/GeometryBase.h"
#include <vector>

class G4GenericMessenger;
class G4LogicalVolume;
class G4Material;

class SiPMpetVUV;
class JaszczakPhantom;

namespace nexus
{
  class SpherePointSampler;
}

using namespace nexus;

class FullRingInfinity : public GeometryBase
{

public:
  // Constructor
  FullRingInfinity();
  //Destructor
  ~FullRingInfinity();

  /// Generate a vertex within a given region of the geometry
  G4ThreeVector GenerateVertex(const G4String &region) const;

private:
  void Construct();
  void BuildCryostat();
  void BuildQuadSensors();
  void BuildSensors();
  void BuildPhantom();
  void BuildPointfile(G4String pointFile);
  G4int binarySearchPt(G4int low, G4int high, G4double rnd) const;
  G4ThreeVector RandomPointVertex() const;
  void CalculateSensitivityVertices(G4double binning);

  SiPMpetVUV *sipm_;

  G4LogicalVolume *lab_logic_;
  G4LogicalVolume *LXe_logic_;
  G4LogicalVolume *active_logic_;

  /// Messenger for the definition of control commands
  G4GenericMessenger *msg_;

  G4double axial_length_;
  G4double sipm_pitch_;
  G4int n_sipm_rows_;
  G4int instr_faces_; ///< number of instrumented faces
  G4bool charge_det_;
  G4double kapton_thickn_;
  G4double lxe_depth_;

  G4ThreeVector sipm_dim_;
  G4double offset_;

  G4double inner_radius_, external_radius_;
  G4double lxe_container_int_thickn_, lxe_container_ext_thickn_;
  G4double vessel_int_thickn_, vessel_ext_thickn_;
  G4double vacuum_thickn_;

  G4double phantom_diam_;
  G4double phantom_length_;

  G4double max_step_size_;

  SpherePointSampler *spheric_gen_;

  G4ThreeVector specific_vertex_;

  G4bool phantom_;

  // Variables for the point generator.
  G4int pt_Nx_, pt_Ny_, pt_Nz_;
  G4float pt_Lx_, pt_Ly_, pt_Lz_;
  G4float *pt_;

  G4bool sensitivity_;
  G4int events_per_point_;
  G4int sensitivity_point_id_;
  mutable G4int sensitivity_index_;
  mutable std::vector<G4ThreeVector> sensitivity_vertices_;
  G4double sensitivity_binning_;
  G4double sens_x_min_, sens_x_max_;
  G4double sens_y_min_, sens_y_max_;
  G4double sens_z_min_, sens_z_max_;

  G4Material* LXe_;
  JaszczakPhantom* jas_phantom_;
};

#endif
