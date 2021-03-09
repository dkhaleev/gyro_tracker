#include <textGL.h>

TextGL::TextGL(){

}

void TextGL::setFont(QFont &font){
  QTextStream(stdout) << "Set font \r\n";
  this->font = &font;
}

TextGL::~TextGL(){}
