#include <QtWidgets>
#include "consoleTab.h"

ConsoleTab::ConsoleTab(Console &console, QWidget *parent) : QWidget(parent){
//  this->setEnabled(false);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(&console);

  mainLayout->addStretch(1);
  setLayout(mainLayout);
}
