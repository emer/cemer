#ifndef HW_H
#define HW_H

#ifndef __MAKETA__
# include <QObject>
#endif

#include "ta_plugin.h"

class HelloPlugin : public QObject, 
                    public IPlugin

{ // ##NO_INSTANCE
  Q_OBJECT

#ifndef __MAKETA__
  Q_INTERFACES(IPlugin)
#endif

 public:
  HelloPlugin(QObject* par = NULL);
  void Hello();

 public:
  int InitializeTypes() const;
  int InitializePlugin();
};

#ifndef __MAKETA__
Q_DECLARE_INTERFACE(HelloPlugin, "pdp.HelloPlugin/1.0")
#endif

#endif
