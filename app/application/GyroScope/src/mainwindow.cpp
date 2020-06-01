#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include <QWidgetAction>

//Main window constructor
//Main menu
MainWindow::MainWindow(QWidget *parent, int w, int h)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  this->resize(w,h);
  this->setWindowTitle("GyroScope");

//  create menu bar
  QMenu *FileMenu = menuBar()->addMenu("&File");
  FileMenu->addSeparator();
  FileMenu->addAction("Quit", qApp, SLOT (quit()), QKeySequence(tr("Ctrl+Q")));

//  View
  QMenu *ViewMenu = menuBar()->addMenu("&View");

//  Params
  menuBar()->addSeparator();
  QMenu *ParamsMenu = menuBar()->addMenu("&Parameters");

//port settings
  PortMenu = ParamsMenu->addMenu("&Port");
//baudrate settings
  BaudrateMenu = ParamsMenu->addMenu("&BaudRate");


//  About
  menuBar()->addSeparator();
  QMenu *AboutMenu = menuBar()->addMenu("&?");


}

MainWindow::~MainWindow()
{
  delete ui;
}

