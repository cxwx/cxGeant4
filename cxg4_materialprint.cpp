#include <G4Material.hh>
#include <G4NistManager.hh>
#include <exception>
#include <iostream>
#include <span>
using namespace std;

auto main(int argc, char** argv) -> int {
  G4NistManager* nistman = G4NistManager::Instance();
  auto args = span(argv, argc);
  if (argc != 2) {
    nistman->ListMaterials("all");
    cerr << "\n\n\n" << args[0] << " \t MaterialName(e.g. G4_AIR)\n";
    terminate();
  }
  G4Material* theMaterial = nistman->FindOrBuildMaterial(args[1]);
  G4cout << theMaterial << G4endl;
}
