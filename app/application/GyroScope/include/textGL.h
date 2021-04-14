#ifndef TEXTGL_H
#define TEXTGL_H
#include <QTextStream>
#include <QPainter>
#include <QGLWidget>
#include <qfont.h>

class TextGL {
  public:
    TextGL();
    ~TextGL();

    void setFont(QFont font);
    void setPlane(QString plane="xy");
    void setThickness(float thickness=1);
    void setColour(QColor &fontColor);
    GLuint renderText(GLuint list, QString textString, float x=0, float y=0, float z=0, float scale=1, QString plane="x");

  private:
    QFont font;
    QString textString;
    QString* plane;
    float* zeroX;
    float* zeroY;
    float* zeroZ;
    float* thickness;
    QColor* fontColor;
    QPainterPath* path;

};

#endif // TEXTGL_H
