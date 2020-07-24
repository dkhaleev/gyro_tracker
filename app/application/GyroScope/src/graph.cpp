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

//new style via QCustomPlot
Graph::Graph(QWidget *parent):
  m_CustomPlot(0),
  timeInterval(400)
{
  QVBoxLayout *layout = new QVBoxLayout;
//  ui->setupUi(this);
  setMinimumWidth( 900 );
  // Allocate memory for the plot widget object
  m_CustomPlot = new QCustomPlot( this );
  // The next line is responsible for adding
  // the plot widget to our application's main window
//  setCentralWidget( m_CustomPlot ); // there is no need to add and promote a widget


  // Add a graph to the plot widget ( the main graph )
  m_CustomPlot->addGraph();
  // Add a graph to the plot widget ( this will be a horizontal line )
  // that folows the current value of the main graph
  m_CustomPlot->addGraph();

  // tell the plot widget to display the x axis values as a time
  // instead of numbers
  m_CustomPlot->xAxis->setTickLabelType( QCPAxis::ltDateTime );
  // Set the x axis time string format
  m_CustomPlot->xAxis->setDateTimeFormat( "hh:mm:ss:zzz" );
  m_CustomPlot->xAxis->setAutoTickStep( true );
  m_CustomPlot->yAxis->setAutoTickStep( true );

  // Allocating memory for the text item that will
  // display the current value as a text.
  m_ValueIndex = new QCPItemText( m_CustomPlot );
  // Set the font of the text item
  m_ValueIndex->setFont(QFont(font().family(), 8)); // make font a bit larger
  // Add the text item to the plt widget
  m_CustomPlot->addItem( m_ValueIndex );

  // Change the color of the graphs
  QColor brushClr = Qt::green;
  brushClr.setAlphaF( .5 );
  m_CustomPlot->graph( 0 )->setBrush( brushClr );
  m_CustomPlot->graph( 0 )->setPen( QColor() );
  m_CustomPlot->graph( 1 )->setPen( QColor() );


  // setup a timer and start it.
//  QTimer *timer = new QTimer(this);
//  connect(timer, SIGNAL( timeout() ), this, SLOT( updatePlot() ) );
//  timer->start( timeInterval );
  m_CustomPlot->setMinimumSize(this->minimumWidth(), 200);

  this->setFixedHeight(200);
  this->setLayout(layout);
  this->show();

}

void Graph::updatePlot()
{

    // Get the current time
    QTime tm = QTime::currentTime();
    // Convert the curretn time to millseconds
    qreal seconds = 60 * 60 * tm.hour() + 60 * tm.minute() + tm.second();
    qreal timeValue = 1000 * seconds + tm.msec();
    // Add the time the x data buffer
    m_XData.append( timeValue );
    // Generate random data with small variations
    // This will generate a random intiger between [ 0 , 1 ]
    qreal r = static_cast<qreal>( rand() ) / RAND_MAX  ;
    // the next value will be 80 plus or minus 5
    qreal value = 80 + 5 * r;
    m_YData.append( value );

    // Keep the data buffers size under 100 value each,
    // so our moemoty won't explode with random numbers
    if( m_XData.size() > 100 ){
        m_XData.remove( 0 );
        m_YData.remove( 0 );
    }

    // Add the data to the graph
    m_CustomPlot->graph( 0 )->setData( m_XData , m_YData );
    // Now this is the tricky part, the previous part
    // was easy and nothing new in it.

    // Set the range of the vertical and horizontal axis of the plot ( not the graph )
    // so all the data will be centered. first we get the min and max of the x and y data
    QVector<double>::iterator xMaxIt = std::max_element( m_XData.begin() , m_XData.end() );
    QVector<double>::iterator xMinIt = std::min_element( m_XData.begin() , m_XData.end() );
    QVector<double>::iterator yMaxIt = std::max_element( m_YData.begin() , m_YData.end() );


    qreal yPlotMin = 0;
    qreal yPlotMax = *yMaxIt;

    qreal xPlotMin = *xMinIt;
    qreal xPlotMax = *xMaxIt;

    // The yOffset just to make sure that the graph won't take the whole
    // space in the plot widget, and to keep a margin at the top, the same goes for xOffset
    qreal yOffset = 0.3 * ( yPlotMax - yPlotMin ) ;
    qreal xOffset = 0.5 *( xPlotMax - xPlotMin );
    m_CustomPlot->xAxis->setRange( xPlotMin , xPlotMax + xOffset );
    m_CustomPlot->yAxis->setRange(yPlotMin , yPlotMax + yOffset);
    //************************************************************//
    // Generate the data for the horizontal line, that changes with
    // the last value of the main graph
    QVector<double> tmpYYData;
    QVector<double> tmpXXData;
    // Since it's a horizontal line, we only need to feed it two points
    // and both points have the y value
    tmpYYData.append( m_YData.last() );
    tmpYYData.append( m_YData.last() );

    // To make the line cross the plot widget horizontally,
    // from extreme left to extreme right
    tmpXXData.append( m_XData.first() );
    tmpXXData.append( m_XData.last() + xOffset );
    // Finaly set the horizental line data
    m_CustomPlot->graph( 1 )->setData( tmpXXData , tmpYYData );
//************************************************************//
    // Now to the text item that displays the current value
    // as a string.
    // These are the coordinates of the text item,
    // the offsets here are just to make the text appear
    // next and above a the horizontal line.
    qreal indexX = m_XData.last() + 0.5 * xOffset;
    qreal indexY = m_YData.last() + 0.2 * yOffset;

    // Set the coordinate that we calculated
    m_ValueIndex->position->setCoords( indexX , indexY );
    // Set the text that we want to display
    m_ValueIndex->setText(  QString::number( tmpYYData.last() ) + "  MB/s" );


    // Update the plot widget
    m_CustomPlot->replot();
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

     dispatchAccelerometer(core_time, iax, iay, iaz);
}

void Graph::dispatchAccelerometer(unsigned long core_time, int16_t  iax, int16_t iay, int16_t iaz){
  //stuff X-axis data storage
  m_XData.append(core_time);
  //stuff X-axis data storage for accelerometer X-axis
  m_YData.append(iax);

  //add the data to the graph
  m_CustomPlot->graph(0)->setData(m_XData, m_YData);

  // Set the range of the vertical and horizontal axis of the plot ( not the graph )
  // so all the data will be centered. first we get the min and max of the x and y data
  QVector<double>::iterator xMaxIt = std::max_element( m_XData.begin() , m_XData.end() );
  QVector<double>::iterator xMinIt = std::min_element( m_XData.begin() , m_XData.end() );
  QVector<double>::iterator yMaxIt = std::max_element( m_YData.begin() , m_YData.end() );


  qreal yPlotMin = 0;
  qreal yPlotMax = *yMaxIt;

  qreal xPlotMin = *xMinIt;
  qreal xPlotMax = *xMaxIt;

  // The yOffset just to make sure that the graph won't take the whole
  // space in the plot widget, and to keep a margin at the top, the same goes for xOffset
  qreal yOffset = 0.3 * ( yPlotMax - yPlotMin ) ;
  qreal xOffset = 0.5 *( xPlotMax - xPlotMin );
  m_CustomPlot->xAxis->setRange( xPlotMin , xPlotMax + xOffset );
  m_CustomPlot->yAxis->setRange(yPlotMin , yPlotMax + yOffset);
  //************************************************************//
  // Generate the data for the horizontal line, that changes with
  // the last value of the main graph
  QVector<double> tmpYYData;
  QVector<double> tmpXXData;
  // Since it's a horizontal line, we only need to feed it two points
  // and both points have the y value
  tmpYYData.append( m_YData.last() );
  tmpYYData.append( m_YData.last() );

  // To make the line cross the plot widget horizontally,
  // from extreme left to extreme right
  tmpXXData.append( m_XData.first() );
  tmpXXData.append( m_XData.last() + xOffset );
  // Finaly set the horizental line data
  m_CustomPlot->graph( 1 )->setData( tmpXXData , tmpYYData );
//************************************************************//
  // Now to the text item that displays the current value
  // as a string.
  // These are the coordinates of the text item,
  // the offsets here are just to make the text appear
  // next and above a the horizontal line.
  qreal indexX = m_XData.last() + 0.5 * xOffset;
  qreal indexY = m_YData.last() + 0.2 * yOffset;

  // Set the coordinate that we calculated
  m_ValueIndex->position->setCoords( indexX , indexY );
  // Set the text that we want to display
  m_ValueIndex->setText(  QString::number( tmpYYData.last() ) + "  MB/s" );


  // Update the plot widget
  m_CustomPlot->replot();


}
