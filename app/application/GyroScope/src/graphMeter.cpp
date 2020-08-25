#include <graphMeter.h>
#include <QtWidgets>


GraphMeter::GraphMeter(QWidget * parent){


  XAxisLabel = new QLabel("Axis X");
  XAxisValue = new QLabel("0.000000");

  YAxisLabel = new QLabel("Axis Y");
  YAxisValue = new QLabel("0.000000");

  ZAxisLabel = new QLabel("Axis Z");
  ZAxisValue = new QLabel("0.000000");

  QGridLayout *meterLayout = new QGridLayout;
  // addWidget(*Widget, row, column, rowspan, colspan)
  meterLayout->addWidget(XAxisLabel, 0, 0, 1, 1);
  meterLayout->addWidget(XAxisValue, 0, 1, 1, 1);

  meterLayout->addWidget(YAxisLabel, 1, 0, 1, 1);
  meterLayout->addWidget(YAxisValue, 1, 1, 1, 1);

  meterLayout->addWidget(ZAxisLabel, 2, 0, 1, 1);
  meterLayout->addWidget(ZAxisValue, 2, 1, 1, 1);


  setLayout(meterLayout);

  this->show();
}

void GraphMeter::dispatchData(double iax, double iay, double iaz){
  XAxisValue->setNum(iax);
  YAxisValue->setNum(iay);
  ZAxisValue->setNum(iaz);
}
