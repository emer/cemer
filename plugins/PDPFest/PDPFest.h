#ifndef PDPFEST_H
#define PDPFEST_H

// already def'd in config.h
#undef QT_SHARED


#include "ta_plugin.h"
#include "ta_base.h"
#include "PDPFest_TA_type.h"

#ifndef __MAKETA__
#include "festival/festival.h"
#endif

//note: for Windows nonsense later...
#define PDPFEST_API

#ifndef __MAKETA__
class PDPFEST_API PDPFestPlugin : public QObject, 
                    public IPlugin
{ 
  Q_OBJECT

  Q_INTERFACES(IPlugin)

 public:
  PDPFestPlugin(QObject* par = NULL);

 public:
  int InitializeTypes() const;
  int InitializePlugin();
};

Q_DECLARE_INTERFACE(PDPFestPlugin, "pdp.PDPFestPlugin/1.0")
#endif // !__MAKETA__


class PDPFEST_API PDPFestBase: public taNBase {
public:
  void Hello(); // #MENU the wonderful test function

  TA_BASEFUNS(PDPFestBase)
private:
  void	Initialize() {}
  void	Destroy() {}
};


#endif
