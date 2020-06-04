#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE

class QAction;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr, int w=600, int h=400);
  ~MainWindow();

private:

private slots:
  void portMenuActionTriggered(QAction * action);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  void loadSettings();
  void saveSettings();

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *paramsMenu;
  QMenu *portMenu;
  QMenu *baudrateMenu;
  QMenu *aboutMenu;

  QToolBar *fileToolBar;

  QAction *connectAct;
  QAction *disconnectAct;

  QString settingsFileName;

  //global port variables;
  QString portName;
  QString portLocation;
  bool isConnected = false;
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
