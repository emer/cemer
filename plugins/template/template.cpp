#include "template.h"

#include "ta_project.h"

/////////////////////////////////////////////////////
//  TemplateClass
/////////////////////////////////////////////////////

void TemplateClass::Initialize() {
// TODO: delete following and add your own member initializations
  a = 2;
  b = 4;
  sum_a_b = a + b;
}

void TemplateClass::Destroy() {
  CutLinks(); // unlinks any owned objects -- always put this in Destroy
  // TODO: release any manually allocated resources, if any
}

void TemplateClass::UpdateAfterEdit_impl() {
  // by using the 'inherited' pseudo-keyword we always call the right base class
  inherited::UpdateAfterEdit_impl();
//TODO: delete following lines, add any update code you may have, or leave empty
  // we always keep sum_a_b up to date
  sum_a_b = a + b;
}


//TODO: replace the methods below with your own methods
void TemplateClass::Hello() {
  taMisc::Info("Hello from TemplateClass!");
}

void TemplateClass::AddToAandB(int add_to_a, int add_to_b) {
  a += add_to_a;
  b += add_to_b;
  UpdateAfterEdit(); // updates any associated members, refreshes display, and notifies clients of this class
}

void TemplateClass::MyCodeMethod() {
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

