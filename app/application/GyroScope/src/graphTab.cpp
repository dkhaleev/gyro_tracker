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
  mainLayout->addWidget(&graph);

  mainLayout->addStretch(1);
  setLayout(mainLayout);
  this->show();
}
