#include "template.h"

// following two needed for PluginState
#include <tabMisc>
#include <taRootBase>

#include <taMisc> // has many useful utility functions

/////////////////////////////////////////////////////
//  Template
/////////////////////////////////////////////////////

// this is needed to define functions declared by TA_BASEFUNS in the header:
TA_BASEFUNS_CTORS_DEFN(Template);

void Template::Initialize() {
// TODO: delete following and add your own member initializations
  a = 2;
  b = 4;
  sum_a_b = a + b;
}

void Template::Destroy() {
  CutLinks(); // unlinks any owned objects -- always put this in Destroy
  // TODO: release any manually allocated resources, if any
}

void Template::UpdateAfterEdit_impl() {
  // by using the 'inherited' pseudo-keyword we always call the right base class
  inherited::UpdateAfterEdit_impl();
//TODO: delete following lines, add any update code you may have, or leave empty
  // we always keep sum_a_b up to date
  sum_a_b = a + b;
}


//TODO: replace the methods below with your own methods
void Template::Hello() {
  taMisc::Info("Hello from Template!");
}

void Template::AddToAandB(int add_to_a, int add_to_b) {
  a += add_to_a;
  b += add_to_b;
  UpdateAfterEdit(); // updates any associated members, refreshes display, and notifies clients of this class
}

void Template::MyCodeMethod() {
   // example, does nothing
}


/////////////////////////////////////////////////////
//  TemplatePluginState	
/////////////////////////////////////////////////////

TemplatePluginState* TemplatePluginState::instance() {
  if (tabMisc::root == NULL) return NULL;
  TemplatePluginState* rval = (TemplatePluginState*)
    tabMisc::root->plugin_state.FindType(StatTypeDef(0));
  return rval;
}

void TemplatePluginState::Initialize() {
  SetBaseFlag(NAME_READONLY); // usually don't let user edit name
// TODO: add your member initializations
}

void TemplatePluginState::Destroy() {
  CutLinks(); // unlinks any owned objects -- always put this in Destroy
  // TODO: release any manually allocated resources, if any
}

void TemplatePluginState::UpdateAfterEdit_impl() {
  // by using the 'inherited' pseudo-keyword we always call the right base class
  inherited::UpdateAfterEdit_impl();
//TODO: add any update code you may have, or leave empty
}

