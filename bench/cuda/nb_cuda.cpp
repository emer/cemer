#include "nb_util.h"
#include "nb_netstru_cuda.h"

#include <QtCore/QString>
#include <QtCore/QCoreApplication>

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  NbCuda nb(argc, argv);
  int rval = nb.PreInitialize();
  if (rval != 0) return rval;
  return nb.main();
}
