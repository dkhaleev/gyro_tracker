#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w(0, 1000, 400);
  w.show();
  return a.exec();
}
