#include <textGL.h>

TextGL::TextGL(){

}

void TextGL::setFont(QFont &font){
  QTextStream(stdout) << "Set font" << "\r\n";
  this->font = &font;
}

void TextGL::setString(QString textString=""){
  QTextStream(stdout) << "Set String " << textString << "\r\n";
  this->textString = &textString;
}


void TextGL::setPlane(QString plane){
  QTextStream(stdout) << "Set plane " << plane << "\r\n";
  this->plane = &plane;
}

void TextGL::setZeroPoint(float x, float y, float z){
  QTextStream(stdout) << "Zero point: X:" << x << " Y: " << y << " Z:" << z << "\r\n";
  this->zeroX = &x;
  this->zeroY = &y;
  this->zeroZ = &z;
}

void TextGL::setHeight(float height){
  QTextStream(stdout) << "Set height: " << height << "\r\n";
  this->height = &height;
}

void TextGL::setThickness(float thickness){
  QTextStream(stdout) << "Set thickness: " << thickness << "\r\n";
  this->thickness = &thickness;
}

void TextGL::setColour(QColor &fontColor){
  QTextStream(stdout) << "Set colour " << "\r\n";
  this->fontColor = &fontColor;
}

void TextGL::renderText(){
  QTextStream(stdout) << "Render Text \r\n";
}

TextGL::~TextGL(){}
