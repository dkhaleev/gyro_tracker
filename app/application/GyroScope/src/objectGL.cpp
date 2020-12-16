#include "objectGL.h"

//constructor
ObjectOpenGL::ObjectOpenGL(QWidget *parent):
  QGLWidget(parent){
  // Init colors
  BackgroundColor = QColor::fromRgb(128, 128, 128);
  X_AxisColor = QColor::fromRgb(255, 65, 65, 128);   //Red. X-axis color
  Y_AxisColor = QColor::fromRgb(65, 255, 65, 128);   //Green. Y-axis color
  Z_AxisColor = QColor::fromRgb(65, 65, 128, 128);   //Blue. Z-axis color
  PointsColor = QColor::fromRgb(128, 128, 128, 128); //Gray. Points color

  //set initial angles value
  angle_x=angle_y=angle_z=0;
  ax=ay=az=gx=gy=gz=mx=my=mz=0;

//  TopView();
  IsometricView();

  QTextStream(stdout) << "objectGL \r\n";

}

// Initialize OpenGl
void ObjectOpenGL::initializeGL()
{
    // Intitialize Open GL
    qglClearColor(BackgroundColor);                            // Set backGround color
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);                                    // Depth buffer enabled (Hide invisible items)
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_BLEND);
    glEnable(GL_COLOR_MATERIAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);          // For transparency
    glColorMaterial(GL_FRONT,GL_DIFFUSE);
    glEnable(GL_NORMALIZE);
}

void ObjectOpenGL::TopView(){
  SetXRotation(-90*16); //@TBDL: change me
  SetYRotation(0);
  SetZRotation(0);
  Zoom = 0.5;
  dx=dy=0;

}

void ObjectOpenGL::IsometricView()
{
    SetXRotation(62*16); //@TBDL: change me
    SetYRotation(0); //-45*16);
    SetZRotation(45*16);
    Zoom=0.5;
    dx=dy=0;
}

// Update the rotation around X if necessary
void ObjectOpenGL::SetXRotation(int angle)
{
    NormalizeAngle(&angle);
    if (angle != xRot)
    {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

// Upadate the rotation around Y if necessary
void ObjectOpenGL::SetYRotation(int angle)
{
    NormalizeAngle(&angle);
    if (angle != yRot)
    {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}


// Update the rotation around Z if necessary
void ObjectOpenGL::SetZRotation(int angle)
{
    NormalizeAngle(&angle);
    if (angle != zRot)
    {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

// OpenGL angles are contained in the interval [0 : 360*16]
// Normalize the angle in this interval
void ObjectOpenGL::NormalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle >= 360 * 16)
        *angle -= 360 * 16;
}

// Resize the openGL window according to width and height
void ObjectOpenGL::resizeGL(int width, int height)
{
    WindowSize=QSize(width,height);
}

// Redraw the openGl window
void ObjectOpenGL::paintGL(  )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set the lights
    GLfloat LightAmbient[]={0.4f,0.4f,0.4f,1.0f};
    GLfloat LightDiffuse[]={0.8f,0.8f,0.8f,1.0f};
    glLightfv(GL_LIGHT0,GL_AMBIENT,LightAmbient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,LightDiffuse);
    int LightPos[4]={0,0,10,1};
    glLightiv(GL_LIGHT0,GL_POSITION,LightPos);

    // Move the display according to the current orientation
    glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotated(zRot / 16.0, 0.0, 0.0, 1.0);

    glDisable(GL_LIGHTING);

    // Invert the Y-axis for an orthonormal frame
    glScalef(1,-1,1);

    // Draw the frame
    //    Draw_Frame();

    // Start display of the items
    glPushMatrix();                                             // The following properties are only for the object

    // Zoom according to the view's parameters
    glScalef(Zoom,Zoom,Zoom);                                        // ReZoom the object


    // Light independant (color is constant)

    glLineWidth(5.0);

    //@TBD Later: visualize vectors

    Draw_Box();
    //@TBD Later
}

void ObjectOpenGL::Draw_Box()
{

    glPushMatrix();

    glRotated(angle_z , 0.0, 0.0, 1.0);
    glRotated(angle_y, 0.0, 1.0, 0.0);
    glRotated(-angle_x, 1.0, 0.0, 0.0);

    glPushMatrix();
    glScalef(4.,4.,4.);
    Draw_Frame();
    glPopMatrix();


    // Bottom. Blue
    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(0,0,255,128));
    glVertex3d(-0.8 ,-0.5   ,-0.2);
    glVertex3d(-0.8 ,0.5    ,-0.2);
    glVertex3d(0.8  ,0.5    ,-0.2);
    glVertex3d(0.8  ,-0.5   ,-0.2);
    glEnd();

    // Top. Green
    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(0,255,0,128));
    glVertex3d(-0.8 ,-0.5   ,0.2);
    glVertex3d(0.8  ,-0.5   ,0.2);
    glVertex3d(0.8  ,0.5    ,0.2);
    glVertex3d(-0.8 ,0.5    ,0.2);
    glEnd();

    //Front. Red
    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(255,0,0,128));
    glVertex3d(0.8  ,-0.5    ,0.2);
    glVertex3d(0.8  ,-0.5    ,-0.2);
    glVertex3d(0.8  ,0.5     ,-0.2);
    glVertex3d(0.8  ,0.5     ,0.2);
    glEnd();

    //Yellow. Back
    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(255,255,0,128));
    glVertex3d(-0.8  ,-0.5    ,0.2);
    glVertex3d(-0.8  ,0.5     ,0.2);
    glVertex3d(-0.8  ,0.5     ,-0.2);
    glVertex3d(-0.8  ,-0.5    ,-0.2);
    glEnd();

    //Magenta. Left
    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(255,0,255,128));
    glVertex3d(-0.8  ,0.5    ,0.2);
    glVertex3d(0.8   ,0.5     ,0.2);
    glVertex3d(0.8   ,0.5     ,-0.2);
    glVertex3d(-0.8  ,0.5    ,-0.2);
    glEnd();

    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(0,255,255,128));
    glVertex3d(-0.8  ,-0.5    ,0.2);
    glVertex3d(-0.8  ,-0.5    ,-0.2);
    glVertex3d(0.8   ,-0.5     ,-0.2);
    glVertex3d(0.8   ,-0.5     ,0.2);
    glEnd();

    //TBD Later
}

void ObjectOpenGL::Draw_Frame(){
  glLineWidth(10.0);
  //X-axis vector
  glBegin(GL_LINES);
  qglColor(X_AxisColor);
  glVertex3d(0,0,0);
  glVertex3d(0.25,0,0);
  glEnd();

  // Y-axis
  glBegin(GL_LINES);
  qglColor(Y_AxisColor);
  glVertex3d(0,0,0);
  glVertex3d(0, 0.25, 0);
  glEnd();

  // Z-axis
  glBegin(GL_LINES);
  qglColor(Z_AxisColor);
  glVertex3d(0,0,0);
  glVertex3d(0, 0, 0.25);
  glEnd();
}

// The user has pressed a button
// Memorizes the last mouse position
void ObjectOpenGL::mousePressEvent(QMouseEvent *event){
    // Right button (Rotate)
    if(event->buttons()==Qt::RightButton)
        LastPos = event->pos();
    // Left button (Move)
    if(event->buttons()==Qt::LeftButton)
        LastPos = event->pos();
}

// Wheel event : Change the Zoom
void ObjectOpenGL::wheelEvent(QWheelEvent *event)
{

    if(event->delta()<0)
        Zoom/= 1-(event->delta()/120.0)/10.0;
    if(event->delta()>0)
        Zoom*= 1+(event->delta()/120.0)/10.0;
}

// Mouse move event
// Update the view if necessary
void ObjectOpenGL::mouseMoveEvent(QMouseEvent *event)
{
    // Left button : move
    if(event->buttons()==Qt::LeftButton)
    {
        // Compute the difference with the previous position and scale to [-0.5 ; 0.5]
        dx+= -(event->x() - LastPos.x() )/(double)WindowSize.width();
        dy+= -(event->y() - LastPos.y() )/(double)WindowSize.height();
        // Update the view according to the new position
        //        resizeGL(WindowSize.width(),WindowSize.height());
        LastPos = event->pos();
    }

    // Right button (Rotate)
    if(event->buttons()==Qt::RightButton)
    {
        // Get the difference with the previous position
        int dx_mouse = event->x() - LastPos.x();
        int dy_mouse = event->y() - LastPos.y();
        // Update the rotation
        SetXRotation(xRot - 4 * dy_mouse);
        SetYRotation(yRot + 4 * dx_mouse);

        // Memorize previous position
        LastPos = event->pos();
    }
}

//Destructor
ObjectOpenGL::~ObjectOpenGL(){
  makeCurrent();
}