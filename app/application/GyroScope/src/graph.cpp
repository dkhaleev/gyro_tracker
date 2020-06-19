#include "graph.h"
#include <QTextStream>
#include "stdio.h"

#include <qwt_scale_map.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qframe.h>

Graph::Graph(QWidget *parent) : QWidget(parent)
{

}

void Graph::dispatchData(const QByteArray &data){

  // Preverse parse raw data
  long int counter = 0;
  unsigned long packet_id = 0;
  unsigned long core_time = 0;

  int16_t  iax, iay, iaz = 0;
  int16_t  igx, igy, igz = 0;
  int16_t  imx, imy, imz = 0;

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

//  for(int i = 0; i < size; ++i){
      QTextStream(stdout) << printf("Graph dispatch index [%lu] \r\n", packet_id);

//    }

}
