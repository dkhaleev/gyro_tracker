#include "objectGL.h"

//constructor
ObjectOpenGL::ObjectOpenGL(QWidget *parent):
  QGLWidget(parent){
  // Init colors
  BackgroundColor = QColor::fromRgb(128, 128, 128);
  X_AxisColor = QColor::fromRgb(255, 65, 65, 128);   //Red. X-axis color
  Y_AxisColor = QColor::fromRgb(65, 255, 65, 128);   //Green. Y-axis color
  Z_AxisColor = QColor::fromRgb(65, 65, 128, 128);   //Blue. Z-axis color

  X_GridColor = QColor::fromRgb(255, 0, 0, 128); //solid Red
  Y_GridColor = QColor::fromRgb(0, 255, 0, 128); //solid Green
  Z_GridColor = QColor::fromRgb(0, 0, 255, 120); //solid Blue
  GridMeshColor = QColor::fromRgb(10, 10, 10, 128); //dark gray
  GridPlaneColor = QColor::fromRgb(250, 250, 250, 128); //shaded white

  PointsColor = QColor::fromRgb(128, 128, 128, 128); //Gray. Points color

  //set initial angles value
  angle_x=angle_y=angle_z=0;
  ax=ay=az=gx=gy=gz=mx=my=mz=0;

//  IsometricView();
  TopView();

}



// Initialize OpenGl
void ObjectOpenGL::initializeGL()
{
    // Intitialize Open GL
  static const GLfloat reflectanceX[4] = { 0.8f, 0.1f, 0.0f, 1.0f };
  static const GLfloat reflectanceY[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
  static const GLfloat reflectanceZ[4] = { 0.2f, 0.2f, 1.0f, 1.0f };

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

    planeX = makePlane(reflectanceX);

    glEnable(GL_NORMALIZE);
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

#define RAW_ACC
#define RAW_GYRO
#define RAW_MAG

#ifdef RAW_ACC
    // Accelerometer
    glBegin(GL_LINES);
    qglColor(QColor::fromRgb(255 ,51 ,255)); //magenta
    glVertex3d(0,0,0);
    glVertex3d(ax,ay,az);
    glEnd();
#endif

#ifdef RAW_GYRO
    // Gyroscopes
    glBegin(GL_LINES);
    qglColor(QColor::fromRgb(255,255 ,0));
    glVertex3d(0,0,0);
    glVertex3d(gx,gy,gz);
    glEnd();
#endif

#ifdef RAW_MAG
    // Magnetometer
    glBegin(GL_LINES);
    qglColor(QColor::fromRgb(32,32,32));
    glVertex3d(0,0,0);
    glVertex3d(mx,my,mz);
    glEnd();
#endif
    Draw_Box();

    glEnable(GL_LIGHTING);                  // Re enable the light

    drawPlane(planeX, dx, dy, dz, 0);

    // End of the object
    glPopMatrix();


    // Update the view
    glViewport(0, 0,WindowSize.width(), WindowSize.height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat Ratio=(GLfloat)WindowSize.width()/(GLfloat)WindowSize.height();
    glOrtho((-0.5+dx)*Ratio,
            ( 0.5+dx)*Ratio ,
            +0.5+dy,
            -0.5+dy,
            -1500.0, 1500.0);
    glMatrixMode(GL_MODELVIEW);
}

GLuint ObjectOpenGL::makePlane(const GLfloat *reflectance){
  std::cout << "Make Plane called"<<std::endl;
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectance);

  glShadeModel(GL_FLAT);

  glBegin(GL_POLYGON); //front XY-polygon
  qglColor(QColor::fromRgb(50, 50, 50, 128));
  glVertex3d(0.0, 0.0, 0.0);
  glVertex3d(1.0, 0.0, 0.0);
  glVertex3d(1.0, 1.0, 0.0);
  glVertex3d(0.0, 1.0, 0.0);
  glEnd();

  glBegin(GL_POLYGON); //reverse XY-polygon
  qglColor(QColor::fromRgb(60, 60, 60, 128));
  glVertex3d(0.0, 1.0, 0.0);
  glVertex3d(1.0, 1.0, 0.0);
  glVertex3d(1.0, 0.0, 0.0);
  glVertex3d(0.0, 0.0, 0.0);
  glEnd();

  glEndList();

  return list;
}

void ObjectOpenGL::drawPlane(GLuint plane, GLdouble dx, GLdouble dy, GLdouble dz, GLdouble angle){
  std::cout << "DrawPlane called" << std::endl;
  glPushMatrix();
  glTranslated(dx, dy, dz);
  glRotated(angle, 0.0, 0.0, 1.0);
  glCallList(plane);
  glPopMatrix();
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
    qglColor(QColor::fromRgb(0,0,255,128)); //bottom blue
    glVertex3d(-0.8 ,-0.5   ,-0.2);
    glVertex3d(-0.8 ,0.5    ,-0.2);
    glVertex3d(0.8  ,0.5    ,-0.2);
    glVertex3d(0.8  ,-0.5   ,-0.2);
    glEnd();

    // Top. Green
    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(0,255,0,128)); //top green
    glVertex3d(-0.8 ,-0.5   ,0.2);
    glVertex3d(0.8  ,-0.5   ,0.2);
    glVertex3d(0.8  ,0.5    ,0.2);
    glVertex3d(-0.8 ,0.5    ,0.2);
    glEnd();

//    Front. Red
    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(255,0,0,128)); //front red
    glVertex3d(0.8  ,-0.5    ,0.2);
    glVertex3d(0.8  ,-0.5    ,-0.2);
    glVertex3d(0.8  ,0.5     ,-0.2);
    glVertex3d(0.8  ,0.5     ,0.2);
    glEnd();

//    Magenta. Left
    glBegin(GL_POLYGON);
    qglColor(QColor::fromRgb(255,255,0,128));
    glVertex3d(-0.8  ,-0.5    ,0.2);
    glVertex3d(-0.8  ,0.5     ,0.2);
    glVertex3d(-0.8  ,0.5     ,-0.2);
    glVertex3d(-0.8  ,-0.5    ,-0.2);
    glEnd();

//    Cyan. Right
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

    qglColor(QColor::fromRgb(255,255,255,255));
    glPointSize(10.0);

    glBegin(GL_POINTS);
    glVertex3d(-0.8 ,-0.5   ,-0.2);
    glVertex3d(-0.8 ,0.5    ,-0.2);
    glVertex3d(0.8  ,0.5    ,-0.2);
    glVertex3d(0.8  ,-0.5   ,-0.2);
    glVertex3d(-0.8 ,-0.5   ,0.2);
    glVertex3d(-0.8 ,0.5    ,0.2);
    glVertex3d(0.8  ,0.5    ,0.2);
    glVertex3d(0.8  ,-0.5   ,0.2);
    glEnd();

    glPopMatrix();
}

// Draw the frame (X,Y and Z axis)
void ObjectOpenGL::Draw_Frame()
{
    glLineWidth(10.0);
    // X-axis
    glBegin(GL_LINES);
    qglColor(X_AxisColor);
    glVertex3d(0,0,0);
    glVertex3d(0.25, 0, 0);
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

void ObjectOpenGL::FrontView()
{
    SetXRotation(0);
    SetYRotation(0);
    SetZRotation(0);
    Zoom=1;
    dx=dy=0;

}

void ObjectOpenGL::RearView()
{
    SetXRotation(0);
    SetYRotation(180*16);
    SetZRotation(0);
    Zoom=1;
    dx=dy=0;
}

void ObjectOpenGL::LeftView()
{
    SetXRotation(0);
    SetYRotation(90*16);
    SetZRotation(0);
    Zoom=1;
    dx=dy=0;
}

void ObjectOpenGL::RightView()
{
    SetXRotation(0);
    SetYRotation(-90*16);
    SetZRotation(0);
    Zoom=1;
    dx=dy=0;
}

void ObjectOpenGL::TopView()
{
    SetXRotation(-90*16);
    SetYRotation(0);
    SetZRotation(0);
    Zoom=0.5;
   dx=dy=0;
}

void ObjectOpenGL::BottomView()
{
    SetXRotation(90*16);
    SetYRotation(0);
    SetZRotation(0);
    Zoom=0.5;
    dx=dy=0;
}

void ObjectOpenGL::IsometricView()
{
  std::cout << "Top View Caled" << std::endl;
    SetXRotation(-90*16);
    SetYRotation(0);
    SetZRotation(0);
    Zoom=0.5;
    dx=dy=0;
}


// The user has pressed a button
// Memorizes the last mouse position
void ObjectOpenGL::mousePressEvent(QMouseEvent *event){
  std::cout << "Mouse Press event" << std::endl;
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
    updateGL();
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
//                resizeGL(WindowSize.width(),WindowSize.height());
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

    updateGL();
}

// Destructor
ObjectOpenGL::~ObjectOpenGL()
{
    makeCurrent();
}

