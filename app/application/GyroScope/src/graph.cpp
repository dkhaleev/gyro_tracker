#include "graph.h"
#include <QTextStream>
#include "stdio.h"

#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>

#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qframe.h>

#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QGLWidget>

//
//   Array Sizes
//
const int Size = 27;
const int CurvCnt = 1;
double xval[Size];
double yval[Size];
QwtScaleMap xMap;
QwtScaleMap yMap;

// Preverse parse raw data
long int counter = 0;
unsigned long packet_id = 0;
unsigned long core_time = 0;

int16_t  iax, iay, iaz = 0;
int16_t  igx, igy, igz = 0;
int16_t  imx, imy, imz = 0;

Graph::Graph(QWidget *parent) : QFrame(parent)
{
  QVBoxLayout *layout = new QVBoxLayout;

  int i;

  xMap.setScaleInterval( -0.5, 10.5 );
  yMap.setScaleInterval( -1.1, 1.1 );

  //
  //  Frame style
  //
  setFrameStyle( QFrame::Box | QFrame::Sunken );
  setLineWidth( 2 );
  setMidLineWidth( 3 );

  //
  // Calculate values
  //
  for( i = 0; i < Size; i++ )
  {
      xval[i] = double( i ) * 10.0 / double( Size - 1 );
      yval[i] = qSin( xval[i] ) * qCos( 2.0 * xval[i] );
  }

  i = 0;



  curve.setSymbol( new QwtSymbol( QwtSymbol::Cross, Qt::NoBrush,
      QPen( Qt::black ), QSize( 5, 5 ) ) );
  curve.setPen( Qt::darkGreen );
  curve.setStyle( QwtPlotCurve::Lines );
  curve.setCurveAttribute( QwtPlotCurve::Fitted );

  i++;

  QPalette p = palette();
  p.setColor(QPalette::Base, Qt::red);
  p.setColor(QPalette::Text, Qt::blue);
  setPalette(p);
  QTextStream(stdout) << "graph! \r\n";
  setFrameStyle( QFrame::Box | QFrame::Sunken );
  setLineWidth( 2 );
  setMidLineWidth( 3 );
  curve.setRawSamples( xval, yval, Size );
  this->setFixedHeight(200);
  this->setLayout(layout);
  this->show();
}

void Graph::dispatchData(const QByteArray &data){

  sscanf(data, "%lu %lu %lu %hu %hu %hu %hu %hu %hu %hu %hu %hu",
         &counter,
         &packet_id,
         &core_time,
         &iax,
         &iay,
         &iaz,
         &igx,
         &igy,
         &igz,
         &imx,
         &imy,
         &imz
         );


      QTextStream(stdout) << printf("Graph dispatch index [%lu] \r\n", packet_id);

     dispatchAccelerometer();
}

void Graph::dispatchAccelerometer(){

}

void Graph::shiftDown( QRect &rect, int offset ) const
{
    rect.translate( 0, offset );
}

void Graph::paintEvent( QPaintEvent *event )
{
    QFrame::paintEvent( event );

    QPainter painter( this );
    painter.setClipRect( contentsRect() );
    drawContents( &painter );
}


//REDRAW CONTENTS

void Graph::drawContents( QPainter *painter )
{
  int deltay, i;

  QRect r = contentsRect();

  deltay = r.height() / CurvCnt - 1;

  r.setHeight( deltay );

  //
  //  draw curves
  //
  for ( i = 0; i < CurvCnt; i++ )
  {
      xMap.setPaintInterval( r.left(), r.right() );
      yMap.setPaintInterval( r.top(), r.bottom() );

      painter->setRenderHint( QPainter::Antialiasing,
          curve.testRenderHint( QwtPlotItem::RenderAntialiased ) );
      curve.draw( painter, xMap, yMap, r );

      shiftDown( r, deltay );
  }

  //
  // draw titles
  //
  r = contentsRect();     // reset r
  painter->setFont( QFont( "Helvetica", 8 ) );

  const int alignment = Qt::AlignTop | Qt::AlignHCenter;

  painter->setPen( Qt::black );

    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
        alignment, "Style: Line/Fitted, Symbol: Cross" );
    shiftDown( r, deltay );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Sticks, Symbol: Ellipse" );
//    shiftDown( r, deltay );

//    painter->drawText( 0 , r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Lines, Symbol: None" );
//    shiftDown( r, deltay );

//    painter->drawText( 0 , r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Lines, Symbol: None, Antialiased" );
//    shiftDown( r, deltay );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: Steps, Symbol: None" );
//    shiftDown( r, deltay );

//    painter->drawText( 0, r.top(), r.width(), painter->fontMetrics().height(),
//        alignment, "Style: NoCurve, Symbol: XCross" );
}
