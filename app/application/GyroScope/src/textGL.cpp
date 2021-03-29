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

void TextGL::setString(QString textString){
  QTextStream(stdout) << "TextGL: Set String " << textString << "\r\n";
  this->textString = textString;
}

void TextGL::setPlane(QString plane){
  QTextStream(stdout) << "TextGL: Set plane " << plane << "\r\n";
  this->plane = &plane;
}

void TextGL::setZeroPoint(float x, float y, float z){
  QTextStream(stdout) << "TextGL: Zero point: X:" << x << " Y: " << y << " Z:" << z << "\r\n";
  this->zeroX = &x;
  this->zeroY = &y;
  this->zeroZ = &z;
}

void TextGL::setHeight(float height){
  QTextStream(stdout) << "TextGL: Set height: " << height << "\r\n";
  this->height = &height;
}

void TextGL::setThickness(float thickness){
  QTextStream(stdout) << "TextGL: Set thickness: " << thickness << "\r\n";
  this->thickness = &thickness;
}

void TextGL::setColour(QColor &fontColor){
  QTextStream(stdout) << "TextGL: Set colour " << "\r\n";
  this->fontColor = &fontColor;
}

void TextGL::renderText(){
  QTextStream(stdout) << "TextGL: Render Text \r\n";
  QPainterPath path;
  path.addText(QPointF(0, 0), this->font, this->textString);

  //---contour--------
//  QList<QPolygonF> poly = path.toSubpathPolygons();
//  for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++){
//      glBegin(GL_LINE_LOOP);
//      for (QPolygonF::iterator p = (*i).begin(); p != i->end(); p++)
//          glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, 0);
//      glEnd();
//  }

  //-------single sided tesselated poly--------
  GLuint id = glGenLists(1);
  GLUtesselator *tess = gluNewTess();

  gluTessCallback( tess, GLU_TESS_BEGIN,          (GLvoid (CALLBACK *)())tessBeginCB);
  gluTessCallback( tess, GLU_TESS_END,            (GLvoid (CALLBACK *)())tessEndCB);
  gluTessCallback( tess, GLU_TESS_VERTEX,         (GLvoid (CALLBACK *)())tessVertexCB);
  gluTessProperty( tess, GLU_TESS_WINDING_RULE,   GLU_TESS_WINDING_ODD);

  glNewList(id, GL_COMPILE);
  glShadeModel(GL_FLAT);
  gluTessBeginPolygon(tess, NULL);

  int elements = 0; // number of total vertices in one glyph, counting all paths.
  QList<QPolygonF> poly = path.toSubpathPolygons();

  for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it++) {
      elements= (*it).size();
  }

  glCullFace(GL_FRONT);
  glEnable(GL_CULL_FACE);

   gluTessBeginPolygon(tess, 0 );
  foreach(QPolygonF polygon, path.toSubpathPolygons()){
      GLdouble (*vertices)[3] = new GLdouble[polygon.count()][3];

      for(int i=0; i<polygon.count(); i++){
          QPointF point = polygon.at(i);
          vertices[i][0] = point.rx()*0.1f;
          vertices[i][1] = -point.ry()*0.1f;
          vertices[i][2] = 0.0f;
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
          vertices[i][0] = point.rx()*0.1f;
          vertices[i][1] = -point.ry()*0.1f;
          vertices[i][2] = -0.1f;
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
              glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, 0.0f);
              glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, -0.1f);
          }
          p = (*it).begin();
          glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, 0.0f); // draw the closing quad
          glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, -0.1f); // of the "wrapping"
          glEnd();
      }

  gluTessEndPolygon(tess);
  glEndList();
  gluDeleteTess(tess);

}

TextGL::~TextGL(){
  glEnable(GL_LIGHTING);
}
