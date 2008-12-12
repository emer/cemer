#include "hw.h"

void HelloBase::Initialize() {
  // any member initializations would go here
}

void HelloBase::Destroy() {
  CutLinks(); // unlinks any owned objects -- always put this in Destroy
  // any additional destruction operations would go here
}

void HelloBase::Hello() {
  taMisc::Error("Hello World!");
}

void HelloBase::AddCoords() {
  FloatTDCoord t = a + b; // demonstrates use of temporary objects
  c = t;
  UpdateAfterEdit(); // updates any associated members, and refreshes display
}
