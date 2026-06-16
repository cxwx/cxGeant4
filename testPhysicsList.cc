#include "testPhysicsList.hh"
#include <G4DecayPhysics.hh>
#include <G4EmCalculator.hh>
#include <G4EmExtraParameters.hh>
#include <G4EmExtraPhysics.hh>
#include <G4EmParameters.hh>
#include <G4EmStandardPhysics.hh>
#include <G4EmStandardPhysics_option1.hh>
#include <G4EmStandardPhysics_option2.hh>
#include <G4EmStandardPhysics_option3.hh>
#include <G4EmStandardPhysics_option4.hh>
#include <G4HadronElasticPhysics.hh>
#include <G4HadronPhysicsFTFP_BERT.hh>
#include <G4HadronicParameters.hh>
#include <G4IonPhysics.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4OpticalPhoton.hh>
#include <G4OpticalPhysics.hh>
#include <G4ProductionCuts.hh>
#include <G4StepLimiterPhysics.hh>
#include <G4StoppingPhysics.hh>
#include <G4SystemOfUnits.hh>
#include <G4UserSpecialCuts.hh>
#include <G4eIonisationParameters.hh>
#include <spdlog/spdlog.h>

using namespace std;

namespace cxfunc::cxG4 {
testPhysicsList::testPhysicsList() : G4VModularPhysicsList() {
  // RegisterPhysics(new G4EmStandardPhysics_option1);
  RegisterPhysics(new G4EmStandardPhysics);
  RegisterPhysics(new G4EmExtraPhysics);
  RegisterPhysics(new G4DecayPhysics);
  // G4HadronicParameters::Instance()->SetMaxEnergy(1000 * TeV);
  RegisterPhysics(new G4HadronElasticPhysics);
  RegisterPhysics(new G4HadronPhysicsFTFP_BERT);
  RegisterPhysics(new G4StoppingPhysics);
  RegisterPhysics(new G4IonPhysics);
  // RegisterPhysics(new G4NeutronTrackingCut);
  RegisterPhysics(new G4StepLimiterPhysics);

  G4OpticalParameters* apar = G4OpticalParameters::Instance();
  apar->SetBoundaryInvokeSD(true);
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics;
  RegisterPhysics(opticalPhysics);
}

void testPhysicsList::SetCuts() {
  SetCutsWithDefault();
}
}  // namespace cxfunc::cxG4
