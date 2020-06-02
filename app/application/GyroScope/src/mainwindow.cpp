#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include <QtWidgets>
#include <QWidgetAction>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <QTextStream>

#include <iostream>

//Main window constructor
//Main menu
MainWindow::MainWindow(QWidget *parent, int w, int h)
  : QMainWindow(parent)
{
  this->resize(w,h);
  this->setWindowTitle("GyroScope");
  this->setContextMenuPolicy(Qt::NoContextMenu);

  createActions();
  createMenus();
  createToolBars();

}

void MainWindow::createActions(){
  connectAct = new QAction(QIcon(""), tr("&Connect"), this);

  disconnectAct = new QAction(QIcon(""), tr("&Disconnect"), this);

}

//  create menu bar
void MainWindow::createMenus(){
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addSeparator();
  fileMenu->addAction("Quit", qApp, SLOT (quit()), QKeySequence(tr("Ctrl+Q")));
  fileMenu->addSeparator();

  //  View
  viewMenu = menuBar()->addMenu(tr("&View"));

  //  Params
  paramsMenu = menuBar()->addMenu(tr("&Parameters"));

  //port settings
  QList<QSerialPortInfo> list;
  list = QSerialPortInfo::availablePorts();

  portMenu = paramsMenu->addMenu(tr("&Port"));

  if(list.size() == 0){
      portMenu->setDisabled(true);
    }

  foreach(QSerialPortInfo info, list){
      portMenu->addMenu(info.portName());
      QTextStream(stdout) << info.portName()
                          << "\r\n"
                          << info.description()
                          << "\r\n"
                          << info.manufacturer()
                          << "\r\n"
                          << info.systemLocation()
                          << "\r\n"
                          << info.vendorIdentifier()
                          << "\r\n"
                          <<info.productIdentifier()
                          << "------";
    }


  //baudrate settings
  baudrateMenu = paramsMenu->addMenu(tr("&BaudRate"));

  //  About
  aboutMenu = menuBar()->addMenu(tr("&?"));
}

// Main ToolBar
void MainWindow::createToolBars(){
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->setMovable(false);
  fileToolBar->setContextMenuPolicy(Qt::NoContextMenu);
  fileToolBar->addAction(connectAct);
  fileToolBar->addAction(disconnectAct);
}

MainWindow::~MainWindow()
{
//  delete ui;
}

