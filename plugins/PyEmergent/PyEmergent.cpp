#include </usr/include/python2.5/Python.h>
#include "PyEmergent.h"

#include "ta_project.h"

/////////////////////////////////////////////////////
//  PyemergentPluginExampleClass
/////////////////////////////////////////////////////

void PyemergentPluginExampleClass::Initialize() {
// TODO: delete following and add your own member initializations
  module_name = "time";
  function_name = "time";
}

void PyemergentPluginExampleClass::Destroy() {
  CutLinks(); // unlinks any owned objects -- always put this in Destroy
  // TODO: release any manually allocated resources, if any
}

void PyemergentPluginExampleClass::UpdateAfterEdit_impl() {
  // by using the 'inherited' pseudo-keyword we always call the right base class
  inherited::UpdateAfterEdit_impl();
//TODO: delete following lines, add any update code you may have, or leave empty
  // we always keep sum_a_b up to date
  //  sum_a_b = a + b;
}


//TODO: replace the methods below with your own methods
void PyemergentPluginExampleClass::Hello() {
  //taMisc::Info("Hello from PyemergentPluginExampleClass!");
  PyObject *pName, *pModule, *pFunc;
  PyObject *pArgs, *pValue;
  Py_Initialize();
  pName = PyString_FromString(module_name);
    /* Error checking of pName left out */
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule != NULL) {
    pFunc = PyObject_GetAttrString(pModule, function_name);
    /* pFunc is a new reference */

    if (pFunc && PyCallable_Check(pFunc)) {
      pArgs = PyTuple_New(argc - 3);
      for (i = 0; i < argc - 3; ++i) {
	pValue = PyInt_FromLong(atoi(argv[i + 3]));
	if (!pValue) {
	  Py_DECREF(pArgs);
	  Py_DECREF(pModule);
	  taMisc::Info("Cannot convert argument");
	  return;
	}
	/* pValue reference stolen here: */
	PyTuple_SetItem(pArgs, i, pValue);
      }
      pValue = PyObject_CallObject(pFunc, pArgs);
      Py_DECREF(pArgs);
      if (pValue != NULL) {
	printf("%ld\n", PyInt_AsLong(pValue));
	Py_DECREF(pValue);
      }
      else {
	Py_DECREF(pFunc);
	Py_DECREF(pModule);
	PyErr_Print();
	taMisc::Info("Call failed\n");
	return;

      }
    }
    else {
      if (PyErr_Occurred())
	PyErr_Print();
      taMisc::Info("Cannot find function" + function_name);
    }
    Py_XDECREF(pFunc);
    Py_DECREF(pModule);
  }
  else {
    PyErr_Print();
    taMisc::Info("Failed to load" + module_name);
    return;
  }
  Py_Finalize();
  return;
}

void PyemergentPluginExampleClass::AddToAandB(int add_to_a, int add_to_b) {
//   a += add_to_a;
//   b += add_to_b;
  UpdateAfterEdit(); // updates any associated members, refreshes display, and notifies clients of this class
}

void PyemergentPluginExampleClass::MyCodeMethod() {
   // example, does nothing
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
// TODO: add your member initializations
}

void PyemergentPluginState::Destroy() {
  CutLinks(); // unlinks any owned objects -- always put this in Destroy
  // TODO: release any manually allocated resources, if any
}

void PyemergentPluginState::UpdateAfterEdit_impl() {
  // by using the 'inherited' pseudo-keyword we always call the right base class
  inherited::UpdateAfterEdit_impl();
//TODO: add any update code you may have, or leave empty
}

