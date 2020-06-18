#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>

class Graph : public QWidget
{
  Q_OBJECT

public:
  explicit Graph(QWidget *parent = nullptr);

  void dispatchData(const QByteArray &data);

signals:

};

#endif // GRAPH_H
