#ifndef CXPHYSICSLIST_CX___
#define CXPHYSICSLIST_CX___

#include <G4VModularPhysicsList.hh>

namespace cxfunc::cxG4 {
class testPhysicsList : public G4VModularPhysicsList {
 public:
  testPhysicsList();
  ~testPhysicsList() override = default;

  void SetCuts() override;
};
}  // namespace cxfunc::cxGeant4

#endif
