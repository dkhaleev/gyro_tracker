#include <textGL.h>

#include <QtOpenGL>
#include <GL/glu.h>
#ifndef CALLBACK
#define CALLBACK
#endif
typedef void (*TessFuncPtr)();


void CALLBACK tessBeginCB(GLenum which)
{
    glBegin(which);
}

void CALLBACK tessEndCB()
{
    glEnd();
}

void CALLBACK tessVertexCB(const GLvoid *data)
{
    const GLdouble *ptr = (const GLdouble*)data;
    glVertex3dv(ptr);
}


GLdouble* NewVector(GLdouble x, GLdouble y);
void CALLBACK BeginCallback(GLenum type);
void CALLBACK EndCallback();
void CALLBACK VertexCallback(GLvoid *vertex);
void CALLBACK CombineCallback(GLdouble coords[3], GLdouble *data[4], GLfloat weight[4], GLdouble **dataOut);
void CALLBACK ErrorCallback(GLenum errorCode);

TextGL::TextGL(){
  glDisable(GL_LIGHTING);
}

void TextGL::setFont(QFont font){
  QTextStream(stdout) << "TextGL: Set font" << "\r\n";
  this->font = font;
}

void TextGL::setPlane(QString plane){
  QTextStream(stdout) << "TextGL: Set plane " << plane << "\r\n";
  this->plane = &plane;
}

void TextGL::setColour(QColor &fontColor){
  QTextStream(stdout) << "TextGL: Set colour " << "\r\n";
  this->fontColor = &fontColor;
}

GLuint TextGL::renderText(
    GLuint list,
    QString textString,
    float thickness,
    float x,
    float y,
    float z,
    float scale,
    QString plane,
    float offsetX,
    float offsetY
    ){
  y = 0 - y; //invert Y because of tesselator mirroring
  QPainterPath path;
  path.addText(QPointF(0, 0), this->font, textString);

  GLUtesselator *tess = gluNewTess();

  gluTessCallback( tess, GLU_TESS_BEGIN,          (GLvoid (CALLBACK *)())tessBeginCB);
  gluTessCallback( tess, GLU_TESS_END,            (GLvoid (CALLBACK *)())tessEndCB);
  gluTessCallback( tess, GLU_TESS_VERTEX,         (GLvoid (CALLBACK *)())tessVertexCB);
  gluTessProperty( tess, GLU_TESS_WINDING_RULE,   GLU_TESS_WINDING_ODD);

  glShadeModel(GL_FLAT);
  gluTessBeginPolygon(tess, NULL);

  int elements = 0; // number of total vertices in one glyph, counting all paths.
  QList<QPolygonF> poly = path.toSubpathPolygons();

  for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it++) {
      elements= (*it).size();
  }

  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW); // To operate on model-view matrix
//  glLoadIdentity(); // Reset the model-view matrix
//  glTranslatef(x, y, -0.00f); // Move right and into the screen

  GLdouble rot = 0.0f;
  GLfloat rot_x = 0.0f;
  GLfloat rot_y = 0.0f;
  GLfloat rot_z = 0.0f;
  if(plane=="x"){//XY-plane. Do nothing

    }else if(plane=="y"){//YZ-plane


    }else if(plane=="z"){

    }
  glRotatef(rot, rot_x, rot_y, rot_z);


  glCullFace(GL_FRONT);
  glEnable(GL_CULL_FACE);

   gluTessBeginPolygon(tess, 0 );
  foreach(QPolygonF polygon, path.toSubpathPolygons()){
      GLdouble (*vertices)[3] = new GLdouble[polygon.count()][3];

      for(int i=0; i<polygon.count(); i++){
          QPointF point = polygon.at(i);
          if(plane == "x"){
              vertices[i][0] = (point.rx()*scale)+x+offsetX;
              vertices[i][1] = (-point.ry()*scale)-y+offsetY;
              vertices[i][2] = (0.0f*scale)+z;
            } else if(plane == "y"){
              vertices[i][0] = (0.0f*scale)+x+offsetX;
              vertices[i][1] = (-point.ry()*scale)-y;
              vertices[i][2] = (-point.rx()*scale)+z+offsetY;
            }else if(plane == "z"){
              vertices[i][0] = (point.ry()*scale)+x+offsetX;
              vertices[i][1] = (0.0f*scale)+y;
              vertices[i][2] = (-point.rx()*scale)+z+offsetY;
            }
        }

        gluTessBeginContour(tess);
        for(int i=0; i<polygon.count(); i++){
          gluTessVertex(tess, vertices[i], vertices[i]);
        }
        gluTessEndContour(tess);
    }
   gluTessEndPolygon(tess);

   glDisable(GL_CULL_FACE);

  gluTessBeginPolygon(tess, 0 );
  foreach(QPolygonF polygon, path.toSubpathPolygons()){
      GLdouble (*vertices)[3] = new GLdouble[polygon.count()][3];
      for(int i=0; i<polygon.count(); i++){
          QPointF point = polygon.at(i);
          if(plane == "x"){
              vertices[i][0] = (point.rx()*scale)+x+offsetX;
              vertices[i][1] = (-point.ry()*scale)-y+offsetY;
              vertices[i][2] = (thickness*scale)+z;
            } else if(plane == "y"){
              vertices[i][0] = (thickness*scale)+x+offsetX;
              vertices[i][1] = (-point.ry()*scale)-y;
              vertices[i][2] = (-point.rx()*scale)+z+offsetY;
            }else if(plane == "z"){
              vertices[i][0] = (point.ry()*scale)+x+offsetX;
              vertices[i][1] = (thickness*scale)+y;
              vertices[i][2] = (-point.rx()*scale)+z+offsetY;
            }
        }

        gluTessBeginContour(tess);
        for(int i=0; i<polygon.count(); i++){
          gluTessVertex(tess, vertices[i], vertices[i]);
        }
        gluTessEndContour(tess);
   }
   gluTessEndPolygon(tess);

   for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it++) {
          glBegin(GL_QUAD_STRIP);
          QPolygonF::iterator p;
          for (p = (*it).begin(); p != it->end(); p++) {
              if(plane=="x"){
                  glVertex3f((p->rx()*scale)+x+offsetX, (-p->ry()*scale)-y+offsetY, (0.0f*scale)+z);
                  glVertex3f((p->rx()*scale)+x+offsetX, (-p->ry()*scale)-y+offsetY, (thickness*scale)+z);
                }else if(plane=="y"){
                  glVertex3f((0.0f*scale)+x, (-p->ry()*scale)-y, (-p->rx()*scale)+z+offsetY);
                  glVertex3f((thickness*scale)+x, (-p->ry()*scale)-y, (-p->rx()*scale)+z+offsetY);
                }else if(plane=="z"){
                  glVertex3f((p->ry()*scale)+x+offsetX, (0.0f*scale)+y, (-p->rx()*scale)+z+offsetY);
                  glVertex3f((p->ry()*scale)+x+offsetX, (thickness*scale)+y, (-p->rx()*scale)+z+offsetY);
                }
          }
          p = (*it).begin();
          if(plane=="x"){
              glVertex3f((p->rx()*scale)+x+offsetX, (-p->ry()*scale)-y+offsetY, (0.0f*scale)+z); // draw the closing quad
              glVertex3f((p->rx()*scale)+x+offsetX, (-p->ry()*scale)-y+offsetY, (thickness*scale)+z); // of the "wrapping"
            }else if(plane=="y"){
              glVertex3f((0.0f*scale)+x, (-p->ry()*scale)-y, (-p->rx()*scale)+z+offsetY);
              glVertex3f((thickness*scale)+x, (-p->ry()*scale)-y, (-p->rx()*scale)+z+offsetY);
            }else if(plane=="z"){
              glVertex3f((p->ry()*scale)+x+offsetX, (0.0f*scale)+y, (-p->rx()*scale)+z+offsetY);
              glVertex3f((p->ry()*scale)+x+offsetX, (thickness*scale)+y, (-p->rx()*scale)+z+offsetY);
            }
          glEnd();
      }

  gluTessEndPolygon(tess);
  gluDeleteTess(tess);

  glDisable(GL_DEPTH_TEST);
  return list;
}

TextGL::~TextGL(){
  glEnable(GL_LIGHTING);
}
