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

  foreach(QPolygonF polygon, path.toSubpathPolygons()){
    GLdouble (*quad)[3] = new GLdouble[polygon.count()][3];

    for(int i=0; i<polygon.count(); i++){
      QPointF point = polygon.at(i);
      quad[i][0] = point.rx()*0.1f;
      quad[i][1] = -point.ry()*0.1f;
      quad[i][2] = 0;
    }

    gluTessBeginContour(tess);
    for(int i=0; i<polygon.count(); i++){
      gluTessVertex(tess, quad[i], quad[i]);
    }
    gluTessEndContour(tess);
  }

  gluTessEndPolygon(tess);
  glEndList();
  gluDeleteTess(tess);

  //---tesselator-------
//  GLuint id = glGenLists(1);
//  QList<QPolygonF> poly = path.toSubpathPolygons();
//  GLUtesselator *tobj = gluNewTess();
//  gluTessCallback(tobj, GLU_TESS_BEGIN, (GLvoid (CALLBACK *)())tessBeginCB);
//  gluTessCallback(tobj, GLU_TESS_VERTEX, (GLvoid (CALLBACK *)())tessVertexCB);
//  gluTessCallback(tobj, GLU_TESS_END, (GLvoid (CALLBACK *)())tessEndCB);
//  gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);

//  glNewList(id, GL_COMPILE); // start a new list
//  glShadeModel(GL_FLAT);
//  gluTessBeginPolygon(tobj, 0 );

//  int elements = 0; // number of total vertices in one glyph, counting all paths.
//  for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it++) {
//    elements= (*it).size();
//  }

//  GLdouble* vertices = (GLdouble*) malloc(elements* 3 * sizeof(GLdouble));
// int j=0;
// for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it++) { // enumerate paths
//     gluTessBeginContour(tobj);
//     int i=0;
//     for (QPolygonF::iterator p = (*it).begin(); p != it->end(); p++) { // enumerate vertices
//         int off = j+i;
//         vertices[off+0] = p->rx();
//         vertices[off+1] = -p->ry();
//         vertices[off+2] = 0; // setting Z offset to zero.
//         gluTessVertex(tobj, &vertices[off], &vertices[off] );
//         i=3; // array math
//     }
//     gluTessEndContour(tobj);
//     j= (*it).size()*3; // some more array math
// }
// gluTessEndPolygon(tobj);


// gluTessBeginPolygon(tobj, 0 );
//    j = 0;
//    for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it) {
//        gluTessBeginContour(tobj);
//        int i = 0;
//        for (QPolygonF::iterator p = (*it).begin(); p != it->end(); p) {
//            int off = j+i;
//            vertices[off+0] = p->rx();
//            vertices[off+1] = -p->ry();
//            vertices[off+2] = -1.0f; //thickness // Z offset set to "minus glyphtickness"
//            gluTessVertex(tobj, &vertices[off], &vertices[off] );
//            i=3;
//        }
//        gluTessEndContour(tobj);
//        j = (*it).size()*3;
//    }
//    gluTessEndPolygon(tobj);
//    free(vertices); // no need for the vertices anymore

//    for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it) {
//           glBegin(GL_QUAD_STRIP);
//           QPolygonF::iterator p;
//           for (p = (*it).begin(); p != it->end(); p) {
//               glVertex3f(p->rx(), -p->ry(), 0.0f);
//               glVertex3f(p->rx(), -p->ry(), -1.0f);
//           }
//           p = (*it).begin();
//           glVertex3f(p->rx(), -p->ry(), 0.0f); // draw the closing quad
//           glVertex3f(p->rx(), -p->ry(), -1.0f); // of the "wrapping"
//           glEnd();
//       }

//    GLfloat gwidth = 1.0f; //font metrics width
//       glTranslatef(gwidth ,0.0f,0.0f);
//       glEndList();
//       gluDeleteTess(tobj);

}

TextGL::~TextGL(){
  glEnable(GL_LIGHTING);
}
