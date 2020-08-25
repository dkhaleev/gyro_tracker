#include <QtWidgets>
#include "graphTab.h"
#include "graphMeter.h"

GraphTab::GraphTab(Graph &accelerometer, Graph &gyroscope, Graph &magnetometer, QWidget *parent) : QWidget(parent)
{
  QLabel *accelerometerLabel = new QLabel(tr("Accelerometer"));
  QLabel *gyroscopeLabel     = new QLabel(tr("Gyroscope"));
  QLabel *magnetometerLabel  = new QLabel(tr("Magnetometer"));

  QWidget *accelerometerMeter = new GraphMeter();
  QWidget *gyroscopeMeter     = new GraphMeter();
  QWidget *magnetometerMeter  = new GraphMeter();

  accelerometerLabel->setFixedHeight(15);

  gyroscopeLabel->setFixedHeight(15);

  magnetometerLabel->setFixedHeight(15);

  accelerometer.setMinimumWidth(1020);

  QGridLayout *mainLayout = new QGridLayout;
  // addWidget(*Widget, row, column, rowspan, colspan)
  mainLayout->addWidget(accelerometerLabel, 0, 0, 1, 2);
  mainLayout->addWidget(&accelerometer, 1, 0, 1, 1);
  mainLayout->addWidget(accelerometerMeter, 1, 2, 1, 1);

  QObject::connect(&accelerometer,
                   SIGNAL(dataDispatched(double, double, double)),
                   accelerometerMeter,
                   SLOT(dispatchData(double, double, double))
                   );


  mainLayout->addWidget(gyroscopeLabel, 2, 0, 1, 2);
  mainLayout->addWidget(&gyroscope, 3, 0, 1, 1);
  mainLayout->addWidget(gyroscopeMeter, 3, 2, 1, 1);

  QObject::connect(&gyroscope,
                   SIGNAL(dataDispatched(double, double, double)),
                   gyroscopeMeter,
                   SLOT(dispatchData(double, double, double))
                   );


  mainLayout->addWidget(magnetometerLabel, 4, 0, 1, 2);
  mainLayout->addWidget(&magnetometer, 5, 0, 1, 1);
  mainLayout->addWidget(magnetometerMeter, 5, 2, 1, 1);

  QObject::connect(&magnetometer,
                   SIGNAL(dataDispatched(double, double, double)),
                   magnetometerMeter,
                   SLOT(dispatchData(double, double, double))
                   );

  setLayout(mainLayout);

  this->show();
}
