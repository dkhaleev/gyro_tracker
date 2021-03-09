#ifndef OBJECTGL_H
#define OBJECTGL_H

#include <QGLWidget>
#include <QtGui>
#include <iostream>
#include "text3d.h"
#include <textGL.h>

//using namespace std;

class ObjectOpenGL : public QGLWidget
{
    Q_OBJECT

public:
    ObjectOpenGL(QWidget *parent = 0);                          // Constructor
    ~ObjectOpenGL();

    void setAccelerometer(double acc_x, double acc_y, double acc_z){ax=acc_x; ay=acc_y; az=acc_z;}
    void setGyroscope(double gyro_x, double gyro_y, double gyro_z){gx=gyro_x; gy=gyro_y; gz=gyro_z;}
    void setMagnetometer(double mag_x, double mag_y, double mag_z){mx=mag_x; my=mag_y; mz=mag_z;}
    void setAngles(double anx, double any, double anz) {angle_x=anx; angle_y=any; angle_z=anz; }

public slots:
    void FrontView(void);                    // Standard view : front view
    void RearView(void);                     // Standard view : read view
    void LeftView(void);                     // Standard view : left view
    void RightView(void);                    // Standard view : right view
    void TopView(void);                      // Standard view : top view
    void BottomView(void);                   // Standard view : bottom view
    void IsometricView(void);

    void SetXRotation(int angle);
    void SetYRotation(int angle);
    void SetZRotation(int angle);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

private:
    void Draw_Frame();
    void Draw_Box();
    void NormalizeAngle(int *angle);

    GLuint      makePlane(const GLfloat *reflectance);
    void        drawPlane(GLuint plane, GLdouble dx, GLdouble dy, GLdouble dz, GLdouble angle);

    GLuint      planeX;
    GLuint      planeY;
    GLuint      planeZ;

    QColor      BackgroundColor;
    QColor      X_AxisColor;
    QColor      Y_AxisColor;
    QColor      Z_AxisColor;

    QColor      X_GridColor;
    QColor      Y_GridColor;
    QColor      Z_GridColor;

    QColor      GridMeshColor;
    QColor      GridPlaneColor;

    QColor      PointsColor;

    QSize       WindowSize;
    QPoint      LastPos;
    GLfloat     dx;
    GLfloat     dy;
    GLfloat     dz;
    GLfloat     Zoom;
    int         xRot;
    int         yRot;
    int         zRot;

    //accelerometer data
    double ax, ay, az;
    //gyroscope data
    double gx, gy, gz;
    //magnetometer data
    double mx, my, mz;
    //Euler angles
    double angle_x, angle_y, angle_z;

    Text3D    text3d;

    void setTextGL(TextGL &textGL);

    TextGL    textGL;
};

#endif // OBJECTGL_H
