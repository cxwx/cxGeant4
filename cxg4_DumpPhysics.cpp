#include "testDetectorConstruction.hh"
#include "cxGeant4.hh"
#include "testPhysicsList.hh"
#include <G4RunManager.hh>

using namespace cxfunc::cxG4;
auto main() -> int {
  auto *runManager = new G4RunManager;
  auto * a = new testDetectorConstruction;
  runManager->SetUserInitialization(a);
  runManager->SetUserInitialization(new testPhysicsList);
  // runManager->InitializePhysics(); //still some bug need geom.
  runManager->Initialize();  // still some bug need geom.
  DumpPhysics();
}
