#if defined(__APPLE__) 

#include <QWidget>


#include <Cocoa/Cocoa.h>
#include <AppKit/Appkit.h>

// following is from:
// https://forum.qt.io/topic/60623/qt-5-4-2-os-x-10-11-el-capitan-how-to-remove-the-enter-full-screen-menu-item/5
// get rid of random automatic menu items!

void TemtMacDefaultSettings() {

  // Remove (disable) the "Start Dictation..." and "Emoji & Symbols" menu items
  // from the "Edit" menu

  [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledDictationMenuItem"];
  [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledCharacterPaletteMenuItem"];

#ifdef AVAILABLE_MAC_OS_X_VERSION_10_12_AND_LATER
  // Remove (don't allow) the "Show Tab Bar" menu item from the "View" menu, if
  // supported

  if ([NSWindow respondsToSelector:@selector(allowsAutomaticWindowTabbing)])
    NSWindow.allowsAutomaticWindowTabbing = NO;
#endif

  // Remove (don't have) the "Enter Full Screen" menu item from the "View" menu

  [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"NSFullScreenMenuItemEverywhere"];
}

#if (QT_VERSION == 0x050200)

// this is from  https://bugreports.qt-project.org/browse/QTBUG-28126
// doing the reverse of what they say -- turning the accept OFF
// here's my bug ticket: https://bugreports.qt-project.org/browse/QTBUG-38815

// this is now fixed in 5.4.0 so we don't need it anymore!!

#include <QGuiApplication>
#if (QT_VERSION == 0x050200)
#include <QtGui/5.2.0/QtGui/qpa/qplatformnativeinterface.h>
#endif
#if (QT_VERSION == 0x050201)
#include <QtGui/5.2.1/QtGui/qpa/qplatformnativeinterface.h>
#endif
#if (QT_VERSION == 0x050300)
#include <QtGui/5.3.0/QtGui/qpa/qplatformnativeinterface.h>
#endif
#if (QT_VERSION == 0x050401)
#include <QtGui/5.4.1/QtGui/qpa/qplatformnativeinterface.h>
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
