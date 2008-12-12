#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "template_def.h"
#include "ta_base.h" // TODO: replace with the temt/emergent file that contains your base class
//TODO: add any headers of additional types that are used in your class, such as ta_geometry.h 


class TEMPLATE_API TemplatePluginExampleClass : public taNBase
{
  INHERITED(taNBase) // declares 'inherited' keyword for safer base-class references

public:
  String		example_member1; // TODO: replace these and add your own members
  Variant		example_member2; // a variant
  int			a; // a first number
  int			b; // a second number
  int			sum_a_b; // #READ_ONLY #NO_SAVE a third number, only accessible by the program
  
  // TODO: delete the following text and methods, and start adding your methods here
  // Comment directives such as the ones below are explained here:
  // http://grey.colorado.edu/emergent/index.php/Comment_directives
  // This particular directive creates a menu for the function Hello() in an edit dialog
  void 			Hello(); // #MENU Hello Function!
  // The next method declares a button in the dialog
  void 			AddToAandB(int add_to_a, int_add_to_b); 
  // #BUTTON adds amounts to a and b and signals a change
  void			MyCodeMethod(); // example of a method that will be used by your code, but not available to the user in a menu -- it will be availabe in the CSS scripting language


  SIMPLE_LINKS(TemplatePluginExampleClass) // automatically links embedded taBase objects into ownership chain
  TA_BASEFUNS(TemplatePluginExampleClass) // Defines a default set of constructors and copy code 

protected:
  // use the 'override' pseudo-keyword to indicate overridden virtual methods
  override void		UpdateAfterEdit_impl(); // called when a class changes, just prior to gui updates and client class notifications
private:
  SIMPLE_COPY(TemplatePluginExampleClass) // enables this object to be copied
  void	Initialize();
  void	Destroy();
};

#endif
