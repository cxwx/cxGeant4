#include "cxGeant4.hh"
#include "testDetectorConstruction.hh"
#include <FTFP_BERT.hh>
#include <G4AnalysisManager.hh>
#include <G4Box.hh>
#include <G4Ellipsoid.hh>
#include <G4Exception.hh>
#include <G4GeometryManager.hh>
#include <G4GeometryTolerance.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4NistManager.hh>
#include <G4OpticalSurface.hh>
#include <G4PVPlacement.hh>
#include <G4RunManagerFactory.hh>
#include <G4SDManager.hh>
#include <G4Sphere.hh>
#include <G4SteppingVerbose.hh>
#include <G4SystemOfUnits.hh>
#include <G4TScoreNtupleWriter.hh>
#include <G4VisAttributes.hh>
#include <charconv>
#include <exception>
#include <string>

// #include <magic_enum/magic_enum_all.hpp> TODO: mac enum

using namespace std;

namespace cxfunc::cxG4 {
int theVerboseLevel = 0;

void init(const YAML::Node& a) {
  if (a["verbose"]) {
    theVerboseLevel = a["verbose"].as<int>();
  }
  if (a["bestUnitPrecision"]) {
    G4SteppingVerbose::UseBestUnit(a["bestUnitPrecision"].as<int>());
  }

  if (a["WorldMaximumExtent"]) {
    double worldExtent = GetValue(a["WorldMaximumExtent"]);
    G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldExtent);
    if (theVerboseLevel != 0) {
      spdlog::info("Tolerance surface:{}, angular:{}, radial:{}", G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / mm, G4GeometryTolerance::GetInstance()->GetAngularTolerance() / mm, G4GeometryTolerance::GetInstance()->GetRadialTolerance() / mm);
    }
  }

  auto runManagerType = G4RunManagerType::SerialOnly;
  if (a["RunManagerType"]) {
    // harper: ignore
    // ss -> enum not work yet
    // string ss = a["RunManagerType"].as<string>(); failed
    // runManagerType = magic_enum::enum_cast<G4RunManagerType>(ss);
    // from_chars(ss.data(), ss.data() + ss.size(), reinterpret_cast<underlying_type<G4RunManagerType>::type&>(runManagerType));
    runManagerType = G4RunManagerType(a["RunManagerType"].as<int>());
    if (theVerboseLevel != 0) {
      spdlog::info("run manager type:{}", static_cast<int>(runManagerType));
      // spdlog::info("vis {}", &(G4VisAttributes::GetInvisible()));  // ERROR:
    }
  }
  auto* aRunManager = G4RunManagerFactory::CreateRunManager(runManagerType);
  if (a["RunManager"]) {
    auto b = a["RunManager"];
    if (b["RandomNumberStore"]) {
      aRunManager->SetRandomNumberStore(b["RandomNumberStore"].as<bool>());
    }
    if (b["PrintProgress"]) {
      aRunManager->SetPrintProgress(b["PrintProgress"].as<int>());
    }
    if (b["NumberOfThreads"]) {
      aRunManager->SetNumberOfThreads(b["NumberOfThreads"].as<int>());
    }
  }
  if (a["TScoreNtupleWriter"]) {
    G4TScoreNtupleWriter<G4AnalysisManager> scoreNtupleWriter;
    if (a["TScoreNtupleWriter"]["VerboseLevel"]) {
      scoreNtupleWriter.SetVerboseLevel(a["TScoreNtupleWriter"]["VerboseLevel"].as<int>());
    }
    scoreNtupleWriter.SetNtupleMerging(true);
  }
  if (a["SDMpointer"]) {
    if (a["SDMpointer"]["verbose"]) {
      G4SDManager::GetSDMpointer()->SetVerboseLevel(a["SDMpointer"]["verbose"].as<int>());
    }
  }
}
void initfast() {
  auto* runManager = new G4RunManager;  // NOLINT
  runManager->SetUserInitialization(new testDetectorConstruction());  // NOLINT
  runManager->SetUserInitialization(new FTFP_BERT());  // NOLINT
  runManager->Initialize();
}
auto GetValue(const std::string& a) -> double {  // convert like 1.0 m
  if (theVerboseLevel != 0) {
    spdlog::debug("{}:{}", __func__, a);
  }
  double val = G4UIcommand::ConvertToDimensionedDouble(a.c_str());
  if (theVerboseLevel != 0) {
    spdlog::debug("{}:{}", __func__, val);
  }
  return val;
}

auto GetBoxXYZ(const G4LogicalVolume* a) -> std::tuple<double, double, double> {
  if (a == nullptr) {
    spdlog::error("logic volume not exist @ GetBoxXYZ");
    return {-1, -1, -1};
  }
  auto* solid = dynamic_cast<G4Box*>(a->GetSolid());
  if (solid == nullptr) {
    spdlog::error("Volume {} not G4Box", a->GetName());;
    return {-1, -1, -1};
  }
  auto x = solid->GetXHalfLength();
  auto y = solid->GetYHalfLength();
  auto z = solid->GetZHalfLength();
  return make_tuple(x, y, z);
}

namespace {
template <typename T>
void DumpParticle2(const T& id) {
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(id);
  particle->DumpTable();
}
}


void DumpParticle(const string & id) {
  int value = 0;
  const char* first = id.data();
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const char* last = first + id.size();
  auto [ptr, ec] = std::from_chars(first, last, value);

  if (ec == std::errc()) {
    DumpParticle2(value);
  } else {
    DumpParticle2(id.c_str());
  }
}



void DumpPhysics() {
  auto* runManager = G4RunManager::GetRunManager();
  // NEEDCHECK(CX):
  auto* list = const_cast<G4VUserPhysicsList*>(runManager->GetUserPhysicsList());  // NOLINT
  list->DumpList();
  list->DumpCutValuesTable();
  cout << (dynamic_cast<G4VModularPhysicsList*>(list)) << '\n';
}

auto ConstructMaterial(const YAML::Node& c) -> G4Material* {
  if (theVerboseLevel != 0) {
    spdlog::debug("{} -- {}", __func__, YAML::Dump(c));
  }
  auto type = c["buildtype"].as<string>();
  if (type == "BuildMaterialWithNewDensity") {
    auto g4name = c["G4name"].as<string>();
    G4Material* mat = GetMaG4Nist().FindOrBuildMaterial(g4name);
    double density = mat->GetDensity();
    string newname = g4name;
    if (c["name"]) {
      newname = c["name"].as<string>();
    }
    if (c["density"]) {
      density = cxG4::GetValue(c["density"]);
    }
    if (c["densityFactor"]) {
      density *= c["densityFactor"].as<double>();
    }
    auto* ma = GetMaG4Nist().BuildMaterialWithNewDensity(newname, g4name, density);
    if (theVerboseLevel != 0) {
      spdlog::debug("{} end --", __func__);
    }
    return ma;
  }
  if (type == "FindOrBuildMaterial") {  // careful for only once for each detector
    auto g4name = c["G4name"].as<string>();
    G4Material* mat = GetMaG4Nist().FindOrBuildMaterial(g4name);
    if (c["name"]) {
      mat->SetName(c["name"].as<string>());
    }
    if (theVerboseLevel != 0) {
      spdlog::debug("{} end --", __func__);
    }
    return mat;
  }
  if (type == "Find") {
    auto name = c["oldname"].as<string>();
    G4Material* mat = GetMaG4Nist().FindMaterial(name);
    if (c["name"]) {
      mat->SetName(c["name"].as<string>());
    }
    if (theVerboseLevel != 0) {
      spdlog::debug("{} end --", __func__);
    }
    return mat;
  }
  spdlog::error("unknown build type!");
  terminate();
}

auto ConstructSolid(const YAML::Node& a) -> G4VSolid* {
  if (theVerboseLevel != 0) {
    spdlog::debug("{} -- {}", __func__, YAML::Dump(a));
  }
  auto name = a["name"].as<string>();
  auto type = a["type"].as<string>();
  if (type == "G4Box") {
    double x = cxG4::GetValue(a["x"]) / 2.;
    double y = cxG4::GetValue(a["y"]) / 2.;
    double z = cxG4::GetValue(a["z"]) / 2.;
    auto* solid = new G4Box(name, x, y, z);
    if (theVerboseLevel != 0) {
      spdlog::debug("{} end --", __func__);
    }
    return solid;
  }
  if (type == "G4Ellipsoid") {
    double x = cxG4::GetValue(a["x"]) / 2.;
    double y = cxG4::GetValue(a["y"]) / 2.;
    double z = cxG4::GetValue(a["z"]) / 2.;
    double zmin = cxG4::GetValue(a["zmin"]);
    double zmax = cxG4::GetValue(a["zmax"]);
    auto* solid = new G4Ellipsoid(name, x, y, z, zmin, zmax);
    if (theVerboseLevel != 0) {
      spdlog::debug("{} end --", __func__);
    }
    return solid;
  }
  if (type == "cxG4Ellipsoid_rl_rs") {
    double rl = cxG4::GetValue(a["rl"]);
    double rs = cxG4::GetValue(a["rs"]);
    double zmin = cxG4::GetValue(a["zmin"]);
    double zmax = cxG4::GetValue(a["zmax"]);
    auto* solid = new G4Ellipsoid(name, rl, rl, rs, zmin, zmax);
    if (theVerboseLevel != 0) {
      spdlog::debug("{} end --", __func__);
    }
    return solid;
  }
  spdlog::error("failed {}: {}: {}", __func__, name, type);
  terminate();
}

auto createVisAttributes(const YAML::Node& c) -> G4VisAttributes* {
  auto* aVis = new G4VisAttributes;  // NOLINT
  if (c["Visibility"]) {
    aVis->SetVisibility(c["vis"]["Visibility"].as<bool>());
  }
  return aVis;
}

auto ConstructLogicalVolume(const YAML::Node& c) -> G4LogicalVolume* {
  if (theVerboseLevel != 0) {
    spdlog::debug("{} -- {}", __func__, YAML::Dump(c));
  }
  auto name = c["name"].as<string>();
  G4VSolid* solid = cxG4::ConstructSolid(c["solid"]);
  G4NistManager* nistman = G4NistManager::Instance();
  G4Material* material = nistman->FindMaterial(c["material"].as<string>());
  auto *l1 = new G4LogicalVolume(solid, material, name);
  if (c["vis"]) {
    auto *aVis = createVisAttributes(c["vis"]);
    l1->SetVisAttributes(aVis);
  }
  if (theVerboseLevel != 0) {
    spdlog::debug("{} end --", __func__);
  }
  return l1;
}

auto ConstructPMTLV(const YAML::Node& aConf, const YAML::Node& pmtdb) -> G4LogicalVolume* {  // only for Up Down PMT in material with RINDEX
  if (theVerboseLevel != 0) {
    spdlog::debug("{} -- {}", __func__, YAML::Dump(aConf));
  }
  bool aCheckForOverlaps = false;
  if (aConf["checkforoverlaps"]) {
    aCheckForOverlaps = aConf["checkforoverlaps"].as<bool>();
  }
  auto name = aConf["name"].as<string>();
  auto pmttype = aConf["solid"]["type"].as<string>();
  const YAML::Node& pmtconf = pmtdb[pmttype];
  double rl = cxG4::GetValue(pmtconf["rl"]);
  double rs = cxG4::GetValue(pmtconf["rs"]);
  double dglass = cxG4::GetValue(pmtconf["dglass"]);
  double dFe = cxG4::GetValue(pmtconf["dFe"]);

  double zmin = cxG4::GetValue(aConf["solid"]["zmin"]);
  double zmax = cxG4::GetValue(aConf["solid"]["zmax"]);
  auto solidname = aConf["solid"]["name"].as<string>();
  auto *sphered = new G4Ellipsoid(solidname, rl, rl, rs, zmin, zmax);
  auto *maGlass_PMT = G4NistManager::Instance()->FindMaterial("maGlassPMT");
  auto *glass_logd = new G4LogicalVolume(sphered, maGlass_PMT, name);
  G4OpticalSurface *fSurface_PMTGlassSkin = nullptr;
  {
    fSurface_PMTGlassSkin = new G4OpticalSurface("PMTGlasssurface");  // NOLINT
    fSurface_PMTGlassSkin->SetType(dielectric_dielectric);
    fSurface_PMTGlassSkin->SetFinish(polished);
    fSurface_PMTGlassSkin->SetModel(unified);
  }
  new G4LogicalSkinSurface("PMTglass_Skin", glass_logd, fSurface_PMTGlassSkin);

  double Fe_rl = rl - dglass;
  double Fe_rs = rs - dglass;
  double Fe_zmin = std::max(zmin, -Fe_rs);
  double Fe_zmax = std::min(zmax, Fe_rs);
  auto *maFe_PMT = G4NistManager::Instance()->FindMaterial("maFePMT");
  auto *Fe_sphered = new G4Ellipsoid(solidname + "_Fe", Fe_rl, Fe_rl, Fe_rs, Fe_zmin, Fe_zmax);
  auto *Fe_logd = new G4LogicalVolume(Fe_sphered, maFe_PMT, name + "_Fe");
  G4OpticalSurface *fSurface_PMTFeSkin = nullptr;
  {
    vector<double>e = {2.0 * eV, 6.0 * eV};
    vector<double> p100 = {1., 1.};
    vector<double> p000 = {0., 0.};
    auto *fMPT_PMTphotocath = new G4MaterialPropertiesTable();  // NOLINT
    fMPT_PMTphotocath->AddProperty("REFLECTIVITY", e, p000);  // ref 0.0
    fMPT_PMTphotocath->AddProperty("EFFICIENCY", e, p100);   // Q.E. 1.0

    fSurface_PMTFeSkin = new G4OpticalSurface("PMTFesurface");  // NOLINT
    fSurface_PMTFeSkin->SetType(dielectric_metal);
    fSurface_PMTFeSkin->SetFinish(polished);
    fSurface_PMTFeSkin->SetModel(unified);
    fSurface_PMTFeSkin->SetMaterialPropertiesTable(fMPT_PMTphotocath);
  }
  new G4LogicalSkinSurface("PMTFe_Skin", Fe_logd, fSurface_PMTFeSkin);
  new G4PVPlacement(nullptr, G4ThreeVector(), Fe_logd, "PMT_Fe_physd", glass_logd, false, 0, aCheckForOverlaps);

  double air_rl = Fe_rl - dFe;
  double air_rs = Fe_rs - dFe;
  double air_zmin = std::max(zmin, -air_rs);
  double air_zmax = std::min(zmax, air_rs);
  auto *Air_sphered = new G4Ellipsoid(solidname + "_air", air_rl, air_rl, air_rs, air_zmin, air_zmax);
  auto *maAir_PMT = G4NistManager::Instance()->FindMaterial("maAirPMT");
  auto *Air_logd = new G4LogicalVolume(Air_sphered, maAir_PMT, name + "_Air");
  new G4PVPlacement(nullptr, G4ThreeVector(), Air_logd, "PMT_air_physd", Fe_logd, false, 0, aCheckForOverlaps);
  if (aConf["vis"]) {
    auto *aVis = createVisAttributes(aConf["vis"]);
    glass_logd->SetVisAttributes(aVis);
    Fe_logd->SetVisAttributes(aVis);
    Air_logd->SetVisAttributes(aVis);
  }
  if (theVerboseLevel != 0) {
    spdlog::debug("{} end --", __func__);
  }
  return glass_logd;
}
}  // namespace cxfunc::cxG4
