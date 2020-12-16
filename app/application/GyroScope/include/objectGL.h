#ifndef OBJECTGL_H
#define OBJECTGL_H

#include <QGLWidget>
#include <QtGui>
#include <iostream>


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
    void TopView(void);
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

    QColor      BackgroundColor;
    QColor      X_AxisColor;
    QColor      Y_AxisColor;
    QColor      Z_AxisColor;
    QColor      PointsColor;
    QSize       WindowSize;

    QPoint      LastPos;
    GLfloat     dx;
    GLfloat     dy;
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
};

#endif // OBJECTGL_H