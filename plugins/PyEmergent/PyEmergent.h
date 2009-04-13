#ifndef PYEMERGENT_H
#define PYEMERGENT_H

#include "PyEmergent_def.h"
#include "ta_base.h"

class PYEMERGENT_API PyEmergent : public taNBase
{
  INHERITED(taNBase)
public:
  String module_name;

  String function_name;
  Variant 			CallFunction(String module_name, String function_name, String format); // #MENU Hello Function!

  SIMPLE_LINKS(PyEmergent)
  TA_BASEFUNS(PyEmergent)

protected:
  override void		UpdateAfterEdit_impl(); // called when a class changes, just prior to gui updates and client class notifications
private:
  SIMPLE_COPY(PyEmergent) // enables this object to be copied
  void	Initialize();
  void	Destroy();
};

class PYEMERGENT_API PyemergentPluginState : public taFBase
{
  INHERITED(taFBase)
public:
  static PyemergentPluginState* instance(); 
  SIMPLE_LINKS(PyemergentPluginState)
  TA_BASEFUNS(PyemergentPluginState)
protected:
  override void		UpdateAfterEdit_impl();
private:
  SIMPLE_COPY(PyemergentPluginState)
  void	Initialize();
  void	Destroy();
};
#endif
