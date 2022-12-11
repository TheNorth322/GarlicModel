#include <GL/glut.h>
#include <cmath>
#include <iostream>

float degree = 1;
float phi = 0.1;
float R = 3;
float delta = 0.05;
const int dimensions = 400;

GLfloat camX, camY, camZ = 0;
GLfloat lookX, lookY, lookZ = 0;

struct Point {
  float x;
  float y;
  float z;

  Point() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
  }

  Point(float _x, float _y, float _z) {
    this->x = _x;
    this->y = _y;
    this->z = _z;
  }
};

void cameraFunc() {
  camX = R * sin(degree) * cos(phi);
  camY = R * sin(degree) * sin(phi);
  camZ = R * cos(degree);

  gluLookAt(camX, camY, camZ, 0, 0, lookZ, 0, 0, 1);
}

float rad(float degree) { return degree * M_PI / 180; }
// B -> teta, L - fi
void calcSphereCoord(Point &point, double r, float fi, float teta) {
  point.x = r * cos(rad(teta)) * sin(rad(fi));
  point.y = r * cos(rad(teta)) * cos(rad(fi));
  point.z = r * sin(rad(teta));
}

void calcGarlicCoord(Point &point, double r, float fi, float teta) {
  calcSphereCoord(point, r, fi, teta);

  point.x *= (1 + 0.5 * fabs(sin(rad(2 * fi))));
  point.y *= (1 + 0.5 * fabs(sin(rad(2 * fi))));
  point.z = (teta > 0) ? point.z + r * powf(teta / 90, 5.0f) : point.z;
}

void lighting() {
  GLfloat ambient[] = {0.7, 0.0, 0.0, 1.0};
  GLfloat diffuse[] = {0.9, 0.0, 0.0, 1.0};
  GLfloat specular[] = {1.0, 0.0, 1.0, 1.0};
  GLfloat position[] = {2.0, 3.0, 10.0, 1.0};

  GLfloat Mambient[] = {0.25, 0.25, 0.25, 1.0};
  GLfloat Mdiffuse[] = {0.4, 0.4, 0.4, 1.0};
  GLfloat Mspecular[] = {0.7, 0.7, 0.7, 1.0};

  glMaterialfv(GL_FRONT, GL_SPECULAR, Mambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, Mdiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, Mspecular);

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

void drawWiredGarlic() {
  Point point;
  int r = 2;

  glPolygonMode(GL_FRONT, GL_FILL);
  for (int fi = 0; fi <= 360; fi += 10) {
    glBegin(GL_LINE_STRIP);
    for (int teta = -90; teta <= 90; teta += 5) {
      glColor3f(1, 0, 0);
      calcGarlicCoord(point, r, fi, teta);
      glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
  }

  for (int teta = -90; teta <= 90; teta += 5) {
    glBegin(GL_LINE_STRIP);
    for (int fi = 0; fi <= 360; fi += 10) {
      glColor3f(1, 0, 0);
      calcGarlicCoord(point, r, fi, teta);
      glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
  }
}

void drawSolidGarlic() {
  Point point;
  int r = 0;

  glPolygonMode(GL_FRONT, GL_FILL);
  for (int fi = 0; fi <= 360; fi += 1) {
    glBegin(GL_QUAD_STRIP);
    for (float teta = -0.5; teta <= 0.5; teta += 0.1) {
      glColor3f(1, 0, 0);
      calcGarlicCoord(point, r, fi, teta);
      glVertex3f(point.x, point.y, point.z);

      glColor3f(1, 0, 0);
      calcGarlicCoord(point, r, fi + 1, teta);
      glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
  }

  /*for (int teta = -90; teta <= 90; teta += 1) {
    glBegin(GL_QUAD_STRIP);
    for (float fi = 0; fi <= 360; fi += 0.1) {
      glColor3f(1, 0, 0);
      calcGarlicCoord(point, r, fi, teta);
      glVertex3f(point.x, point.y, point.z);
    }
    glEnd();
  }*/
}
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glLoadIdentity();

  cameraFunc();
  drawWiredGarlic();

  glutSwapBuffers();
}

void init(void) {
  glShadeModel(GL_FLAT);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  lighting();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 1, 100);
  glMatrixMode(GL_MODELVIEW);
}

void reshape(int width, int height) {
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glShadeModel(GL_SMOOTH);
}

void processSpecialKeys(int key, int x, int y) {

  switch (key) {
  case GLUT_KEY_RIGHT:
    phi += delta;
    break;
  case GLUT_KEY_LEFT:
    phi -= delta;
    break;
  case GLUT_KEY_UP:
    degree += delta;
    break;
  case GLUT_KEY_DOWN:
    degree -= delta;
    break;
  case GLUT_KEY_PAGE_UP:
    R -= 0.5f;
    break;
  case GLUT_KEY_PAGE_DOWN:
    R += 0.5f;
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glEnable(GL_LIGHT0);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glEnable(GL_DEPTH_TEST);
  glutInitWindowSize(800, 800);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Garlic");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(processSpecialKeys);
  glutMainLoop();
  return 0;
}
