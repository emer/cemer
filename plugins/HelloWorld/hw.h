#ifndef HW_H
#define HW_H

#include "hw_def.h"
#include "ta_base.h"
#include "minmax.h"
#include "ta_geometry.h"

// The actual content of the plugin follows. Inheriting from taNBase
// (where the N stands for Named) interfaces you with the TypeAccess
// system

class HW_API HelloBase : public taNBase
{
  INHERITED(taNBase) // declares 'inherited' keyword for safer base-class references

public:
  FixedMinMax		min_max; // example of complex types as fields
  FloatTDCoord		a; // 1st input parameter to AddCoords
  FloatTDCoord		b; // 1st input parameter to AddCoords
  FloatTDCoord		c; // 1st input parameter to AddCoords
  
  // Comment directives such as the one below are explained in section
  // 18.3.3 of the PDP++ User Guide. This particular directive creates
  // a menu for the function Hello() in an edit dialog
  void Hello(); // #MENU Hello, World! Function
  void AddCoords(); // #MENU adds a + b and puts result in c


  SIMPLE_LINKS(HelloBase) // automatically links embedded taBase objects into ownership chain
  TA_BASEFUNS(HelloBase) /* Defines a default constructor that calls
    Initialize(), Register() and SetDefaultName() in that order.*/

private:
  SIMPLE_COPY(HelloBase) // enables this object to be copied
  void	Initialize();
  void	Destroy();
};

#endif
