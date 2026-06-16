#include "testDetectorConstruction.hh"
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4RotationMatrix.hh>
#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>

using namespace std;

auto testDetectorConstruction::Construct() -> G4VPhysicalVolume* {
  G4NistManager* aNist = G4NistManager::Instance();
  G4Material* aMaterial = aNist->FindOrBuildMaterial("G4_WATER");
  auto* abox = new G4Box("WSDDetector", 5 * m, 5 * m, 5 * m);
  auto* World_log = new G4LogicalVolume(abox, aMaterial, "World");
  G4VPhysicalVolume* World_phys =
      new G4PVPlacement(nullptr, G4ThreeVector(), World_log, "World", nullptr, false, 0, false);
  return World_phys;
}
