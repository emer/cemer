#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "template_def.h"
#include <taNBase> // TODO: replace with the temt/emergent file that contains
// your base class -- use <ClassName> to include any class by name
//TODO: add any headers of additional types that are used in your class

// The following is an example class -- replace with actual object name you want to use

TypeDef_Of(Template); // this declares the TA_Template type access object that
// will be generated automatically by maketa -- provides type info for gui and script
// interface to automatically interact with your new objects

class TEMPLATE_API Template : public taNBase {
  // TODO: describe your object
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
  void 			AddToAandB(int add_to_a, int add_to_b); 
  // #BUTTON adds amounts to a and b and signals a change
  void			MyCodeMethod(); // example of a method that will be used by your code, but not available to the user in a menu -- it will be availabe in the CSS scripting language


  TA_SIMPLE_BASEFUNS(Template);
  // defines a default set of constructors and copy code -- all the infrastructure 
protected:
  // use the 'override' pseudo-keyword to indicate overridden virtual methods
  override void		UpdateAfterEdit_impl(); // called when a class changes, just prior to gui updates and client class notifications
private:
  // these are the basic constructor and destructor code for this object -- just initialize
  // the members of this class -- usually destroy is not needed unless something special needs
  // to be freed (any taBase member will be dealt with automatically in the std BASEFUNS)
  void	Initialize();
  void	Destroy();
};

/* TemplatePluginState

  This class is used to hold user options and internal state. An instance
  is automatically created in the .root.plugin_state collections.
  Note: if you rename your plugin you MUST fix up this class name accordingly.
*/

TypeDef_Of(TemplatePluginState);

class TEMPLATE_API TemplatePluginState : public taFBase {
  // this class is used to hold and save/restore user options and internal state for the TemplatePlugin 
  INHERITED(taFBase)
public:
  static TemplatePluginState* instance(); 
  // use this accessor routine to obtain your instance
  
/* TODO: add user options or internal state here and delete this comment block
  -- examples of how to do some are below
   
   String	user_option1; // this text here will explain to the user what "user_option1" does
   int		user_status1; // #READ_ONLY #NO_SAVE this item will display for user as read-only, and won't get saved
   float	internal_state1; // #HIDDEN the user won't see this, but it will still be saved/loaded
*/
  
  SIMPLE_LINKS(TemplatePluginState)
  TA_BASEFUNS(TemplatePluginState)
protected:
  override void		UpdateAfterEdit_impl();
private:
  SIMPLE_COPY(TemplatePluginState)
  void	Initialize();
  void	Destroy();
};

#endif
