#ifndef TEXT3D_H
#define TEXT3D_H
#include <QOpenGLFunctions>
#include <QString>
#include <QFont>
#include <QFontMetricsF>
#include <stdio.h>
#include <QTextStream>
class Text3D {

public: Text3D();
   void initfont(QFont & f, float thickness); // set up a font and specify the "thickness"
   void print(QString text); // print it in 3D!
private:

   void buildglyph(GLuint b, int c); // create one displaylist for character "c"
   QFont* font;
   QFontMetricsF* fm;
   float glyphthickness;
   GLuint base; // the "base" of our displaylists
};

#endif // TEXT3D_H
