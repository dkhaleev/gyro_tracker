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

private:
  void createActions();
  void createMenus();
  void createToolBars();

  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *paramsMenu;
  QMenu *portMenu;
  QMenu *baudrateMenu;
  QMenu *aboutMenu;

  QToolBar *fileToolBar;

  QAction *connectAct;
  QAction *disconnectAct;

  typedef struct _portConfig{
    QString name;
    QString description;
    QString manufacturer;
    QString systemLocation;
    QString vendorIdentifier;
    QString productIdentifier;
  }*portConfig;


};
#endif // MAINWINDOW_H
