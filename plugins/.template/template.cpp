#include "template.h"

void TemplatePluginExampleClass::Initialize() {
  // TODO: delete following and add your own member initializations
  a = 2;
  b = 4;
  sum_a_b = a + b;
}

void TemplatePluginExampleClass::Destroy() {
  CutLinks(); // unlinks any owned objects -- always put this in Destroy
  // TODO: release any manually allocated resources, if any
}

void TemplatePluginExampleClass::UpdateAfterEdit_impl() {
  // by using the 'inherited' pseudo-keyword we always call the right base class
  inherited::UpdateAfterEdit_impl();
//TODO: delete following lines, add any update code you may have, or leave empty
  // we always keep sum_a_b up to date
  sum_a_b = a + b;
}


//TODO: replace the methods below with your own methods
void TemplatePluginExampleClass::Hello() {
  taMisc::Info("Hello from TemplatePluginExampleClass!");
}

void TemplatePluginExampleClass::AddToAandB(int add_to_a, int_add_to_b) {
  a += add_to_a;
  b += add_to_b;
  UpdateAfterEdit(); // updates any associated members, refreshes display, and notifies clients of this class
}

void TemplatePluginExampleClass::MyCodeMethod() {
   // example, does nothing
}
