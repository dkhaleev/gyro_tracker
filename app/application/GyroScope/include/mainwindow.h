#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <rOc_timer.h>
#include <rOc_serial.h>
#include "generalTab.h"
#include "graphTab.h"
#include "consoleTab.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE

class QAction;
class QMenu;

QT_END_NAMESPACE

class Console;
class Graph;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr, int w=600, int h=400);
  ~MainWindow();

private:

private slots:
  void portMenuActionTriggered(QAction * action);
  void consoleMenuActionTriggered();
  void graphsMenuActionTriggered();
  bool connectDevice();
  bool disconnectDevice();

  //input stream reader
  void onTimerReadData();

  //main dispatcher. Replace by signals structure later
  void stateWasModified();

protected:
  void resizeEvent(QResizeEvent *);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createTabs();
  void createStatusBar();


  void loadSettings();
  void saveSettings();

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *tabsMenu;
  QMenu *paramsMenu;
  QMenu *portMenu;
  QMenu *baudrateMenu;
  QMenu *aboutMenu;

  QTabWidget *tabs;

  QToolBar *fileToolBar;

  QAction *connectAct;
  QAction *disconnectAct;
  QAction *graphsAction;
  QAction *consoleAction;

  QWidget generalTabWidget;
  QTabWidget *tabWidget;

  QString settingsFileName;

  Console *console;
  Graph *accelerometerGraph;
  Graph *gyroscopeGraph;
  Graph *magnetometerGraph;

  GraphTab *graphTab;
  ConsoleTab *consoleTab;

  //global port variables;
  QString portName;
  QString portLocation;

  bool consoleTabView;
  bool graphTabView;
  bool isConnected = false;

  //serial device
  rOc_serial mpu9250;

};

typedef struct _portConfig{
  QString name;
  QString description;
  QString manufacturer;
  QString systemLocation;
  QString vendorIdentifier;
  QString productIdentifier;
} *portConfig;
//Q_DECLARE_METATYPE(_portConfig);

#endif // MAINWINDOW_H
