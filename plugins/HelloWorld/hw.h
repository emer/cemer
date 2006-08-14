#ifndef HW_H
#define HW_H

// already def'd in config.h
#undef QT_SHARED
#include "ta_plugin.h"
#include "ta_base.h"
#include "hw_TA_type.h"

//note: for Windows nonsense later...
#define HW_API

#ifndef __MAKETA__
class HW_API HelloPlugin : public QObject, 
                    public IPlugin
{ 
  Q_OBJECT

  Q_INTERFACES(IPlugin)

 public:
  HelloPlugin(QObject* par = NULL);

 public:
  int InitializeTypes() const;
  int InitializePlugin();
};

Q_DECLARE_INTERFACE(HelloPlugin, "pdp.HelloPlugin/1.0")
#endif // !__MAKETA__


class HW_API HelloBase: public taNBase {
public:
  void Hello(); // #MENU the wonderful test function

  TA_BASEFUNS(HelloBase)
private:
  void	Initialize() {}
  void	Destroy() {}
};


#endif
