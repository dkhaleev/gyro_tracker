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

class Graph : public QFrame
{
  Q_OBJECT

public:
  explicit Graph(QWidget *parent = 0);

  void dispatchData(const QByteArray &data);

  void dispatchAccelerometer();

  void dispatchGyroscope(const QByteArray &data);

  void dispatchMagnetometer(const QByteArray &data);

protected:
    virtual void paintEvent( QPaintEvent * );
    void drawContents( QPainter *p );

private:
    void shiftDown( QRect &rect, int offset ) const;
    QwtPlotCurve curve;
};

#endif // GRAPH_H
