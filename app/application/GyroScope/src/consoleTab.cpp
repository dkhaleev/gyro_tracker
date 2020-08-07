#include <QtWidgets>
#include "consoleTab.h"

ConsoleTab::ConsoleTab(Console &console, QWidget *parent) : QWidget(parent)
{
  QLabel *consoleLabel = new QLabel(tr("Raw Data"));
  consoleLabel->setFrameStyle(QFrame::Panel|QFrame::Sunken);

  QVBoxLayout *mainLayout = new QVBoxLayout;

  mainLayout->addWidget(consoleLabel);
  mainLayout->addWidget(&console);

  setLayout(mainLayout);
}

