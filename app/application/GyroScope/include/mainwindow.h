#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr, int w=600, int h=400);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

private slots:

private:
  QMenu *ParamsMenu;
  QMenu *PortMenu;
  QMenu *BaudrateMenu;

};
#endif // MAINWINDOW_H
