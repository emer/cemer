// Hack file for maketa because it chokes on Qt header files

#ifndef QTMAKETA_H
#define QTMAKETA_H

#ifdef __MAKETA__

#include "ta_maketa_defs.h"

#ifdef TA_USE_INVENTOR
class SoBase {// ##NO_INSTANCE ##NO_TOKENS
public:
	int dummy; // #IGNORE
	virtual ~SoBase() {}
};

class SoFieldContainer: public SoBase {
public:
	int dummy; // #IGNORE
};

class SoNode: public SoFieldContainer {
public:
	int dummy; // #IGNORE
};

class SoGroup: public SoNode {
public:
	int dummy; // #IGNORE
};

class SoSeparator: public SoGroup {
public:
	int dummy; // #IGNORE
};
#endif

#endif // __MAKETA__
#endif // QTMAKETA_H
