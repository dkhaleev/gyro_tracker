#include "graph.h"
#include <QTextStream>
Graph::Graph(QWidget *parent) : QWidget(parent)
{

}

void Graph::dispatchData(const QByteArray &data){
  QTextStream(stdout) << "Graph dispatch \r\n";
}
