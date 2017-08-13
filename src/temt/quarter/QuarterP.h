#ifndef QUARTER_QUARTERP_H
#define QUARTER_QUARTERP_H
#include <Inventor/SbName.h>
#include <QCursor>

/* define this to the libQuarter major version number */
#define QUARTER_MAJOR_VERSION 1

/* define this to the libQuarter release version number */
#define QUARTER_MICRO_VERSION 1

/* define this to the libQuarter minor version number */
#define QUARTER_MINOR_VERSION 0

/* define this to the full libQuarter major.minor.micro version number */
#define QUARTER_VERSION "QUARTER_VERSION"

template <class Key, class T> class QMap;

namespace SIM { namespace Coin3D { namespace Quarter {

class QuarterP {
 public:
  QuarterP();
  ~QuarterP();

  class SensorManager * sensormanager;
  class ImageReader * imagereader;

  typedef QMap<SbName, QCursor> StateCursorMap;
  static StateCursorMap * statecursormap;

  bool initCoin;
};

}}};

#define COIN_CT_ASSERT(expr)                                            \
  do { switch ( 0 ) { case 0: case (expr): break; } } while ( 0 )

#define COMPILE_ONLY_BEFORE(MAJOR,MINOR,MICRO,REASON) \
  COIN_CT_ASSERT( (QUARTER_MAJOR_VERSION < MAJOR) || (QUARTER_MAJOR_VERSION == MAJOR && ((QUARTER_MINOR_VERSION < MINOR) || ( QUARTER_MINOR_VERSION == MINOR && (QUARTER_MICRO_VERSION < MICRO )))))

#endif //QUARTER_QUARTERP_H
