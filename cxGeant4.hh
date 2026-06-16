#ifndef CXGEANT4_HH__
#define CXGEANT4_HH__

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4ParticleTable.hh>
#include <G4UIcommand.hh>
#include <G4VSolid.hh>
#include <G4VisAttributes.hh>
#include <spdlog/spdlog.h>
#include <string>
#include <tuple>
#include <yaml-cpp/yaml.h>

namespace cxfunc::cxG4 {
extern int theVerboseLevel;

void init(const YAML::Node& aConf);
void initfast();
auto GetValue(const std::string& a) -> double;
inline auto GetValue(const YAML::Node& a) -> double {
  return GetValue(a.as<std::string>());
}

void DumpParticle(const std::string &);

void DumpPhysics();

inline auto GetMaG4Nist() -> G4NistManager& {
  return *(G4NistManager::Instance());
}
auto GetBoxXYZ(const G4LogicalVolume* a) -> std::tuple<double, double, double>;

auto createVisAttributes(const YAML::Node&) -> G4VisAttributes*;
auto ConstructMaterial(const YAML::Node&) -> G4Material*;
auto ConstructSolid(const YAML::Node&) -> G4VSolid*;
auto ConstructLogicalVolume(const YAML::Node& c) -> G4LogicalVolume*;
auto ConstructPMTLV(const YAML::Node& c, const YAML::Node& pmtdb) -> G4LogicalVolume*;

}  // namespace cxfunc::cxG4

#endif
