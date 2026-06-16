# cxg4

A small personal toolkit that makes [Geant4](https://geant4.web.cern.ch/)
simulation setup less verbose: YAML-driven construction of materials,
solids, logical volumes, and PMTs, plus a handful of inspection utilities.

- Language: C++23
- Namespace: `cxfunc::cxG4`
- Version: see [`VERSION.md`](./VERSION.md)

## Features

- `cxg4` shared library (`libcxg4`) exposing helpers under `cxfunc::cxG4`:
  - `init(YAML::Node)` — configure the run manager, geometry tolerance,
    `SDManager`, `TScoreNtupleWriter`, etc. from a YAML config.
  - `GetValue(std::string)` — parse Geant4 dimensioned strings like
    `"1.0 m"` or `"2.5 eV"` into `double`.
  - `ConstructMaterial / ConstructSolid / ConstructLogicalVolume` — build
    Geant4 objects from YAML descriptors. Solids supported today:
    `G4Box`, `G4Ellipsoid`, and `cxG4Ellipsoid_rl_rs`.
  - `ConstructPMTLV` — assemble a PMT logical volume (glass / Fe / air
    shells) with the corresponding optical skin surfaces.
  - `createVisAttributes`, `GetBoxXYZ`, `GetMaG4Nist`, `DumpParticle`,
    `DumpPhysics`.
- Four standalone CLI tools (also installed alongside the library):
  - `cxg4_materialprint [G4_NAME]` — print a NIST material (no arg lists all).
  - `cxg4_DumpUnit` — print the Geant4 units table.
  - `cxg4_DumpParticle <name|PDG>` — dump a particle definition.
  - `cxg4_DumpPhysics` — dump the registered physics list and cut table.

## Requirements

- CMake ≥ 3.20
- A C++23 compiler
- Geant4 ≥ 11.2 (with `ui_all vis_all` when visualization is enabled)
- [spdlog](https://github.com/gabime/spdlog)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Options

| Option        | Default | Description                              |
| ------------- | ------- | ---------------------------------------- |
| `CXG4PLOT`    | `ON`    | Pull in Geant4 UI + visualization modules. |

### Install

By default the install prefix is `$HOME/software/install/`. Override with
`-DCMAKE_INSTALL_PREFIX=<dir>` if needed.

```sh
cmake --install build
```

The public header `cxGeant4.hh` is installed under
`include/cxfunc/`; the library and CLI tools under `bin/` and `lib/`.

## Using the library

Link `cxg4` and include the header:

```cpp
#include <cxfunc/cxGeant4.hh>  // adjust include path to your install layout

int main() {
  YAML::Node conf = YAML::LoadFile("config.yaml");
  cxfunc::cxG4::init(conf);

  YAML::Node matNode;
  matNode["buildtype"] = "FindOrBuildMaterial";
  matNode["G4name"]    = "G4_WATER";
  G4Material* water = cxfunc::cxG4::ConstructMaterial(matNode);
  // ...
}
```

### Example YAML fragment

```yaml
solid:
  name: myBox
  type: G4Box          # or G4Ellipsoid, cxG4Ellipsoid_rl_rs
  x: "10 cm"
  y: "10 cm"
  z: "20 cm"

pmt:
  name: PMT_front
  solid:
    type: R7081        # key into the PMT database passed to ConstructPMTLV
    name: PMT_front_solid
    zmin: "-10 cm"
    zmax: "10 cm"
```

## Repository layout

```
cxGeant4.cc/.hh        library implementation and public header
test*.cc/.hh           minimal DetectorConstruction / PhysicsList used by the tools
cxg4_*.cpp             the four CLI entry points
VERSION.md             MAJ / MIN / PAT read by CMake
CMakeLists.txt         build + install configuration
```

## License

MIT. See [`LICENSE`](./LICENSE).

Geant4 itself is distributed under the Apache License 2.0; this project
depends on it but does not redistribute it.
