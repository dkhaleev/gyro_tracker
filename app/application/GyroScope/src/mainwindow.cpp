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

  QWidget *centralWidget = new QWidget(this);

  //  tabs routine
  tabs = new QTabWidget(centralWidget);

  tabs->setMinimumSize(this->size());

//  console routine  
  console = new Console;
  consoleTab = new ConsoleTab(*console);

  QString type;

  type = "accel";
  accelerometerGraph  = new Graph(parent, type);

  type = "gyro";
  gyroscopeGraph      = new Graph(parent, type);

  type = "mag";
  magnetometerGraph   = new Graph(parent, type);
  graphTab = new GraphTab(*accelerometerGraph, *gyroscopeGraph, *magnetometerGraph);

  objectGL = new ObjectOpenGL();
  objectGL->setObjectName(QString::fromUtf8("ObjectOpenGL"));
  generalTab = new GeneralTab(objectGL);

  if(generalTabView){
    tabs->insertTab(0, generalTab, tr("General"));
    }

  if(graphTabView){
      tabs->insertTab(1, graphTab, tr("Graph"));
    }

  if(consoleTabView){
      tabs->insertTab(2, consoleTab, tr("Console"));
    }
  
  this->setCentralWidget(centralWidget);

//to not to interfere console width
  this->setMinimumWidth(1020);

// Timer for reading raw data (every 10ms)
  QTimer *timerArduino = new QTimer();
  timerArduino->connect(timerArduino, SIGNAL(timeout()),this, SLOT(onTimerReadData()));
  timerArduino->start(10);
//  connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
  createActions();
  createMenus();
  createToolBars();

  createStatusBar();
  stateWasModified();
}

void MainWindow::loadSettings()
{
  QSettings settings(settingsFileName, QSettings::IniFormat);
  portName       = settings.value("portName", "").toString();
  portLocation   = settings.value("portLocation", "").toString();
  generalTabView = settings.value("generalTabView", false).toBool();
  graphTabView   = settings.value("graphTabView", false).toBool();
  consoleTabView = settings.value("consoleTabView", false).toBool();
}

void MainWindow::saveSettings(){
  QSettings settings(settingsFileName, QSettings::IniFormat);
  settings.value("portName", portName);
  settings.value("portLocation", portLocation);
  settings.value("generalTabView", generalTabView);
  settings.value("graphTabView", graphTabView);
  settings.value("consoleTabView", consoleTabView);
}

void MainWindow::createActions(){
  connectAct = new QAction(QIcon(""), tr("&Connect"), this);
  connect(connectAct, SIGNAL(triggered()), this, SLOT(connectDevice()));

  disconnectAct = new QAction(QIcon(""), tr("&Disconnect"), this);
  connect(disconnectAct, SIGNAL(triggered()), this, SLOT(disconnectDevice()));

  generalAction = new QAction("&General", this);
  generalAction->setCheckable(true);
  generalAction->setChecked(false);
  generalAction->setData(0);
  if(generalTabView == true){
      generalAction->setChecked(true);
      generalAction->setData(true);
    }
  connect(generalAction, SIGNAL(triggered()), this, SLOT(generalMenuActionTriggered()));

  graphsAction = new QAction("&Graps", this);
  graphsAction->setCheckable(true);
  graphsAction->setChecked(false);
  graphsAction->setData(0);
  if(graphTabView == true){
      graphsAction->setChecked(true);
      graphsAction->setData(true);
    }
  connect(graphsAction, SIGNAL(triggered()), this, SLOT(graphsMenuActionTriggered()));

  consoleAction = new QAction("&Console", this);
  consoleAction->setCheckable(true);
  consoleAction->setChecked(false);
  consoleAction->setData(false);
  if(consoleTabView == true){
      consoleAction->setChecked(true);
      consoleAction->setData(true);
    }
  connect(consoleAction, SIGNAL(triggered()), this, SLOT(consoleMenuActionTriggered()));


  QTextStream(stdout) << "PN " << portName << "\r\n";
}

//  create menu bar
void MainWindow::createMenus(){
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addSeparator();
  fileMenu->addAction("&Quit", qApp, SLOT (quit()), QKeySequence(tr("Ctrl+Q")));
  fileMenu->addSeparator();

  //  View
  viewMenu = menuBar()->addMenu(tr("&View"));

  tabsMenu = viewMenu->addMenu(tr("&Tabs"));

  viewMenu->addSeparator();
  viewportMenu = viewMenu->addMenu(tr("&Viewport"));
  viewportMenu->addAction(tr("&Front View"),      objectGL, SLOT (FrontView()),     QKeySequence(tr("Ctrl+f")));
  viewportMenu->addAction(tr("&Rear View"),       objectGL, SLOT (RearView()),      QKeySequence(tr("Ctrl+r")));
  viewportMenu->addAction(tr("&Left View"),       objectGL, SLOT (LeftView()),      QKeySequence(tr("Ctrl+l")));
  viewportMenu->addAction(tr("&Right View"),      objectGL, SLOT (RightView()),     QKeySequence(tr("Ctrl-r")));
  viewportMenu->addAction(tr("&Top View"),        objectGL, SLOT (TopView()),       QKeySequence(tr("Ctrl+t")));
  viewportMenu->addAction(tr("&Bottom View"),     objectGL, SLOT (BottomView()),    QKeySequence(tr("Ctrl+b")));
  viewportMenu->addAction(tr("&Isometric View"),  objectGL, SLOT (IsometricView()), QKeySequence(tr("Ctrl+i")));

  tabsMenu->addAction(generalAction);
  tabsMenu->addAction(graphsAction);
  tabsMenu->addAction(consoleAction);
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

void MainWindow::generalMenuActionTriggered(){
  QSettings settings(settingsFileName, QSettings::IniFormat);
  generalTabView = !generalTabView;
  settings.setValue("generalTabView", generalTabView);
  settings.sync();
  stateWasModified();
  if(generalTabView){
      tabs->insertTab(0, generalTab, tr("General"));
    }else{
      tabs->removeTab(tabs->indexOf(generalTab));
    }
}

void MainWindow::graphsMenuActionTriggered(){
  QSettings settings(settingsFileName, QSettings::IniFormat);
  graphTabView = !graphTabView;
  settings.setValue("graphTabView", graphTabView);
  settings.sync();
  stateWasModified();
  if(graphTabView){
      tabs->insertTab(1, graphTab, tr("Graph"));
    }else{
      tabs->removeTab(tabs->indexOf(graphTab));
    }
}

void MainWindow::consoleMenuActionTriggered(){
  QSettings settings(settingsFileName, QSettings::IniFormat);
  consoleTabView =!consoleTabView;
  settings.setValue("consoleTabView", consoleTabView);
  settings.sync();
  stateWasModified();
  if(consoleTabView){
      tabs->insertTab(2, consoleTab, tr("Console"));
    }else{
      tabs->removeTab(tabs->indexOf(consoleTab));
    }
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
          char buffer[300];
          mpu9250.readString(buffer, '\n', 300, 0);

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
          std::cout << counter << "\t";
          std::cout << packet_id << "\t";
          std::cout << core_time << "\t";

          std::cout << iax << "\t" << iay << "\t" << iaz << "\t";
          std::cout << igx << "\t" << igy << "\t" << igz << "\t";
          std::cout << imx << "\t" << imy << "\t" << imz << "\t";
//          std::cout << "\r\n";
          std::cout << std::endl;

          // Convert into floats
          float ax,ay,az;
          float gx,gy,gz;
          float mx,my,mz;

          ax=iax*RATIO_ACC;
          ay=iay*RATIO_ACC;
          az=iaz*RATIO_ACC;

          gx=(igx-48.4827)*RATIO_GYRO;
          gy=(igy+76.3552)*RATIO_GYRO;
          gz=(igz+64.3234)*RATIO_GYRO;

          mx=imx*RATIO_MAG;
          my=imy*RATIO_MAG;
          mz=imz*RATIO_MAG;

//          assemly data for meta-console
          QByteArray data = buffer;

          QString currentTabName = tabs->tabText(tabs->currentIndex());

          if(currentTabName == "Console" && consoleTabView){
              console->putData(data);
          }else if(currentTabName == "Graph" && graphTabView){
              accelerometerGraph->dispatchData(core_time, ax, ay, az);
              gyroscopeGraph->dispatchData(core_time, gx, gy, gz);
              magnetometerGraph->dispatchData(core_time, mx, my, mz);
          }else if(currentTabName == "General" && generalTabView){
              objectGL->setAccelerometer(ax, ay, az);
              objectGL->setGyroscope(gx, gy, gz);
              objectGL->setMagnetometer(mx, my, mz);
          }

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
