#ifndef TESTDETECTORCONSTRUCTION_CX__
#define TESTDETECTORCONSTRUCTION_CX__

#include <G4VUserDetectorConstruction.hh>

class testDetectorConstruction : public G4VUserDetectorConstruction {
 public:
  testDetectorConstruction() = default;
  ~testDetectorConstruction() override = default;
  auto Construct() -> G4VPhysicalVolume* override;
};

#endif
