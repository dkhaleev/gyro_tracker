#include <QtWidgets>
#include "generalTab.h"

GeneralTab::GeneralTab(ObjectOpenGL *ObjectOpenGL, QWidget *parent) : QWidget(parent)
{
  QTextStream(stdout) << "general tab \r\n";

   centralWidget = new QWidget(this);
   gridLayoutWidget = new QWidget(centralWidget);
   gridLayoutWidget->setGeometry(0, 0, this->width(), this->height());

   gridLayout = new QGridLayout(gridLayoutWidget);
   gridLayout->addWidget(ObjectOpenGL, 0,0,1,1);

   setLayout(gridLayout);
}
