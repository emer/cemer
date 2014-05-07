#if defined(__APPLE__) 

#include <QWidget>

#if (QT_VERSION >= 0x050200)

// this is from  https://bugreports.qt-project.org/browse/QTBUG-28126
// doing the reverse of what they say -- turning the accept OFF
// here's my bug ticket: https://bugreports.qt-project.org/browse/QTBUG-38815

#include <Cocoa/Cocoa.h>
#include <QGuiApplication>
#if (QT_VERSION == 0x050200)
#include <QtGui/5.2.0/QtGui/qpa/qplatformnativeinterface.h>
#endif
#if (QT_VERSION == 0x050201)
#include <QtGui/5.2.1/QtGui/qpa/qplatformnativeinterface.h>
#endif

void TurnOffTouchEventsForWindow(QWindow* qtWindow) {
  if(!qtWindow) return;
  NSView *qtView = (NSView *)QGuiApplication::platformNativeInterface()->
      nativeResourceForWindow("nsview", qtWindow);
  [qtView setAcceptsTouchEvents:NO];
//   [qtView setWantsRestingTouches:NO];
}

#endif

#endif
