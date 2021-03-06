#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qframe.h>
#include <qcustomplot.h>

class Graph : public QFrame
{
  Q_OBJECT

public:
  explicit Graph(QWidget *parent = 0, QString graphType = "");

  QString getType();

  void dispatchData(unsigned long core_time, float  iax, float iay, float iaz);

  double dax, day, daz = 0;

private slots:
    // This function is responsible for ploting
    // and updating the graphs , with each timer tick
    void updatePlot();

private:
    QString type;
    // Our custom plot widget
    QCustomPlot * m_CustomPlot;
    // This object will hold the current value as a text
    // that will appear at the extreme right of the plot,
    QCPItemText *m_ValueIndexX;
    QCPItemText *m_ValueIndexY;
    QCPItemText *m_ValueIndexZ;

    // The time between each update, this
    // will be  used by the timer to call "updatePlot" slot
    qreal timeInterval;

    // Data buffers    
    QVector<qreal> m_XData;   //X-axis graph accelerometer Timeline data
    QVector<qreal> m_YAXData; //Y-axis graph accelerometer X-axis data
    QVector<qreal> m_YAYData; //Y-asis graph accelerometer Y-axis data
    QVector<qreal> m_YAZData; //Y-axis graph acceleromete Z-axis data

signals:
    void dataDispatched(double, double, double);
};

#endif // GRAPH_H
