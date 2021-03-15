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
    void setString(QString textString);
    void setPlane(QString plane="xy");
    void setZeroPoint(float x=0, float y=0, float z=0);
    void setHeight(float height=1);
    void setThickness(float thickness=1);
    void setColour(QColor &fontColor);
    void renderText();

  private:
    QFont font;
    QString textString;
    QString* plane;
    float* zeroX;
    float* zeroY;
    float* zeroZ;
    float* height;
    float* thickness;
    QColor* fontColor;
    QPainterPath* path;

};

#endif // TEXTGL_H
