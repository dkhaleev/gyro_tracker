#ifndef GRAPHTAB_H
#define GRAPHTAB_H

#include <QDialog>
#include <QObject>
#include "graph.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFileInfo;
class QTabWidget;
QT_END_NAMESPACE

class GraphTab : public QWidget {
  Q_OBJECT

public:
  explicit GraphTab(Graph &accelerometer, Graph &gyroscope, Graph &magnetometer, QWidget *parent = 0);

private:
  Graph *graph;
};

#endif // GRAPHTAB_H
