#include <QFileDialog>
#include <QApplication>
#include <QFile>

int
main(int argc, char ** argv)
{
  QApplication app(argc, argv);

  QFileDialog* fd = new QFileDialog(NULL, "fd bug", "./");

  fd->setOptions(QFileDialog::DontUseNativeDialog);

  int rval = fd->exec();
 
  QByteArray fdsvset = fd->saveState();

  QFile qf("/Users/oreilly/emergent/qt5_fd_bug/fd_bug_out");
  qf.open(QIODevice::WriteOnly);
  qf.write(fdsvset);
  qf.close();

  delete fd;
  return 0;
}
