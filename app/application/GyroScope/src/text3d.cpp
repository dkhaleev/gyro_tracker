#include <QFont>
#include <QList>
#include <QPainter>
#include <QOpenGLFunctions>
#include <QChar>
#include <gl/GLU.h>
#include "text3d.h"
typedef void (__stdcall *TessFuncPtr)(); // defintion of the callback function type

Text3D::Text3D() // nothing special in the constructor

   : glyphthickness(1.0f)
   , base(0)
{ }

////The initialization just loops through the first 256 char's and calls buildglyph() for each of them.
void Text3D::initfont(QFont& f, float thickness) {
QTextStream(stdout) << "T3D: initialized font \r\n";
   font = &f;
   fm = new QFontMetricsF(f);
   glyphthickness = thickness;
   if (base) // if we have display lists already, delete them first
       glDeleteLists(base, 256);
   base = glGenLists(256); // generate 256 display lists
   if (base==0) {
       QTextStream(stdout) << "cannot create display lists. \r\n";
       throw;
   }
//   for (int i=0; i<25;++i) // loop to build the first 256 glyphs
   int i =2;
       buildglyph(base+i, (char)i);
}

////The print() function uses glCallLists() to "interpret" a complete string. See below how the char-by-char advance works.
void Text3D::print(QString text) {

   glPushAttrib(GL_LIST_BIT); // Pushes The Display List Bits
   glListBase(base); // Sets The Base Character to 0
   glCallLists(text.length(), GL_UNSIGNED_BYTE, text.toLocal8Bit()); // Draws The Display List Text
   glPopAttrib(); // Pops The Display List Bits
}

////At the beginning we need to set up both, the tesselation and the display list.
void Text3D::buildglyph(GLuint listbase, int c) // this is the main "workhorse" function. Create a displaylist with
{
// ID "listbase" from character "c"
   GLUtriangulatorObj *tobj;

   QPainterPath path;
   path.addText(QPointF(0,0),*font, QString((char)c));
   QList<QPolygonF> poly = path.toSubpathPolygons(); // get the glyph outline as a list of paths
   // set up the tesselation
   tobj = gluNewTess();
   gluTessCallback(tobj, GLU_TESS_BEGIN, (TessFuncPtr)glBegin);
   gluTessCallback(tobj, GLU_TESS_VERTEX, (TessFuncPtr)glVertex3dv);
   gluTessCallback(tobj, GLU_TESS_END, (TessFuncPtr)glEnd);
   gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
   glNewList(listbase, GL_COMPILE); // start a new list
   glShadeModel(GL_FLAT);
   gluTessBeginPolygon(tobj, 0 ); // start tesselate
   // first, calculate number of vertices.
   int elements = 0; // number of total vertices in one glyph, counting all paths.
   for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it++) {
       elements= (*it).size();
   }

////Now it's ready to tesselate the "front plate" polygon.

   GLdouble* vertices = (GLdouble*) malloc(elements* 3 * sizeof(GLdouble));
   int j=0;
   for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it++) { // enumerate paths
       gluTessBeginContour(tobj);
       int i=0;
       for (QPolygonF::iterator p = (*it).begin(); p != it->end(); p++) { // enumerate vertices
           int off = j+i;
           vertices[off+0] = p->rx();
           vertices[off+1] = -p->ry();
           vertices[off+2] = 0; // setting Z offset to zero.
           gluTessVertex(tobj, &vertices[off], &vertices[off] );
           i=3; // array math
       }
       gluTessEndContour(tobj);
       j= (*it).size()*3; // some more array math
   }
   gluTessEndPolygon(tobj);


////Do the whole tesselation a second time with an offset applied for the "back plate". The "offset" (thickness) is set in



////The "wrapping" between the two "plates" is simple compared to the tesselation.

   for (QList<QPolygonF>::iterator it = poly.begin(); it != poly.end(); it++) {
       glBegin(GL_QUAD_STRIP);
       QPolygonF::iterator p;
       for (p = (*it).begin(); p != it->end(); p++) {
           glVertex3f(p->rx(), -p->ry(), 0.0f);
           glVertex3f(p->rx(), -p->ry(), -glyphthickness);
       }
       p = (*it).begin();
       glVertex3f(p->rx(), -p->ry(), 0.0f); // draw the closing quad
       glVertex3f(p->rx(), -p->ry(), -glyphthickness); // of the "wrapping"
       glEnd();
   }

////This is where the char-by-char advance is done. Get the width from the font metrics and apply a glTranslate() with that value. This goes into the displaylist as well. (This may have side-effects as the matrix is not in the same "state" as before the call[[Image:|Image:]]!)

   GLfloat gwidth = (float)fm->width(c);
   glTranslatef(gwidth ,0.0f,0.0f);
   glEndList();
//   gluDeleteTess(tobj);
   QTextStream(stdout) << "T3D: BuildGlyph called \r\n";
}

////The whole thing can actually be used in a init() and render() functions within a OpenGL object like this: void init() {

//   QString text = "Qt is great!";
//   QFont dfont("Comic Sans MS", 20);
//   QFontMetrics fm(dfont);
//   int textwidth = fm.width(text);
//   //qDebug() << "width of text: " << textwidth;

//   //initfont(&dfont, float 5);
////}//

////void render() {

////   glEnable(GL_DEPTH_TEST);
////   glMatrixMode(GL_MODELVIEW); // To operate on model-view matrix
////   glLoadIdentity(); // Reset the model-view matrix
////   glTranslatef(0, 0.0f, –500.0f); // Move right and into the screen

////   glRotatef(rot, 1.0f, 0.0f, 0.0f); // Rotate On The X Axis
////   glRotatef(rot*1.5f, 0.0f, 1.0f, 0.0f); // Rotate On The Y Axis
////   glRotatef(rot*1.4f, 0.0f, 0.0f, 1.0f); // Rotate On The Z Axis

////   glColor3f( 1.0f*float(cos(rot/20.0f)), // Animate the color
////       1.0f*float(sin(rot/25.0f)),
////       1.0f-0.5f*float(cos(rot/17.0f))
////   );

////   glTranslatef(-textwidth/2.0f, 0.0f, 0.0f); // textwidth holds the pixel width of the text
////   // Print GL Text To The Screen
////   print(text);
////   glDisable(GL_DEPTH_TEST);
////   rot=0.3f; // increase rot value
////   if (rot > 2000.f)
////       rot = 0.0f; // wrap around at 2000
////}
