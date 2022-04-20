// ----------------------------------------------------------------------------
// petalosim | JaszczakPhantom.cc
//
// This class implements the geometry of a Jaszczak phantom, filled with water.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "JaszczakPhantom.h"

#include "nexus/FactoryBase.h"
#include "nexus/Visibilities.h"

#include <G4Tubs.hh>
#include <G4Orb.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4RotationMatrix.hh>
#include <G4VisAttributes.hh>

using namespace nexus;

REGISTER_CLASS(JaszczakPhantom, GeometryBase)

JaszczakPhantom::JaszczakPhantom(): GeometryBase(),
                                    cylinder_inner_diam_(216*mm),
                                    cylinder_height_(186*mm),
                                    cylinder_thickn_(3.2*mm),
                                    sphere1_d_(9.5*mm),
                                    sphere2_d_(12.7*mm),
                                    sphere3_d_(15.9*mm),
                                    sphere4_d_(19.1*mm),
                                    sphere5_d_(25.4*mm),
                                    sphere6_d_(31.8*mm),
                                    sphere_height_(127*mm),
                                    rod1_d_(3.2*mm),
                                    rod2_d_(4.8*mm),
                                    rod3_d_(6.4*mm),
                                    rod4_d_(7.9*mm),
                                    rod5_d_(9.5*mm),
                                    rod6_d_(11.1*mm),
                                    rod_height_(88*mm)
{
}


JaszczakPhantom::~JaszczakPhantom()
{
}


void JaszczakPhantom::Construct()
{
  auto cyl_name = "CYLINDER";
  auto cylinder_solid =
    new G4Tubs(cyl_name, 0, (cylinder_inner_diam_+2*cylinder_thickn_)/2,
               (cylinder_height_+2*cylinder_thickn_)/2., 0, twopi);

  G4Material* aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  auto cylinder_logic =
    new G4LogicalVolume(cylinder_solid, aluminum, cyl_name);

  this->SetLogicalVolume(cylinder_logic);


  auto water_name = "WATER_BCKG";
  auto water_solid =
    new G4Tubs(water_name, 0, cylinder_inner_diam_/2, cylinder_height_/2., 0, twopi);

  G4Material* water = G4NistManager::Instance()->FindOrBuildMaterial("G4_water");
  auto water_logic =
    new G4LogicalVolume(water_solid, water, water_name);
  new G4PVPlacement(0, G4ThreeVector(0., 0., 0.),
                    water_logic, water_name, cylinder_logic, false, 0, true);

  // Spheres
  std::vector<G4double> sphere_radii =
    {sphere1_d_/2, sphere2_d_/2, sphere3_d_/2, sphere4_d_/2, sphere5_d_/2, sphere6_d_/2};
  auto radius_pos = cylinder_inner_diam_/4.;
  auto z_pos      = - cylinder_height_/2. + sphere_height_;

  for (unsigned long i=0; i<sphere_radii.size(); i++) {
    BuildSpheres(i, sphere_radii[i], radius_pos, z_pos, water_logic, water);
  }

  // Rods
  std::vector<G4double> rod_radii =
    {rod1_d_/2, rod2_d_/2, rod3_d_/2, rod4_d_/2, rod5_d_/2, rod6_d_/2};
  z_pos = - cylinder_height_/2. + rod_height_/2;
  for (unsigned long i=0; i<rod_radii.size(); i++) {
    BuildRods(i, rod_radii[i], z_pos, water_logic, water);
  }

}


void JaszczakPhantom::BuildSpheres(unsigned long n, G4double r, G4double r_pos, G4double z_pos,
                                   G4LogicalVolume* mother_logic, G4Material* mat) const
{
  auto sphere_name = "SPHERE" + std::to_string(n);
  auto sphere_solid = new G4Orb(sphere_name, r);
  auto sphere_logic = new G4LogicalVolume(sphere_solid, mat, sphere_name);
  auto angle = n * 60 * deg;
  auto x_pos = r_pos * cos(angle);
  auto y_pos = r_pos * sin(angle);
  new G4PVPlacement(0, G4ThreeVector(x_pos, y_pos, z_pos), sphere_logic,
                    sphere_name, mother_logic, false, 0, true);
}



void JaszczakPhantom::BuildRods(unsigned long n, G4double r, G4double z_pos,
                                G4LogicalVolume* mother_logic, G4Material* mat) const
  {
    auto diam = 2 * r;

    // Sector displacement from centre, to accommodate gap between sectors
    auto gap =  14.4 * mm;
    auto dx  = gap * cos(pi/6);
    auto dy  = gap * sin(pi/6);
    // Displacement of first rod WRT sector corner
    dx += r * sqrt(3);
    dy += r;
    // Basis vectors of rod lattice
    const auto Ax = 2.0, Ay = 0.0;
    const auto Bx = 1.0, By = sqrt(3);
    auto a = 0;
    for (bool did_b=true ; did_b; a+=1) {
      did_b = false;
      for (auto b = 0; /*break when touches the cylinder*/; b+=1, did_b = true) {
        auto x_pos = (a*Ax + b*Bx) * diam + dx;
        auto y_pos = (a*Ay + b*By) * diam + dy;
        auto margin = 0.1 * mm;
        if (sqrt(x_pos*x_pos + y_pos*y_pos) + r + margin >= cylinder_inner_diam_/2.) { break; }
        auto label = "ROD" + std::to_string(n);
        auto rod_solid = new G4Tubs(label, 0, r, rod_height_/2, 0, twopi);
        auto rod_logic = new G4LogicalVolume(rod_solid, mat, label);
        G4ThreeVector pos = G4ThreeVector(x_pos, y_pos, z_pos);
        G4ThreeVector newpos = pos.rotateZ(n*pi/3);
        new G4PVPlacement(0, newpos, rod_logic, label, mother_logic, false, 0, true);

        G4VisAttributes col = nexus::Blue();
        col.SetForceSolid(true);
        rod_logic->SetVisAttributes(col);
      }
    }
  }
