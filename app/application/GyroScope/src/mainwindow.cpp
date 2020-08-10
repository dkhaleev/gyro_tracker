#include "mainwindow.h"
#include "ui_mainwindow.h"
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
  stateWasModified();

  QWidget *centralWidget = new QWidget(this);

//  console routine
  console = new Console;
  console->setEnabled(false);

  accelerometerGraph  = new Graph();
  gyroscopeGraph      = new Graph();
  magnetometerGraph   = new Graph();

//  tabs routine
  tabs = new QTabWidget(centralWidget);

  tabs->setMinimumSize(this->size());

//  tabs->addTab(new GeneralTab("simple text"), tr("General"));
  tabs->addTab(new GraphTab(*accelerometerGraph, *gyroscopeGraph, *magnetometerGraph), tr("Graph"));

  tabs->addTab(new ConsoleTab(*console), tr("Console"));
//  tabs->show();
  this->setCentralWidget(centralWidget);

//to not to interfere console width
  this->setMinimumWidth(1020);

// Timer for reading raw data (every 10ms)
  QTimer *timerArduino = new QTimer();
  timerArduino->connect(timerArduino, SIGNAL(timeout()),this, SLOT(onTimerReadData()));
  timerArduino->start(10);
//  connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
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
  connect(connectAct, SIGNAL(triggered()), this, SLOT(connectDevice()));

  disconnectAct = new QAction(QIcon(""), tr("&Disconnect"), this);
  connect(disconnectAct, SIGNAL(triggered()), this, SLOT(disconnectDevice()));


  QTextStream(stdout) << "PN " << portName << "\r\n";
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

      if(portLocation == info.systemLocation()){
          action->setChecked(true);
          portName = info.portName();
        }else{
      action->setChecked(false);
      }
      action->setData(info.systemLocation());

      portMenu->addAction(action);
      connect(portMenu, SIGNAL(triggered(QAction *)), this, SLOT(portMenuActionTriggered(QAction *)));
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

void MainWindow::resizeEvent(QResizeEvent *){
  tabs->setMinimumSize(centralWidget()->size());
  tabs->setMaximumSize(centralWidget()->size());
}

//Status Bar
void MainWindow::createStatusBar(){
  if(isConnected){
      statusBar()->showMessage("Ready");
    } else {
      statusBar()->showMessage("Disconnected");
    }
}

bool MainWindow::connectDevice(){
  QTextStream(stdout) << "Connect Action " << portLocation << "\r\n";

  if(mpu9250.openDevice(portLocation.toLocal8Bit().data(), 115200)!=1){
      std::cerr << "Error while opening serial device" << std::endl;
      isConnected = false;
      stateWasModified();
      return false;
    }
  QTextStream(stdout) << "Device Connected" << "\r\n";

  // Flush receiver of previously received data
  Sleep(100);
  mpu9250.flushReceiver();
  isConnected = true;
  stateWasModified();
  return true;
}

bool MainWindow::disconnectDevice(){
  QTextStream(stdout) << "Disconnect Action called " << portLocation << " \r\n";
  mpu9250.closeDevice();
  isConnected = false;
  stateWasModified();
  return false;
}

void MainWindow::portMenuActionTriggered(QAction * action){
  QVariant portSystemLocation = action->data();
  QSettings settings(settingsFileName, QSettings::IniFormat);
  settings.setValue("portLocation", portSystemLocation.toString());
  portLocation = portSystemLocation.toString();
  settings.sync();
  statusBar()->showMessage("Port selected");
  stateWasModified();
 }


//some sort of rations
//@TODO: research later
#define         RATIO_ACC       (4./32767.)
#define         RATIO_GYRO      ((1000./32767.)*(M_PI/180.))
//#define         RATIO_GYRO      (1000./32767.)
#define         RATIO_MAG       (48./32767.)

//timer event: get raw data from Arduino
void MainWindow::onTimerReadData(){
  if(isConnected){
      if(mpu9250.peekReceiver())
        {
          //Read block
          char buffer[200];
          mpu9250.readString(buffer, '\n', 200, 0);

          // Parse raw data
          long int counter = 0;
          unsigned long packet_id = 0;
          unsigned long core_time = 0;

          int16_t  iax, iay, iaz = 0;
          int16_t  igx, igy, igz = 0;
          int16_t  imx, imy, imz = 0;

          sscanf(buffer, "%lu %lu %lu %hu %hu %hu %hu %hu %hu %hu %hu %hu",
                 &counter,
                 &packet_id,
                 &core_time,
                 &iax,
                 &iay,
                 &iaz,
                 &igx,
                 &igy,
                 &igz,
                 &imx,
                 &imy,
                 &imz
                 );

//          // Display raw data
//          std::cout << counter << "\t";
//          std::cout << packet_id << "\t";
//          std::cout << core_time << "\t";
//          std::cout << iax ;

//          std::cout << iax << "\t" << iay << "\t" << iaz << "\t";
//          std::cout << igx << "\t" << igy << "\t" << igz << "\t";
//          std::cout << imx << "\t" << imy << "\t" << imz << "\t";
//          std::cout << "\r\n";
//          std::cout << std::endl;

//          assemly data for meta-console
          QByteArray data = buffer;

          console->putData(data);
          accelerometerGraph->dispatchData(core_time, iax, iay, iaz);
          gyroscopeGraph->dispatchData(core_time, igx, igy, igz);
          magnetometerGraph->dispatchData(core_time, imx, imy, imz);
        }
    }else{
//      QTextStream(stdout)
//          << "Not Connected"
//          << "\r\n"
//             ;
    }
}

void MainWindow::stateWasModified(){
  QTextStream(stdout)
      << "State was modified "
      << "\r\n"
      ;

  if(isConnected){
      connectAct->setDisabled(true);
      disconnectAct->setDisabled(false);
      statusBar()->showMessage("connected");
    }else{
      if(portLocation == ""){
          connectAct->setDisabled(true);
        } else {
          connectAct->setDisabled(false);
        }
      statusBar()->showMessage("disconnected");
      disconnectAct->setDisabled(true);
    }
}

MainWindow::~MainWindow()
{
  //  delete ui;
}
