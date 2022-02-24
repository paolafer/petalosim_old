// ----------------------------------------------------------------------------
// petalosim | Back2backGammas.cc
//
// This generator simulates two gammas of 510.999 keV back to back, with
// random direction. It is possible to specify a limited solid angle.
// Control histograms of the solid angle where gammas are generated
// are produced.
//
// The PETALO Collaboration
// ----------------------------------------------------------------------------

#include "Back2backGammas.h"

#include "nexus/RandomUtils.h"
#include "nexus/DetectorConstruction.h"
#include "nexus/GeometryBase.h"
#include "nexus/FactoryBase.h"

#include <G4Event.hh>
#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4ParticleTable.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <TFile.h>
#include <TH1F.h>


using namespace CLHEP;
using namespace nexus;

REGISTER_CLASS(Back2backGammas, G4VPrimaryGenerator)

Back2backGammas::Back2backGammas(): geom_(0), costheta_min_(-1.),
                                    costheta_max_(1.),
                                    phi_min_(0.), phi_max_(2.*pi)
{
  //G4cout << "Limits = " << std::numeric_limits<unsigned int>::max() << G4endl;
  msg_ = new G4GenericMessenger(this, "/Generator/Back2back/",
                                "Control commands of 511-keV back to back gammas generator.");

  msg_->DeclareProperty("region", region_,
                        "Set the region of the geometry where the vertex will be generated.");

  msg_->DeclareProperty("min_costheta", costheta_min_,
                        "Set minimum cosTheta for the direction of the particle.");
  msg_->DeclareProperty("max_costheta", costheta_max_,
                        "Set maximum cosTheta for the direction of the particle.");
  msg_->DeclareProperty("min_phi", phi_min_,
                        "Set minimum phi for the direction of the particle.");
  msg_->DeclareProperty("max_phi", phi_max_,
                        "Set maximum phi for the direction of the particle.");

  DetectorConstruction* detconst =
    (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  geom_ = detconst->GetGeometry();

  theta_angle_ = new TH1F("CosTheta", "CosTheta", 180, -2, 2);
  theta_angle_->GetXaxis()->SetTitle("Cos(theta)");
  theta_angle_->GetYaxis()->SetTitle("Entries");

  phi_angle_ = new TH1F("Phi", "Phi", 360, -pi-2, pi+2);
  phi_angle_->GetXaxis()->SetTitle("Phi (rad)");
  phi_angle_->GetYaxis()->SetTitle("Entries");
}

Back2backGammas::~Back2backGammas()
{
  out_file_ = new TFile("GenerationAngles.root", "recreate");
  theta_angle_->Write();
  phi_angle_->Write();
  out_file_->Close();
}

void Back2backGammas::GeneratePrimaryVertex(G4Event* evt)
{
  G4ParticleDefinition* gamma =
    G4ParticleTable::GetParticleTable()->FindParticle("gamma");

  G4double r = G4RandGauss::shoot(0., 0.00092); // it gives
   // a distribution of deviation of collinearity with 0.5 degrees FWHM.

  G4double e1 = electron_mass_c2 + r;
  G4double e2 = electron_mass_c2 - r;

  auto p1 = e1 * (
            (costheta_min_ != -1. || costheta_max_ != 1. || phi_min_ != 0. || phi_max_ != 2.*pi) ?
              RandomDirectionInRange(costheta_min_, costheta_max_, phi_min_, phi_max_):
            G4RandomDirection());

  G4ThreeVector position = geom_->GenerateVertex(region_);
  G4double time = 0.;
  auto vertex = new G4PrimaryVertex(position, time);

  vertex->SetPrimary(new G4PrimaryParticle(gamma,  p1.x(),  p1.y(),  p1.z()));

  G4double DeltaTeta = 2*r/electron_mass_c2;
  G4double Phi1      = (twopi * G4UniformRand())/2. ;

  G4ThreeVector p2(sin(DeltaTeta)*cos(Phi1), sin(DeltaTeta)*sin(Phi1), cos(DeltaTeta));
  p2.rotateUz(p1.unit());

  vertex->SetPrimary(new G4PrimaryParticle(gamma, -e2*p2.x(), -e2*p2.y(), -e2*p2.z()));

  evt->AddPrimaryVertex(vertex);

  // theta_angle_->Fill(p.z());
  G4double angle = std::acos(-p1.x()*p2.x()-p1.y()*p2.y()-p1.z()*p2.z());
  theta_angle_->Fill(angle);
  theta_angle_->Fill(-angle);
  phi_angle_->Fill(std::atan2(p1.y(), p1.x()));
}
