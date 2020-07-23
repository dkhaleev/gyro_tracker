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
  explicit Graph(QWidget *parent = 0);

  void dispatchData(const QByteArray &data);

  void dispatchAccelerometer();

  void dispatchGyroscope(const QByteArray &data);

  void dispatchMagnetometer(const QByteArray &data);

private slots:
    // This function is responsible for ploting
    // and updating the graphs , with each timer tick
    void updatePlot();

private:
//    Ui::MainWindow *ui;
    // Our custom plot widget
    QCustomPlot * m_CustomPlot;
    // This object will hold the current value as a text
    // that will appear at the extreme right of the plot,
    QCPItemText *m_ValueIndex;

    // The time between each update, this
    // will be  used by the timer to call "updatePlot" slot
    qreal timeInterval;

    // Data buffers
    QVector<qreal> m_YData;
    QVector<qreal> m_XData;


};

#endif // GRAPH_H
