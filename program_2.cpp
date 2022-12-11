#include <iostream>
#include <glut.h>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int dimensions = 400;
const float PI = 3.1415;
float delta = 0.05;
float teta = 1;
float phi = 0.1;
float r = 15;

GLfloat camX, camY, camZ = 0;
GLfloat lookX, lookY, lookZ = 0;

float t = 0;

struct Point
{
    float x;
    float y;
    float z;
    float R = 2;
    float H = 8;
    float r = 1;

    Point(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Point() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    float radToDeg(float radian) {
        return (180 * radian)/ PI ;
    }

    float getModifiedRadius(float h, int l) {
        float a = 2 * h * PI + 0.5 * l * PI / 180;
        return R * (1 + fabs(sin(a)));
    }


    void caclCylinderCoord(float h, float l) {
        float modR = getModifiedRadius(h, l);
        this->x = modR * sin(l * PI / 180);
        this->y = modR * cos(l * PI / 180);
        this->z = H * h;
    }

    void calcTorusCoord(int phi, int omega) { //исправить
        this->x = (R + r * cos(phi)) * sin(omega);
        this->z = (R + r * cos(phi)) * cos(omega);
        this->y = r * sin(phi);
    }

    void calcSphereCoord(float B, float L) {
        B *= PI / 180;
        L *= PI / 180;
        this->x = R * cos(B ) * sin(L);
        this->y = R * cos(B ) * cos(L );
        this->z = R * sin(B );
    }

    void calcWeirdSphereCoord(float B, float L) {
        float b = B * PI / 180;
        float l = L * PI / 180;

        float x = R * cos(b) * sin(l);
        float y = R * cos(b) * cos(l);
        float z = R * sin(b);


        this->x = x * (1 + 0.5 * fabs(sin(2 * l)));
        this->y = y * (1 + 0.5 * fabs(sin(2 * l)));
        if (B > 0 )
            this->z = z + R * powf((float)(B / 90), 5.0f);
        else
            this->z = z;

    }
};

void drawWireSphere() {
    Point p;

    glPolygonMode(GL_FRONT, GL_FILL);
    for (int l = 0; l <= 360; l += 20) {
        glBegin(GL_LINE_STRIP);
        for (int b = -90; b <= 90; b += 5) {
            glColor3f(1, 0, 0);
            p.calcWeirdSphereCoord(b, l);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }
    
    for (int b = -90; b <= 90; b += 5) {
        glBegin(GL_LINE_STRIP);
        for (int l = 0; l <= 360; l += 20) {
            glColor3f(1, 0, 0);
            p.calcWeirdSphereCoord(b, l);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }
}

void drawWireTorus() {
    Point p;

    glPolygonMode(GL_FRONT, GL_LINE);
    for (int omega = 0; omega <= 360; omega += 1) {
        glBegin(GL_QUAD_STRIP);
        for (int phi = 0; phi <= 360; phi += 12) {
            p.calcTorusCoord(phi, omega);
            glColor3f(1, 0, 0);
            glVertex3f(p.x, p.y, p.z);

            p.calcTorusCoord(phi, omega+1);
            glColor3f(0, 0, 0);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }
}

void drawWireCylinder() {
    Point p;

    glPolygonMode(GL_FRONT, GL_LINE);
    for (int l = 0; l <= 360; l += 5) {
        glBegin(GL_LINE_STRIP);
        for (float h = -0.5; h <= 0.5; h += 0.1) {
            p.caclCylinderCoord(h, l);
            glColor3f(h, 0, 1 - h);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }

    for (float h = -0.5; h <= 0.5; h += 0.1) {
        glBegin(GL_LINE_STRIP);
        for (int l = 0; l <= 360; l += 5) {
            p.caclCylinderCoord(h, l);
            glColor3f(h, 0, 1 - h);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }
}

void drawSolidCylinder() {
    Point p;

    glPolygonMode(GL_FRONT, GL_FILL);
    for (int l = 0; l < 360; l += 1) {
        glBegin(GL_QUAD_STRIP);
        for (float h = -0.5; h <= 0.5; h += 0.1) {
            p.caclCylinderCoord(h, l);
            glColor3f(h, 0, 1 - h);
            glVertex3f(p.x, p.y, p.z);
            
            p.caclCylinderCoord(h, l+1);
            glColor3f(h, 0, 1 - h);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }

}

GLuint createTexture(char* path) {
    GLuint texture;
    int width, height, nrChannels;
    unsigned char* data;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    data = stbi_load(path, &width, &height, &nrChannels, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        width, height, 0,
        GL_RGB, GL_UNSIGNED_BYTE, data);

    return texture;
}

void cameraFunc() {
    camX = r * sin(teta) * cos(phi);
    camY = r * sin(teta) * sin(phi);
    camZ = r * cos(teta);

    gluLookAt(camX, camY, camZ, 0, 0, lookZ, 0, 0, 1);
    //gluLookAt(0, 0, 0, camX, camY, camZ, 0, 0, 1);
}

void drawCircle() {
    for (int z = 0; z < 10; z++) {

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 50; i++) {
            float a = (float)i / 50.0f * 3.1415f * 2.0f;
            glVertex3f(cos(a), sin(a), z);
        }
        glEnd();
    }
}
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    cameraFunc();

    //char fname[] = "floppa.bmp";
    //GLuint tex = createTexture(fname);

    glColor3f(0.0f, 1.0f, 0.0f);
    
    drawWireSphere();
    //glViewport(0, 0, 200, 400);
    //drawWireCylinder();
    //glViewport(200, 0, 200, 400);
    //drawSolidCylinder();

    glutSwapBuffers();
}

void lighting() {
    GLfloat ambient[] = { 0.7, 0.0, 0.0, 1.0 };
    GLfloat diffuse[] = { 0.9, 0.0, 0.0, 1.0 };
    GLfloat specular[] = { 1.0, 0.0, 1.0, 1.0 };
    GLfloat position[] = { 2.0, 3.0, 10.0, 1.0 };

    GLfloat Mambient[] = { 0.25, 0.25, 0.25, 1.0 };
    GLfloat Mdiffuse[] = { 0.4,0.4, 0.4, 1.0 };
    GLfloat Mspecular[] = { 0.7,0.7, 0.7, 1.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, Mambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, Mdiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Mspecular);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void init(void)
{

    glShadeModel(GL_FLAT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    //проекция
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 100);
    glMatrixMode(GL_MODELVIEW);
}

void reshape(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glShadeModel(GL_SMOOTH);
}

void processSpecialKeys(int key, int x, int y) {

    if (key == GLUT_KEY_RIGHT)
        phi += delta;
    else if (key == GLUT_KEY_LEFT)
        phi -= delta;
    
    if (key == GLUT_KEY_UP) {
        teta += delta;
    }
    else if (key == GLUT_KEY_DOWN) {
        teta -= delta;
    }

    if (key == GLUT_KEY_PAGE_UP) {
        r -= 0.5f;
    }
    else if (key == GLUT_KEY_PAGE_DOWN) {
        r += 0.5f;
    }

    glutPostRedisplay();
}

void processRegularKeys(unsigned char key, int x, int y) {
    if (key == 'e') {
        lookZ += 1.0f;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(dimensions, dimensions);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Расчётно-графическое задание.");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(processSpecialKeys);
    glutKeyboardFunc(processRegularKeys);

    glutMainLoop();
    return 0;
}