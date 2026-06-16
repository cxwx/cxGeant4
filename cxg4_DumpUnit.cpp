#include <G4UnitsTable.hh>
using namespace std;

auto main() -> int {
  G4UnitDefinition::BuildUnitsTable();
  G4UnitDefinition::PrintUnitsTable();
}
