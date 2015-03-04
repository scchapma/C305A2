//-------------------------------------------------------------------------------------------
//   Qtgl for graphics class
//-------------------------------------------------------------------------------------------

#include "glwidget.h"

const double lim=0.5;
const double RadPerPixel = - 0.01;
const double MovePerPixel = - 0.1;
GLfloat vertices[4][2] = {{100.0, 100.0}, {400.0, 100.0}, {400.0, 400.0}, {100.0, 400.0}}; //  vertex coords
GLubyte indices[] = {0,1,1,2,2,3,3,0};


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    startup();
}

GLWidget::~GLWidget()
{    

}

void GLWidget::startup()
{
    winw=width();  // width returns width of window
    winh=height();
    button = 0;
    cerr << "Glwidget\n";
    version=MYVERSION;
    xangle= yangle= zangle=0.0;
    scale = 1.5;
    object =0;
    CameraPos.x = CameraPos.y = CameraPos.z = 5;
    Rotating = false;
    Scaling = false;
}

void GLWidget::clear()
{
     updateGL();
}

void GLWidget::initializeGL()
{
    int i;
    QImage buf(256, 256, QImage::Format_RGB32);

GLfloat whiteDir[4] = {2.0, 2.0, 2.0, 1.0};
GLfloat whiteAmb[4] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightPos[4] = {30.0, 30.0, 30.0, 1.0};

glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteAmb);

glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteDir);
glMaterialfv(GL_FRONT, GL_SPECULAR, whiteDir);
glMaterialf(GL_FRONT, GL_SHININESS, 20.0);

glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDir);		// enable diffuse
glLightfv(GL_LIGHT0, GL_SPECULAR, whiteDir);	// enable specular
glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

glShadeModel( GL_SMOOTH );

// Set up the textures
//QImage buf( 256, 256, 32 );
for (i=0; i<6; i++) {
makeSpots(i, &buf);
tex[i] = QGLWidget::convertToGLFormat( buf );  // flipped 32bit RGBA
}

glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
glEnable( GL_TEXTURE_2D );

// Set up various other stuff
glClearColor( 0.5, 1.0, 0.75, 0.0 ); // Let OpenGL clear to black
glEnable( GL_CULL_FACE );  	// don't need Z testing for convex objects
glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

// Make the object display list
object = makeDice( );	// Generate an OpenGL display list
}

void GLWidget::redraw()
{
        updateGL();
}

void GLWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );

    glLoadIdentity();
    //gluLookAt(CameraPos.x,
    //          CameraPos.y,
   //           CameraPos.z, 0, 0, 0, 0.0, 1.0, 0.0);

    GLdouble input_matrix[16];
    GLdouble * transform_matrix = constructMatrix(input_matrix);

    glMultMatrixd(transform_matrix);
    //glTranslated(-eyex, -eyey, -eyez);
    glTranslated((-1)*CameraPos.x, (-1)*CameraPos.y, (-1)*CameraPos.z);

    glRotatef( xangle, 1.0, 0.0, 0.0 );
    glRotatef( yangle, 0.0, 1.0, 0.0 );
    glRotatef( zangle, 0.0, 0.0, 1.0 );

    drawAxes();
    drawGrid();

    glCallList( object );

}

GLdouble * GLWidget::constructMatrix(GLdouble matrix[])
{
    //Init and normaliz F vector
    Vector3d F;
    F.x = (0 - CameraPos.x);
    F.y = (0 - CameraPos.y);
    F.z = (0 - CameraPos.z);
    F = normalize(F);

    //Init and normalize Up vector
    Vector3d Up;
    Up.x = 0.0;
    Up.y = 1.0;
    Up.z = 0.0;
    Up = normalize(Up);

    //s = F x Up
    Vector3d S;
    S = cross(F, Up);

    //Normalize s
    S = normalize(S);

    //u = s X f
    Vector3d U;
    U = cross(S, F);

    //construct M from S, U, and F
    matrix[0] = S.x;
    matrix[1] = U.x;
    matrix[2] = (-1)*F.x;
    matrix[3] = 0;
    matrix[4] = S.y;
    matrix[5] = U.y;
    matrix[6] = (-1)*F.y;
    matrix[7] = 0;
    matrix[8] = S.z;
    matrix[9] = U.z;
    matrix[10] = (-1)*F.z;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;

    return matrix;
}

Vector3d GLWidget::normalize (Vector3d input){
    double length = sqrt(input.x*input.x + input.y*input.y + input.z*input.z);
    input.x /= length;
    input.y /= length;
    input.z /= length;
    return input;
}

Vector3d GLWidget::cross (Vector3d input1, Vector3d input2){
    Vector3d result;
    result.x = (input1.y*input2.z - input1.z*input2.y);
    result.y = (input1.z*input2.x - input1.x*input2.z);
    result.z = (input1.x*input2.y - input1.y*input2.x);
    return result;
}

/* 2D */
void GLWidget::resizeGL( int w, int h )
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 1500.0 );
    glMatrixMode( GL_MODELVIEW );
}

void GLWidget::about()
{
    QString vnum;
    QString mess, notes;
    QString title="QtOpenGl-1 ";

    vnum.setNum (version );
    mess="Simple OpenGl and Qt by Brian Wyvill Release Version: ";
    mess = mess+vnum;
    notes = "\n\n News: No News.";
    mess = mess+notes;
    QMessageBox::information( this, title, mess, QMessageBox::Ok );
}

void GLWidget::help()
{
    QString vnum;
    QString mess, notes;
    QString title="Flowsnake ";

    vnum.setNum ( version );
    mess="Simple Interface to openGl and Qt by Brian Wyvill Release Version: ";
    mess = mess+vnum;
    notes = "\nThis version driven from the GLwidget. \n \
            Just draws a circle that's all \n   ";
    mess = mess+notes;
    QMessageBox::information( this, title, mess, QMessageBox::Ok );
}


void GLWidget::drawAxes()
{
    // draw axes
    glBegin(GL_LINES);
    // draw line for x axis
    glLineWidth(10);
    glColor3f(1.0f, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(10.0, 0.0, 0.0);
    // draw line for y axis
    glColor3f(0.0, 1.0f, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 10.0, 0.0);
    // draw line for Z axis
    glColor3f(0.0, 0.0, 1.0f);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 10.0);
    glEnd();
}

void GLWidget::drawGrid()
{
    //draw grid
    glColor3f(.3,.3,.3);
    glBegin(GL_QUADS);
    glVertex3f( -10,-0.001, -10);
    glVertex3f( -10,-0.001,10);
    glVertex3f(10,-0.001,10);
    glVertex3f(10,-0.001, -10);
    glEnd();

    glBegin(GL_LINES);
    for(int i=-10;i<=10;i++) {
        if (i==0) { glColor3f(.6,.3,.3); } else { glColor3f(.25,.25,.25); };
        glVertex3f(i,0,-10);
        glVertex3f(i,0,10);
        if (i==0) { glColor3f(.3,.3,.6); } else { glColor3f(.25,.25,.25); };
        glVertex3f(-10,0,i);
        glVertex3f(10,0,i);
    };
    glEnd();
}

void GLWidget::initLight()
{
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 128.0 };
   GLfloat light_position[] = { 0.2, 0.2, 0.9, 0.0 };
   GLfloat light_ambient[] = { 0.05, 0.05, 0.05, 1.0 };
   GLfloat light_specular[] = { 0.99, 0.99, 0.99, 1.0 };
   GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };

   glShadeModel (GL_SMOOTH);

// MATERIAL
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

// LIGHT0
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

}

GLuint GLWidget::makeDice( )
{
    GLuint list;
    float w = 0.8;

    list = glGenLists( 1 );

    glNewList( list, GL_COMPILE );


    // one
    drawFace(0,  w);

    // six
    glPushMatrix();
    glRotatef( 180.0, 1.0, 0.0, 0.0 );
    drawFace(5, w);
    glPopMatrix();

    // four on left
    glPushMatrix();
    glRotatef( -90.0, 0.0, 1.0, 0.0 );
    drawFace(3, w);
    glPopMatrix();

    // three on right
    glPushMatrix();
    glRotatef( 90.0, 0.0, 1.0, 0.0 );
    drawFace(2, w);
    glPopMatrix();

    // two
    glPushMatrix();
    glRotatef( 90.0, 1.0, 0.0, 0.0 );
    drawFace(1, w);
    glPopMatrix();

    // five
    glPushMatrix();
    glRotatef( -90.0, 1.0, 0.0, 0.0 );
    drawFace(4, w);
    glPopMatrix();

    glEndList();

    return list;
}

void GLWidget::drawFace( int tim, float w)
{
    glTexImage2D( GL_TEXTURE_2D, 0, 3, tex[tim].width(), tex[tim].height(), 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, tex[tim].bits() );

    glBegin( GL_POLYGON );
    glTexCoord2f(0.0, 0.0);
    glVertex3f(  -w,  -w, w );
    glTexCoord2f(0.0, 1.0);
    glVertex3f(   w,  -w, w );
    glTexCoord2f(1.0, 1.0);
    glVertex3f(   w,   w, w );
    glTexCoord2f(1.0, 0.0);
    glVertex3f(  -w,   w, w );
    glEnd();

}


void GLWidget::makeSpots(int tim, QImage *buf)
{
  int r=255, g=0, b=0;
  int rad=25;
  int w,h,i,j;

  w=buf->width();
  h=buf->height();

  // set red
  for (i=0; i<buf->width(); i++)
    for (j=0; j<buf->height(); j++)
      buf->setPixel(i,j, qRgb(r, g, b));

  switch(tim) {
  case 0: // value 1
    drawCircle(rad, w/2, h/2, buf);
    break;

  case 1: // value 2
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    break;


  case 2: // value 3
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, w/2, h/2, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    break;

  case 3: // value 4
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, w/4, 3*h/4, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    drawCircle(rad, 3*w/4, h/4, buf);
    break;

  case 4: // value 5
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, w/4, 3*h/4, buf);
    drawCircle(rad, w/2, h/2, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    drawCircle(rad, 3*w/4, h/4, buf);
    break;

  case 5: // value 6
    drawCircle(rad, w/4, h/4, buf);
    drawCircle(rad, w/4, h/2, buf);
    drawCircle(rad, w/4, 3*h/4, buf);

    drawCircle(rad, 3*w/4, h/4, buf);
    drawCircle(rad, 3*w/4, h/2, buf);
    drawCircle(rad, 3*w/4, 3*h/4, buf);
    break;

  default: std::cerr << " big oopsy \n";
    break;

  }
}

void GLWidget::drawCircle(int radius, int xcen, int ycen,  QImage *buf)
{
  int i,j,r2;

  r2=radius*radius;

  for(i=xcen-radius; i<xcen+radius; i++)
    for(j=ycen-radius; j<ycen+radius; j++) {
      if  ( (i-xcen)*(i-xcen) + (j-ycen)*(j-ycen) < r2)
        buf->setPixel(i,j,qRgb(255, 255, 255));
   }
}

// communication with the window widget
void GLWidget::rotx(int a)
{
        xangle =  (double)a;
        updateGL();
}
void GLWidget::roty(int a)
{
        yangle =  (double)a;
        updateGL();
}

void GLWidget::rotz(int a)
{
        zangle =  (double)a;
        updateGL();
}


void GLWidget::setxFrom(int a)
{
    CameraPos.x=a;
    updateGL();
}

void GLWidget::setyFrom(int a)
{
    CameraPos.y=a;
    updateGL();
}
void GLWidget::setzFrom(int a)
{
    CameraPos.z=a;
    updateGL();
}

//****************************************************
// mouse routines for camera control to be implemented
//****************************************************

void GLWidget::mousePressEvent( QMouseEvent *e )
{
    //TODO:When a button is pressed!
    //*******delete
    if (e->button() == Qt::LeftButton)
    {
        lastMousePoint = e->pos();
        Rotating = true;
    }

    if (e->button() == Qt::RightButton)
    {
        lastMousePoint = e->pos();
        Scaling = true;
    }
    //*********
}

void GLWidget::mouseReleaseEvent( QMouseEvent *e)
{
    //TODO:When some pressed mouse button is release!
    //********delete
    if (e->button() == Qt::LeftButton && Rotating)
    {
        DoRotate(e->pos(), lastMousePoint);
        Rotating = false;
    }

    if (e->button() == Qt::RightButton && Scaling)
    {
        DoScale(e->pos(), lastMousePoint);
        Scaling = false;
    }

    updateGL();
    //************
}

void GLWidget::mouseMoveEvent( QMouseEvent *e )
{
    //TODO: when the mouse is moved!
    //*******delete
    if ((e->buttons() & Qt::LeftButton) && Rotating)
    {
        DoRotate(e->pos(), lastMousePoint);
        lastMousePoint = e->pos();
    }

    if ((e->buttons() & Qt::RightButton) && Scaling)
    {
        DoScale(e->pos(), lastMousePoint);
        lastMousePoint = e->pos();
    }

    updateGL();
    //*********
}



void GLWidget::RotateY(Vector3d * pVec, double rad)
{
    //TODO: rotate pVec around Z axis by the angle rad
    //******delete
    double cosPhi = (double)cos(rad);
    double sinPhi = (double)sin(rad);

    Matrix33d m;
    m.M11 = cosPhi;
    m.M13 = sinPhi;
    m.M22 = 1;
    m.M31 = - sinPhi;
    m.M33 = cosPhi;
    *pVec = MultiplyMatrix33Vec3(*pVec, m);
    //************
}

void GLWidget::RotateZ(Vector3d * pVec, double rad)
{
    //TODO: rotate pVec around Z axis by the angle rad
    //******delete
    double cosPhi = (double)cos(rad);
    double sinPhi = (double)sin(rad);

    Matrix33d m;
    m.M11 = cosPhi;
    m.M12 = - sinPhi;
    m.M21 = sinPhi;
    m.M22 = cosPhi;
    m.M33 = 1;
    *pVec = MultiplyMatrix33Vec3(*pVec, m);
    //*********
}

void GLWidget::DoRotate(QPoint desc, QPoint orig)
{
    //TODO: adjust the camera position so the viewport is rotated
    //********delete
    double YRot = (desc.x() - orig.x()) * RadPerPixel;
    double ZRot = (desc.y() - orig.y()) * RadPerPixel;

    RotateY(&CameraPos, YRot);
    RotateZ(&CameraPos, ZRot);
    //*********
}

void GLWidget::DoScale(QPoint desc, QPoint orig)
{
    //TODO: adjust the camera position so the viewport is scaled.
    //******delete
    double length = sqrt(CameraPos.x * CameraPos.x + CameraPos.y * CameraPos.y);
    double newLength = length + (desc.y() - orig.y()) * MovePerPixel;
    if (newLength > lim)
    {
        double ratio = newLength / length;
        CameraPos.x = CameraPos.x * ratio;
        CameraPos.y = CameraPos.y * ratio;
        CameraPos.z = CameraPos.z * ratio;
    }
    //*******
}
