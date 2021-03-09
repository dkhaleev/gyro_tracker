#ifndef TEXTGL_H
#define TEXTGL_H
#include <QTextStream>

#include <qfont.h>

class TextGL {
  public:
    TextGL();
    ~TextGL();

    void setFont(QFont &font);

  private:
  QFont* font;

};

#endif // TEXTGL_H
