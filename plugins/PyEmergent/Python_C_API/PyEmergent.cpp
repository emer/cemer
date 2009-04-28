/////////////////////////////////////////////////////
//  PyEmergent
/////////////////////////////////////////////////////

#include </usr/include/python2.5/Python.h>
#include "PyEmergent.h"
#include "ta_project.h"

void PyEmergent::Initialize() {
  Py_Initialize();
}

void PyEmergent::Destroy() {
  CutLinks();
  Py_Finalize();
}

void PyEmergent::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

Variant PyEmergent::CallFunction(String module_name, String function_name, String format = NULL) {
  PyObject *pModule, *pFunc, *pValue;

  // This is probably getting imported on every function call - bad
  pModule = PyImport_Import(PyString_FromString(module_name));

  if (pModule != NULL) {
    pFunc = PyObject_GetAttrString(pModule, function_name);

    if (pFunc && PyCallable_Check(pFunc)) {
      pValue = PyObject_CallFunction(pFunc, format);

      //if (pValue != NULL) {
      //Py_DECREF(pValue);
      //}
      if (pValue == NULL) {
	Py_DECREF(pFunc);
	Py_DECREF(pModule);
	PyErr_Print();
	taMisc::Info("Call failed");
	return 1;

      }
    }
    else {
      if (PyErr_Occurred())
	PyErr_Print();
      taMisc::Info("Cannot find function: " + function_name);
    }
    Py_XDECREF(pFunc);
    Py_DECREF(pModule);
  }
  else {
    PyErr_Print();
    taMisc::Info("Failed to load: " + module_name);
    return 1;
  }

  return pValue;
}


/////////////////////////////////////////////////////
//  PyemergentPluginState	
/////////////////////////////////////////////////////

PyemergentPluginState* PyemergentPluginState::instance() {
  if (tabMisc::root == NULL) return NULL;
  PyemergentPluginState* rval = (PyemergentPluginState*)
    tabMisc::root->plugin_state.FindType(StatTypeDef(0));
  return rval;
}

void PyemergentPluginState::Initialize() {
  SetBaseFlag(NAME_READONLY); // usually don't let user edit name
}

void PyemergentPluginState::Destroy() {
  CutLinks(); // unlinks any owned objects -- always put this in Destroy
  // TODO: release any manually allocated resources, if any
}

void PyemergentPluginState::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}
