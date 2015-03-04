//-------------------------------------------------------------------------------------------
//  University of Bath Computer Science Department
//	FrameWork for OpenGL application under QT
//  Course title: Computer Graphics CM20219
//-------------------------------------------------------------------------------------------
//These two lines are header guardians against multiple includes
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QProgressBar>
#include "foundation.h"
#include <QtGui>
#include <QtOpenGL>
#include <glu.h>
#include "version.h"


const int maxw=1024;
const int maxh=1024;
//This is our OpenGL Component we built it on top of QGLWidget
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    //Constructor for GLWidget
    GLWidget(QWidget *parent = 0);

    //Destructor for GLWidget
    ~GLWidget();

    void clear();
    void about();
    void help();
    void rotx(int);
    void roty(int);
    void rotz(int);
    void setxFrom(int a);
    void setyFrom(int a);
    void setzFrom(int a);

protected:
    //Initialize the OpenGL Graphics Engine
    void initializeGL();

    //All our painting stuff are here
    void paintGL();

    //When user resizes main window, the scrollArea will be resized and it will call this function from
    //its attached GLWidget
    void resizeGL(int width, int height);
    //Handle mouse
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

private:
    void startup();
    int winw, winh, button,imx,imy;
    int version;
    void dopan(int x, int y, bool click);
    void zoomWheel(int z);
    QImage *myimage;
    QImage timage;
    void drawFace(float w, float h);
    void displayImage();
    void drawCircle(double radius, double xcen, double ycen, bool line, int nseg);
    // for display list
    int object;
    // for the cube
    double xangle, yangle, zangle, scale;
    QImage tex[6]; // one for each face

    // Functions
    void redraw();
    void makeSpots(int tim, QImage *buf);
    void drawCircle(int radius, int xcen, int ycen,  QImage *buf);
    void drawFace( int tim, float w);
    GLuint makeDice( );
    void initLight();
    void drawAxes();
    void drawGrid();
    GLdouble * constructMatrix(GLdouble matrix[]);
    Vector3d normalize (Vector3d input);
    Vector3d cross (Vector3d input1, Vector3d input2);

    // Rotation Calculation

    Vector3d CameraPos;
    QPoint lastMousePoint;

    bool Rotating;
    bool Scaling;
    void RotateY(Vector3d * vec_in, double rad);
    void RotateZ(Vector3d * vec_in, double rad);
    void DoRotate(QPoint desc, QPoint orig);
    void DoScale(QPoint desc, QPoint orig);
};


#endif
