#include <QtWidgets>
#include "graphTab.h"

GraphTab::GraphTab(Graph &graph, QWidget *parent) : QWidget(parent)
{
  QLabel *graphLabel = new QLabel(tr("Graph QLabel"));
  graphLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  graphLabel->setFixedHeight(20);
//  graphLabel->setFixedWidth(this->width());
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(graphLabel);
//  &graph.setFixedHeight(200);
  graph.setMinimumSize(300,300);
  mainLayout->addWidget(&graph);
//  this->setMinimumSize(300,300);
//  mainLayout->addStretch(1);
  setLayout(mainLayout);
  this->show();
}
