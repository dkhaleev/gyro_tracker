#ifndef GRAPHMETER_H
#define GRAPHMETER_H
#include <QWidget>
#include <QLabel>

class GraphMeter : public QWidget{
  Q_OBJECT

public:
  explicit GraphMeter(QWidget *parent = 0);

//  void dispatchData(double iax, double iay, double iaz);

  QLabel *XAxisLabel;
  QLabel *XAxisValue;
  QLabel *YAxisLabel;
  QLabel *YAxisValue;
  QLabel *ZAxisLabel;
  QLabel *ZAxisValue;

public slots:
  void dispatchData(double, double, double);

};


#endif // GRAPHMETER_H


