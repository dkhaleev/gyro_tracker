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
  stateWasModified();

  // Timer for reading raw data (every 10ms)
  QTimer *timerArduino = new QTimer();
  timerArduino->connect(timerArduino, SIGNAL(timeout()),this, SLOT(onTimerReadData()));
  timerArduino->start(10);
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

  if(mpu9250.openDevice(portLocation.toLocal8Bit().data(), 9600)!=1){
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
          mpu9250.readString(buffer, '\n', 200, 10);


//          struct Payload {
//            long int counter = 0;         //system counter
//            unsigned long packet_id = 0;  //packet ID
//            unsigned long core_time = 0;  //sensor unit core time
//            int16_t ax = 0;               //acceleration by X axis
//            int16_t ay = 0;               //acceleration by Y axis
//            int16_t az = 0;               //acceleration by Z axis
//            int16_t gx = 0;               //orientation by X axis
//            int16_t gy = 0;               //orientation by Y axis
//            int16_t gz = 0;               //orientation by Z axis
//            int16_t mx = 0;               //magnetic inclination by X axis
//            int16_t my = 0;               //magnetic inclination by Y axis
//            int16_t mz = 0;               //magnetic inclination by Z axis
//          };

          // Parse raw data
          long int counter;
          unsigned long packet_id;
          unsigned long core_time;

          int16_t  iax, iay;

          sscanf(buffer, "%lu %lu %lu %hu %hu",
                 &counter,
                 &packet_id,
                 &core_time,
                 &iax,
                 &iay);
////          int  it;

//          ,iay,iaz;
//          int16_t  igx,igy,igz;
//          int16_t  imx,imy,imz;
//          sscanf (buffer,"%lu %lu %lu %hu %hu %hu %hu %hu %hu %hu %hu %hu",
//                  &counter,
//                  &packet_id,
//                  &core_time,
//                  &iax,
//                  &iay,
//                  &iaz,
//                  &igx,
//                  &igy,
//                  &igz,
//                  &imx,
//                  &imy,
//                  &imz);

//          sscanf(buffer, "%lu %lu", &counter, &packet_id);
//          // Display raw data
          std::cout << counter << "\t";
          std::cout << packet_id << "\t";
          std::cout << core_time << "\t";
          std::cout << iax << "\t";
          std::cout << iay << "\t";
//          std::cout << iaz << "\t";
//          std::cout << iax << "\t" << iay << "\t" << iaz << "\t";
//          std::cout << igx << "\t" << igy << "\t" << igz << "\t";
//          std::cout << imx << "\t" << imy << "\t" << imz << "\t";
//          std::cout << "\r\n";
          std::cout << std::endl;

        }
    }else{
      QTextStream(stdout)
          << "Not Connected"
          << "\r\n"
             ;
    }
}

void MainWindow::stateWasModified(){
  QTextStream(stdout)
      << "State was modified "
      << "\r\n"
//      << "Port location "
//      << portLocation
//      << "\r\n"
//      << "Port Name"
//      << portName
      ;

  if(isConnected){
      connectAct->setDisabled(true);
      disconnectAct->setDisabled(false);
    }else{
      if(portLocation == ""){
          connectAct->setDisabled(true);
        }
      disconnectAct->setDisabled(true);
    }
}

MainWindow::~MainWindow()
{
  //  delete ui;
}
