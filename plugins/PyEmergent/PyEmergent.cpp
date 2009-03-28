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

Variant PyEmergent::CallPythonFunNoArgs(String module_name, String function_name) {
  PyObject *pName, *pModule, *pFunc;
  PyObject *pValue;

  pName = PyString_FromString(module_name);
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule != NULL) {
    pFunc = PyObject_GetAttrString(pModule, function_name);
    /* pFunc is a new reference */

    if (pFunc && PyCallable_Check(pFunc)) {
      pValue = PyObject_CallObject(pFunc, NULL);
      if (pValue != NULL) {
	printf("%ld\n", PyInt_AsLong(pValue));
	Py_DECREF(pValue);
      }
      else {
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

  return 0;
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
