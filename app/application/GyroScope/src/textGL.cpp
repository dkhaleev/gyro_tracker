#include <textGL.h>

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

  QList<QPolygonF> poly = path.toSubpathPolygons();
  for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++){
      glBegin(GL_LINE_LOOP);
      for (QPolygonF::iterator p = (*i).begin(); p != i->end(); p++)
          glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, 0);
      glEnd();
  }
}

TextGL::~TextGL(){
  glEnable(GL_LIGHTING);
}
