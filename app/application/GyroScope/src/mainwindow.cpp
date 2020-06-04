#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include <QtWidgets>
#include <QWidgetAction>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <QTextStream>
#include <QSettings>
#include <QDebug>

#include <iostream>

//Main window constructor

//Main menu
MainWindow::MainWindow(QWidget *parent, int w, int h)
  : QMainWindow(parent)
{
  //settings file handler
  settingsFileName = QApplication::applicationDirPath() + "/config.ini";

  loadSettings();

  this->resize(w,h);
  this->setWindowTitle("GyroScope");
  this->setContextMenuPolicy(Qt::NoContextMenu);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

}

void MainWindow::loadSettings()
{
  QSettings settings(settingsFileName, QSettings::IniFormat);
  portName      = settings.value("portName", "").toString();
  portLocation  = settings.value("portLocation", "").toString();
}

void MainWindow::saveSettings(){
  QSettings settings(settingsFileName, QSettings::IniFormat);
  settings.value("portName", portName);
  settings.value("portLocation", portLocation);
}

void MainWindow::createActions(){
  connectAct = new QAction(QIcon(""), tr("&Connect"), this);

  disconnectAct = new QAction(QIcon(""), tr("&Disconnect"), this);

  QTextStream(stdout) << "PN " << portName << "\r\n";

  if(isConnected){
      connectAct->setDisabled(true);
    }else{
      if(portName == ""){
          connectAct->setDisabled(true);
        }
      disconnectAct->setDisabled(true);
    }
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
      QAction * action = new QAction(info.portName(), this);
      action->setCheckable(true);
      action->setChecked(false);

      action->setData(info.systemLocation());

      portMenu->addAction(action);
      connect(portMenu, SIGNAL(triggered(QAction *)), this, SLOT(portMenuActionTriggered(QAction *)));

      ////      dynamic creation of ports list
      //      portMenu->addMenu(info.portName());
      //      QTextStream(stdout) << info.portName()
      //                          << "\r\n"
      //                          << info.description()
      //                          << "\r\n"
      //                          << info.manufacturer()
      //                          << "\r\n"
      //                          << info.systemLocation()
      //                          << "\r\n"
      //                          << info.vendorIdentifier()
      //                          << "\r\n"
      //                          << info.productIdentifier()
      //                          << "\r\n"
      //                          << "------"
      //                          << "\r\n";
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

//Status Bar

void MainWindow::createStatusBar(){
  if(isConnected){
      statusBar()->showMessage("Ready");
    } else {
      statusBar()->showMessage("Disconnected");
    }

}

void MainWindow::portMenuActionTriggered(QAction * action){
  QVariant portLocation = action->data();
  QSettings settings(settingsFileName, QSettings::IniFormat);
  settings.setValue("portLocation", portLocation.toString());
  settings.sync();

  connectAct->setEnabled(true);
  statusBar()->showMessage("Port selected");
 }

MainWindow::~MainWindow()
{
  //  delete ui;
}

