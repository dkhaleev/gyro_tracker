#include <QtWidgets>
#include "graphTab.h"

GraphTab::GraphTab(Graph &graph, QWidget *parent) : QWidget(parent){
  this->setEnabled(false);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addStretch(1);
  setLayout(mainLayout);
}
