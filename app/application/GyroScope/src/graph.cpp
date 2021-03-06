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
double xval[Size];
double yval[Size];
QwtScaleMap xMap;
QwtScaleMap yMap;

QString type;

// Preverse parse raw data
long int counter = 0;
unsigned long packet_id = 0;
unsigned long core_time = 0;

QStringList types;

//new style via QCustomPlot
Graph::Graph(QWidget *parent, QString graphType):
  m_CustomPlot(0),
  timeInterval(400)
{
  types << "accel" << "gyro" << "mag";

  //set graph type: accelerometer, gyroscope, magnetometer
  type = graphType;

  QVBoxLayout *layout = new QVBoxLayout;

  setMinimumWidth(1020);

  // Allocate memory for the plot widget object
  m_CustomPlot = new QCustomPlot( this );
  // The next line is responsible for adding
  // the plot widget to our application's main window

  // Add a graph to the plot widget ( the main accelerometer x-axis graph )
  m_CustomPlot->addGraph();
  // Add a graph to the plot widget ( this will be a horizontal line )
  // that folows the current value of the main x-axis graph
  m_CustomPlot->addGraph();

  // Add a graph to the plot widget ( the main accelerometer y-axis graph )
  m_CustomPlot->addGraph();
  // Add a graph to the plot widget ( this will be a horizontal line )
  // that folows the current value of the main y-axis graph
  m_CustomPlot->addGraph();

  // Add a graph to the plot widget ( the main accelerometer z-axis graph )
  m_CustomPlot->addGraph();
  // Add a graph to the plot widget ( this will be a horizontal line )
  // that folows the current value of the main z-axis graph
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
  m_ValueIndexX = new QCPItemText( m_CustomPlot );
  m_ValueIndexY = new QCPItemText( m_CustomPlot );
  m_ValueIndexZ = new QCPItemText( m_CustomPlot );
  // Set the font of the text item
  m_ValueIndexX->setFont(QFont(font().family(), 8)); // make font a bit larger
  m_ValueIndexY->setFont(QFont(font().family(), 8)); // make font a bit larger
  m_ValueIndexZ->setFont(QFont(font().family(), 8)); // make font a bit larger
  // Add the text item to the plt widget
  m_CustomPlot->addItem( m_ValueIndexX );
  m_CustomPlot->addItem( m_ValueIndexY );
  m_CustomPlot->addItem( m_ValueIndexZ );

  // Change the color of the graphs
  QColor AXBrushClr = Qt::red;
  QColor AYBrushClr = Qt::green;
  QColor AZBrushClr = Qt::blue;

  m_CustomPlot->graph( 0 )->setPen( AXBrushClr );
  m_CustomPlot->graph( 1 )->setPen( QColor() );

  m_CustomPlot->graph( 2 )->setPen( AYBrushClr );
  m_CustomPlot->graph( 3 )->setPen( QColor() );

  m_CustomPlot->graph( 4 )->setPen( AZBrushClr );
  m_CustomPlot->graph( 5 )->setPen( QColor() );

  m_CustomPlot->setMinimumSize(this->minimumWidth(), 180);

  this->setFixedHeight(180);
  this->setLayout(layout);
  this->show();
}

QString Graph::getType(){
  return type;
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
    m_YAXData.append( value );

    // Keep the data buffers size under 100 value each,
    // so our moemoty won't explode with random numbers
    if( m_XData.size() > 100 ){
        m_XData.remove( 0 );
        m_YAXData.remove( 0 );
    }

    // Add the data to the graph
    m_CustomPlot->graph( 0 )->setData( m_XData , m_YAXData );
    // Now this is the tricky part, the previous part
    // was easy and nothing new in it.

    // Set the range of the vertical and horizontal axis of the plot ( not the graph )
    // so all the data will be centered. first we get the min and max of the x and y data
    QVector<double>::iterator xMaxIt = std::max_element( m_XData.begin() , m_XData.end() );
    QVector<double>::iterator xMinIt = std::min_element( m_XData.begin() , m_XData.end() );
    QVector<double>::iterator yMaxIt = std::max_element( m_YAXData.begin() , m_YAXData.end() );


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
    tmpYYData.append( m_YAXData.last() );
    tmpYYData.append( m_YAXData.last() );

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
    qreal indexY = m_YAXData.last() + 0.2 * yOffset;

    // Set the coordinate that we calculated
    m_ValueIndexX->position->setCoords( indexX , indexY );
    // Set the text that we want to display
    m_ValueIndexX->setText(  QString::number( tmpYYData.last() ) + "  MB/s" );


    // Update the plot widget
    m_CustomPlot->replot();
}

void Graph::dispatchData(unsigned long core_time, float  iax, float iay, float iaz){
  //stuff X-axis data storage
  m_XData.append(core_time);

  //shrink graph to size
  if(m_XData.size() > 100){
      m_XData.removeFirst();
      //@todo: refactor me
      //cleanup X-axis accel data
      if(!m_YAXData.empty()){
          m_YAXData.removeFirst();
        }

      //cleanup Y-axis accel data
      if(!m_YAYData.empty()){
          m_YAYData.removeFirst();
        }

      //cleanup Z-axis accel data
      if(!m_YAZData.empty()){
          m_YAZData.removeFirst();
        }
    }

//  argument type conversion
  double d_iax = iax;
  double d_iay = iay;
  double d_iaz = iaz;

  //since sensor uses two-bytes Words for each axis value(2 bytes) is signed
  //so we need to conver them to  -32768 till +32768 depend on your scale  for
  //accelerometer ( ±2g, ±4g, ±8g and ±16g)
  //gyroscope ( ±250, ±500, ±1000, and ±2000°/sec )

  //We need to dovode output values from sensor according to selected sensor range
  //For example for 16G we divide output/2048 (if we have stable the sensor Z axe will
  //show us 1 gravity , so if it show you 2048 the accelerometer is on +-16G mode)
  //We get samples in internal, if we get every 0.001sec you -> velocityZ =accelerationZ*0.001*G
  // => velocityZ=accelerationZ*0.001*9.8
  //  G=9.8m/s2.

  //stuff X-axis data storage for sensor X,Y and Z axes
  //  (data/ +/- sensor range[bytes])*9.8 m/s^2 -> value normalized to m/s^2

  //8192 used in acceleration block is for ±4g doubled (positive and negative)
  //  AFS_SEL | Full Scale Range | LSB Sensitivity
  //     --------+------------------+----------------
  //     0       | ± 2g           | 8192 LSB/mg
  //     1       | ± 4g           | 4096 LSB/mg
  //     2       | ± 8g           | 2048 LSB/mg
  //     3       | ± 16g          | 1024 LSB/mg

  //8192 used in inclination block is for ±1000 degrees/s doubled (positive and negative)
  //  FS_SEL | Full Scale Range   | LSB Sensitivity
  //  -------+--------------------+----------------
  //  0      | ± 250 degrees/s  | 131 LSB/deg/s
  //  1      | ± 500 degrees/s  | 65.5 LSB/deg/s
  //  2      | ± 1000 degrees/s | 32.8 LSB/deg/s
  //  3      | ± 2000 degrees/s | 16.4 LSB/deg/s

  //dimensions
  QString dimensions;
  dimensions = "M/s²";

  switch(types.indexOf(type))
    {
    case 0: //accel
      dimensions = "M/s²";
      m_YAXData.append((d_iax/8192)*9.8);
      m_YAYData.append((d_iay/8192)*9.8);
      m_YAZData.append((d_iaz/8192)*9.8);
      emit dataDispatched(
            (d_iax/8192)*9.8,
            (d_iay/8192)*9.8,
            (d_iaz/8192)*9.8
            );
      break;
    case 1: //gyro
      dimensions = "Deg/s";
      m_YAXData.append(d_iax/32.8);
      m_YAYData.append(d_iay/32.8);
      m_YAZData.append(d_iaz/32.8);
      emit dataDispatched(
            d_iax/32.8,
            d_iay/32.8,
            d_iaz/32.8
            );
      break;
    case 2: //mag -- @todo: dummy value for now. Does not represents the real values
      dimensions = "Deg/s";
      m_YAXData.append((d_iax/8192)*9.8);
      m_YAYData.append((d_iay/8192)*9.8);
      m_YAZData.append((d_iaz/8192)*9.8);
      emit dataDispatched(
            (d_iax/8192)*9.8,
            (d_iay/8192)*9.8,
            (d_iaz/8192)*9.8
            );
      break;
    }



  //add the data to the x-axis accelerometer graph
  m_CustomPlot->graph(0)->setData(m_XData, m_YAXData);
  m_CustomPlot->graph(2)->setData(m_XData, m_YAYData);
  m_CustomPlot->graph(4)->setData(m_XData, m_YAZData);

  // Set the range of the vertical and horizontal axis of the plot ( not the graph )
  // so all the data will be centered. first we get the min and max of the x and y data
  QVector<double>::iterator xMaxIt = std::max_element( m_XData.begin() , m_XData.end() );
  QVector<double>::iterator xMinIt = std::min_element( m_XData.begin() , m_XData.end() );

  //create dynamic array for max acceleration for axes by module
  QVector<double> yMaxIt;
  //max X-axis
  yMaxIt.append( *std::max_element( m_YAXData.begin() , m_YAXData.end() ) );
  //max X-axis by module
  yMaxIt.append( fabs( *std::min_element(m_YAXData.begin() , m_YAXData.end() ) ) );
  //max Y-axis
  yMaxIt.append( *std::max_element( m_YAYData.begin() , m_YAYData.end() ) );
  //max Y-axis by module
  yMaxIt.append( fabs( *std::min_element( m_YAYData.begin() , m_YAYData.end() ) ) );
  //max Z-axis
  yMaxIt.append( *std::max_element( m_YAZData.begin() , m_YAZData.end() ) );
  //max Z-axis by module
  yMaxIt.append( fabs( *std::min_element( m_YAZData.begin() , m_YAZData.end() ) ) );

  qreal xPlotMin = *xMinIt;
  qreal xPlotMax = *xMaxIt;

  //find maximum absolute value
  qreal yMax = fabs(*std::max_element( yMaxIt.begin() , yMaxIt.end() ) );

  // The yOffset just to make sure that the graph won't take the whole
  // space in the plot widget, and to keep a margin at the top, the same goes for xOffset
  qreal yOffset = 0.3 * ( yMax) ;
  qreal xOffset = 0.1 *( xPlotMax - xPlotMin );
  m_CustomPlot->xAxis->setRange( xPlotMin , xPlotMax+xOffset);
  m_CustomPlot->yAxis->setRange(-(yMax+yOffset) , yMax + yOffset);
  //************************************************************//
  // Generate the data for the horizontal line, that changes with
  // the last value of the main graph
  QVector<double> tmpYXData;
  QVector<double> tmpYYData;
  QVector<double> tmpYZData;
  QVector<double> tmpXXData;
  // Since it's a horizontal line, we only need to feed it two points
  // and both points have the y value
  tmpYXData.append( m_YAXData.last() );
  tmpYXData.append( m_YAXData.last() );

  tmpYYData.append( m_YAYData.last() );
  tmpYYData.append( m_YAYData.last() );

  tmpYZData.append( m_YAZData.last() );
  tmpYZData.append( m_YAZData.last() );

  // To make the line cross the plot widget horizontally,
  // from extreme left to extreme right
  tmpXXData.append( m_XData.first() );
  tmpXXData.append( m_XData.last() + xOffset );

//Disabled due the bamming
////************************************************************//
//  // Now to the text item that displays the current value
//  // as a string.
//  // These are the coordinates of the text item,
//  // the offsets here are just to make the text appear
//  // next and above a the horizontal line.
//  qreal indexX = m_XData.last() + 0.5 * xOffset;
//  qreal indexYX = m_YAXData.last() + 0.2 * yOffset;
//  qreal indexYY = m_YAYData.last() + 0.3 * yOffset;
//  qreal indexYZ = m_YAZData.last() + 0.4 * yOffset;

//  // Set the coordinate that we calculated
//  m_ValueIndexX->position->setCoords( indexX , indexYX );
//  // Set the text that we want to display
//  m_ValueIndexX->setText(  QString::number( tmpYXData.last() ) + " " + dimensions );

//  // Set the coordinate that we calculated
//  m_ValueIndexY->position->setCoords( indexX , indexYY );
//  // Set the text that we want to display
//  m_ValueIndexY->setText(  QString::number( tmpYYData.last() ) + " " + dimensions );

//  // Set the coordinate that we calculated
//  m_ValueIndexZ->position->setCoords( indexX , indexYZ );
//  // Set the text that we want to display
//  m_ValueIndexZ->setText(  QString::number( tmpYZData.last() ) + " " + dimensions );


  // Update the plot widget
  m_CustomPlot->replot();

}
