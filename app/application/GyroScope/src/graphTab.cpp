#include <QtWidgets>
#include "graphTab.h"

GraphTab::GraphTab(Graph &accelerometer, Graph &gyroscope, Graph &magnetometer, QWidget *parent) : QWidget(parent)
{
  QLabel *accelerometerLabel = new QLabel(tr("Accelerometer"));
  QLabel *gyroscopeLabel     = new QLabel(tr("Gyroscope"));
  QLabel *magnetometerLabel  = new QLabel(tr("Magnetometer"));

  accelerometerLabel->setFixedHeight(15);

  gyroscopeLabel->setFixedHeight(15);

  magnetometerLabel->setFixedHeight(15);

  accelerometer.setMinimumWidth(1020);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(accelerometerLabel);
  mainLayout->addWidget(&accelerometer);

  mainLayout->addWidget(gyroscopeLabel);
  mainLayout->addWidget(&gyroscope);

  mainLayout->addWidget(magnetometerLabel);
  mainLayout->addWidget(&magnetometer);

  setLayout(mainLayout);

  this->show();
}
