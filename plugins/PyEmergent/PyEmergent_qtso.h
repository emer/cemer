#ifndef PYEMERGENT_QTSO_H
#define PYEMERGENT_QTSO_H

#include "PyEmergent_def.h"

// maketa cannot properly parse Qt so we only include those headers for
// the C++ compiler -- 
 
#ifdef __MAKETA__
// defines of most Qt classes are already in the qtdefs.h file, but
// any that aren't there you will have to define here 
#else 
//TODO: put your Inventor or Qt headers here, as below
# include <QObject>
#endif // __MAKETA__


#endif
