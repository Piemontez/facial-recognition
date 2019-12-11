#include "3dviewer.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <math.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

static int imageType = 0;

static int zoom = -0;
static int width = 960;
static int height = 640;
static int window = 0;

static GLfloat xv = 0.f;
static GLfloat yv = 0.f;
static GLfloat zv = 0.f;
static int xvOrient = 1;
static int yvOrient = 1;
static int zvOrient = 1;

static cv::Mat pointCloudGlobal;

void multiplyMatrix(float (&inputMatrix)[4][1],
                    float (&outputMatrix)[4][1],
                    float (&rotationMatrix)[4][4]);
void setUpRotationMatrix(float (&rotationMatrix)[4][4],
                         float angle, float u, float v, float w);
void showPoint(float **outputMatrix);

void Viewer3D::view(cv::Mat depthImg)
{
    if (depthImg.type() == CV_32FC1) {
        cv::Mat tmp;
        depthImg.convertTo(tmp, CV_8UC1, 255);
        pointCloudGlobal = tmp;
    } else
        pointCloudGlobal = depthImg;

    int argc = 0;
    char **argv = 0;
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);

    window = glutCreateWindow("RAP3DF");
    glClearColor(1.f, 1.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(50.0, 1.0, 900.0, 11000.0);

    glutDisplayFunc([] () {
        int w = 119;
        int h = 149;

        std::vector<uint16_t>::size_type i, j;
        //std::vector<uint16_t> file = files[filePos];
        //std::vector<uint16_t> irFile = irFiles[filePos];

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, width, height);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        {
            glRotatef(180, 1.f, 0.f, 0.f); // Rotate our camera on the x-axis (looking up and down)
            glTranslatef(xv*3, 0, abs(xv * 2));
            glRotatef(-xv, 0.f, 1.f, 0.f);
            glRotatef(-yv, 1.f, 0.f, 0.f);
            glRotatef(-zv, 0.f, 0.f, 1.f);

            glPointSize(5.0f);
            glBegin(GL_POINTS);
//            glBegin(GL_TRIANGLES);

            char min = 0, max = 0, z;
            for (int y = 0; y < (h -1); ++y)
                for (int x = 0; x < (w -1); ++x)
                {
                    i = static_cast<unsigned long>(y * w) + static_cast<unsigned long>(x);
                    char val = pointCloudGlobal.at<char>(x,y);

                    if (val) continue;

                    if (max < val) {
                        if (max == 0)
                            min = val;
                        max = val;
                    }
                    if (min > val)
                        min = val;
                }

            for (int y = 0; y < (h -1); ++y)
                for (int x = 0; x < (w -1); ++x)
                {
                    uint16_t val = pointCloudGlobal.at<char>(y, x);
                    if (val == 0) continue;

                    if (imageType == 0) {
                        glColor3f((val - min) * 3.0 / 255.0,
                                  (val - min) * 2.0 / 255.0,
                                  (val - min) * 8.0 / 255.0);
                    } else {
                        //glColor3ub(static_cast<unsigned char>(irFile[i]), 0, 0);
                    }

                    {

                        uint16_t valJ = pointCloudGlobal.at<char>(y,x); z = !valJ ? val : valJ;
                        glVertex3f(x-(w/2), y-(h/2), z * 0.9 + zoom);
//                        valJ = pointCloudGlobal.at<char>(y+1,x); z = !valJ ? val : valJ;
//                        glVertex3f(x+1-(w/2), y-(h/2), z * 0.9 + zoom);
//                        valJ = pointCloudGlobal.at<char>(y,x+w); z = !valJ ? val : valJ;
//                        glVertex3f(x-(w/2), y+1-(h/2), z * 0.9 + zoom);

                    }
                    {
//                        uint16_t valJ;
//                        valJ = pointCloudGlobal.at<char>(y+1,x); z = !valJ ? val : valJ;
//                        glVertex3f(x+1-(w/2), y-(h/2), z * 0.9+ zoom);
//                        valJ = pointCloudGlobal.at<char>(y,x+w); z = !valJ ? val : valJ;
//                        glVertex3f(x-(w/2), y+1-(h/2), z * 0.9+ zoom);
//                        valJ = pointCloudGlobal.at<char>(y+1,x+w); z = !valJ ? val : valJ;
//                        glVertex3f(x+1-(w/2), y+1-(h/2), z * 0.9 + zoom);
                    }
                }
            }

            glEnd();

        glFlush();
        glutSwapBuffers();
    });

//    glutIdleFunc([] () {
//        glutPostRedisplay();
//    });

    glutReshapeFunc([] (int w, int h) {
        width = w;
        height = h;

        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(50.0, static_cast<double>(width) / height, 1.0, 12000.0);
    });


    glutKeyboardFunc([] (unsigned char key, int /*x*/, int /*y*/) {
        int w = 119;
        int h = 149;

        if (key == 0x1B) {// ESC
            glutDestroyWindow(window);
            exit(0);
        }
        switch (key)
        {
            case  'Q':
            case  'q':
                if (zv > (w/2)) {
                    zvOrient = -1;
                }
                if (zv < -(w/2)) {
                    zvOrient = 1;
                }
                zv += zvOrient;

                glutPostRedisplay();
                break;
            case  'e':
            case  'E':
                if (yv > (w/2)) {
                    yvOrient = -1;
                }
                if (yv < -(w/2)) {
                    yvOrient = 1;
                }
                yv += yvOrient;

                glutPostRedisplay();
                break;
            case  'w':
            case  'W':
            {
                if (xv > (w/2)) {
                    xvOrient = -1;
                }
                if (xv < -(w/2)) {
                    xvOrient = 1;
                }
                xv += xvOrient;

                float rotationMatrix[4][4];
                setUpRotationMatrix(rotationMatrix, 10.0, 1.0, 0, 0);

                int w = 119;
                int h = 149;
                std::vector<uint16_t>::size_type i, j;

                for (int y = 0; y < (h -1); ++y)
                {
                    for (int x = 0; x < (w -1); ++x)
                    {
                        i = static_cast<unsigned long>(y * w) + static_cast<unsigned long>(x);

                        uint16_t val = pointCloudGlobal.at<char>(x,y);
                        if (!val) continue;

                        float inputMatrix[4][1];
                        inputMatrix[0][0] = x;
                        inputMatrix[1][0] = y;
                        inputMatrix[2][0] = val;
                        inputMatrix[3][0] = 1.0;

                        multiplyMatrix(inputMatrix,
                                       inputMatrix,
                                    rotationMatrix);
                        //showPoint(inputMatrix);
                    }
                }



                glutPostRedisplay();
                break;
            }
            case  'p':
            case  'P':
                imageType = (++imageType) % 3;
                glutPostRedisplay();
                break;
        }
    });

    glutMainLoop();


}


void showPoint(float **outputMatrix) {
    std::cout<<"("<<outputMatrix[0][0]<<","<<outputMatrix[1][0]<<","<<outputMatrix[2][0]<<")"<<std::endl;
}

void multiplyMatrix(float (&inputMatrix)[4][1],
                    float (&outputMatrix)[4][1],
                    float (&rotationMatrix)[4][4])
{
    for(int i = 0; i < 4; i++ ) {
        for(int j = 0; j < 1; j++) {
            outputMatrix[i][j] = 0;
            for(int k = 0; k < 4; k++) {
                outputMatrix[i][j] += rotationMatrix[i][k] * inputMatrix[k][j];
            }
        }
    }
}

void setUpRotationMatrix(float (&rotationMatrix)[4][4],
                         float angle, float u, float v, float w)
{
    float L = (u*u + v * v + w * w);
    angle = angle * 3.14159265359 / 180.0; //converting to radian value
    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;

    rotationMatrix[0][0] = (u2 + (v2 + w2) * cos(angle)) / L;
    rotationMatrix[0][1] = (u * v * (1 - cos(angle)) - w * sqrt(L) * sin(angle)) / L;
    rotationMatrix[0][2] = (u * w * (1 - cos(angle)) + v * sqrt(L) * sin(angle)) / L;
    rotationMatrix[0][3] = 0.0;

    rotationMatrix[1][0] = (u * v * (1 - cos(angle)) + w * sqrt(L) * sin(angle)) / L;
    rotationMatrix[1][1] = (v2 + (u2 + w2) * cos(angle)) / L;
    rotationMatrix[1][2] = (v * w * (1 - cos(angle)) - u * sqrt(L) * sin(angle)) / L;
    rotationMatrix[1][3] = 0.0;

    rotationMatrix[2][0] = (u * w * (1 - cos(angle)) - v * sqrt(L) * sin(angle)) / L;
    rotationMatrix[2][1] = (v * w * (1 - cos(angle)) + u * sqrt(L) * sin(angle)) / L;
    rotationMatrix[2][2] = (w2 + (u2 + v2) * cos(angle)) / L;
    rotationMatrix[2][3] = 0.0;

    rotationMatrix[3][0] = 0.0;
    rotationMatrix[3][1] = 0.0;
    rotationMatrix[3][2] = 0.0;
    rotationMatrix[3][3] = 1.0;
}
