#include "nb_util.h"
#include "nb_netstru.h"

#include <QtCore/QString>
#include <QtCore/QCoreApplication>

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  Nb nb(argc, argv);
  nb.Initialize();
  return nb.main();
}
