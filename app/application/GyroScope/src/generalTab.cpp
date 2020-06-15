#include <QtWidgets>
#include "generalTab.h"

GeneralTab::GeneralTab(const QString &text, QWidget *parent) : QWidget(parent){
  this->setEnabled(false);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addStretch(1);
  setLayout(mainLayout);
}
