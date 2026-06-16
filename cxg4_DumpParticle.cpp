#include "cxGeant4.hh"
#include <FTFP_BERT.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4RunManager.hh>
#include <span>
#include <spdlog/spdlog.h>
using cxfunc::cxG4::DumpParticle;
using spdlog::error;

using namespace std;

auto main(int argc, char** argv) -> int {
  auto args = span(argv, argc);
  if (argc != 2) {
    error("{}: {}", __func__, args[0]);
    error("{} particlename(e.g. mu-)", args[0]);
    terminate();
  }

  auto* runManager = new G4RunManager;
  G4VModularPhysicsList* physics = new FTFP_BERT;
  runManager->SetUserInitialization(physics);
  DumpParticle(args[1]);
}
