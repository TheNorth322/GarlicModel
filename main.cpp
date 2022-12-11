#define _CRT_SECURE_NO_WARNINGS
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

float r = 5.0f;
const char *TEXTURE_PATH = "1.bmp";
// Смещение фигуры
float cx = 0, cy = 0, cz = 0;
// Смещение источника света
float lx = 0, ly = 0, lz = 0;

typedef enum {
  Wired = 0,
  Color,
  WiredAndColor,
  Perspective,
  Aksonometric,
  Textured
} Mode;

Mode mode;

GLfloat m[3];
static GLuint texture;

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

float rad(float degree) { return degree * M_PI / 180; }

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

void drawWiredGarlic() {
  Point point;

  glPolygonMode(GL_FRONT, GL_FILL);
  for (int fi = 0; fi <= 360; fi += 10) {
    glBegin(GL_LINE_STRIP);
    for (int teta = -90; teta <= 90; teta += 5) {
      glColor3f(1, 0, 0);
      calcGarlicCoord(point, r, fi, teta);
      glVertex3f(point.x + cx, point.y + cy, point.z + cz);
    }
    glEnd();
  }

  for (int teta = -90; teta <= 90; teta += 5) {
    glBegin(GL_LINE_STRIP);
    for (int fi = 0; fi <= 360; fi += 10) {
      glColor3f(1, 0, 0);
      calcGarlicCoord(point, r, fi, teta);
      glVertex3f(point.x + cx, point.y + cy, point.z + cz);
    }
    glEnd();
  }
}

void calcNormals(GLfloat a[], GLfloat b[], Point &point_1, Point &point_2,
                 Point &point_3) {
  a[0] = point_2.x - point_1.x;
  a[1] = point_2.y - point_1.y;
  a[2] = point_2.z - point_1.z;
  b[0] = point_3.x - point_1.x;
  b[1] = point_3.y - point_1.y;
  b[2] = point_3.z - point_1.z;
  m[0] = a[1] * b[2] - a[2] * b[1];
  m[1] = a[0] * b[2] - a[2] * b[0];
  m[2] = a[0] * b[1] - a[1] * b[0];
}

//Создание полигонов по точкам
void drawPolygonGarlic(int type, float dB, float dL) {
  Point point_1, point_2, point_3, point_4;
  GLfloat a[3], b[3];
  int i, j;
  GLfloat ix = 0, iy = 0, iwx = 1 / (180 / dB), iwy = 1 / (360 / dL);

  for (int theta = -90; theta < 90; theta += dB) {
    for (int phi = 0; phi <= 360; phi += dL) {

      calcGarlicCoord(point_1, r, phi, theta);
      calcGarlicCoord(point_2, r, phi, theta + dB);
      calcGarlicCoord(point_3, r, phi + dL, theta + dB);
      calcGarlicCoord(point_4, r, phi + dL, theta);

      switch (type) {
      case 1:
        glBegin(GL_POLYGON);

        glNormal3fv(m);
        glTexCoord2d(ix, iy);
        glVertex3d(point_1.x + cx, point_1.y + cy, point_1.z + cz);

        glNormal3fv(m);
        glTexCoord2d(ix, iy + iwy);
        glVertex3d(point_2.x + cx, point_2.y + cy, point_2.z + cz);

        glNormal3fv(m);
        glTexCoord2d(ix + iwx, iy + iwy);
        glVertex3d(point_3.x + cx, point_3.y + cy, point_3.z + cz);

        glNormal3fv(m);
        glTexCoord2d(ix + iwx, iy);
        glVertex3d(point_4.x + cx, point_4.y + cy, point_4.z + cz);

        glEnd();
        break;

      case 2:
        calcNormals(a, b, point_1, point_2, point_3);
        glColor3f(1.0, 0.4, 0.1);
        glNormal3fv(m);
        glBegin(GL_QUADS);

        glNormal3fv(m);
        glVertex3d(point_1.x + cx, point_1.y + cy, point_1.z + cz);

        glNormal3fv(m);
        glVertex3d(point_2.x + cx, point_2.y + cy, point_2.z + cz);

        glNormal3fv(m);
        glVertex3d(point_3.x + cx, point_3.y + cy, point_3.z + cz);

        glNormal3fv(m);
        glVertex3d(point_4.x + cx, point_4.y + cy, point_4.z + cz);
        glEnd();
        break;

      case 3:
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_POLYGON);

        glVertex3d(point_1.x + cx, point_1.y + cy, point_1.z + cz);
        glVertex3d(point_2.x + cx, point_2.y + cy, point_2.z + cz);
        glVertex3d(point_3.x + cx, point_3.y + cy, point_3.z + cz);
        glVertex3d(point_4.x + cx, point_4.y + cy, point_4.z + cz);

        glEnd();
        break;
      default:
        break;
      }
      ix += iwx;
    }
    iy += iwy;
    ix = 0;
  }
}
void display(void) {
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0, 0.0, 0.0);
  glPushMatrix();
  GLfloat mat_specular[] = {0.0, 0.0, 1.0, 1.0};
  GLfloat mat_shininess[] = {50.0};
  GLfloat lpos0[] = {-5.0 + lx, -5.0 + ly, -5.0 + lz, 1.0};
  glShadeModel(GL_SMOOTH);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, lpos0);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);

  switch (mode) {
  // Сетка
  case Wired: {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15, 15, -15, 15, -25, 25);
    gluLookAt(0, 5, 0, 0, 1, 0, 0, 0, 1);
    glDisable(GL_LIGHTING);
    drawWiredGarlic();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    break;
  }

  // Цвет
  case Color: {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15, 15, -15, 15, -25, 25);
    gluLookAt(0, 5, 0, 0, 1, 0, 0, 0, 1);
    glEnable(GL_NORMALIZE);
    drawPolygonGarlic(2, 4, 4);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    break;
  }

  // Цвет и каркас
  case WiredAndColor: {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15, 15, -15, 15, -25, 25);
    gluLookAt(0, 5, 0, 0, 1, 0, 0, 0, 1);
    glEnable(GL_NORMALIZE);
    drawPolygonGarlic(2, 4, 4);
    drawWiredGarlic();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    break;
  }

  //Перспективная проекция
  case Perspective: {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50, 1, 1, 400);
    gluLookAt(0, 30, 0, 0, 0, 0, 0, 0, 1);
    glRotatef(20, 0, 1, 0);
    glRotatef(-27, 1, 0, 0);
    glRotatef(37, 0, 0, 1);
    drawWiredGarlic();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    break;
  }

  //Аксонометрическая проекция
  case Aksonometric: {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15, 15, -15, 15, 25, -25);
    //Описывает параллельный объем отсечения.
    //Такая проекция означает, что объекты, удаленные от пользователя, не
    //кажутся меньше
    gluLookAt(0, 5, 0, 0, 0, 0, 0, 0, 1);
    glRotatef(45, 0, 0, 1);
    glRotatef(30, 1, 0, 0);
    drawWiredGarlic();
    glDisable(GL_DEPTH_TEST);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    break;
  }

  case Textured: {
    glDisable(GL_LIGHTING);
    gluPerspective(50, 1, 1, 400);
    gluLookAt(0, 30, 0, 0, 0, 0, 0, 0, 1);
    glRotatef(20, 0, 1, 0);
    glRotatef(-27, 1, 0, 0);
    glRotatef(37, 0, 0, 1);

    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glColor3d(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, texture);
    drawPolygonGarlic(1, 4, 4);
    glDisable(GL_TEXTURE_2D);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    break;
  }
  default: {
    glDisable(GL_LIGHTING);
    mode = Wired;
    glLoadIdentity();
    break;
  }
  }
  glPopMatrix();
  glFlush();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 27:
    exit(0);
    break;

  // Движение фигуры в пространстве
  case 'w':
    cz += 0.1;
    break;
  case 's':
    cz -= 0.1;
    break;
  case 'a':
    cx += 0.1;
    break;
  case 'd':
    cx -= 0.1;
    break;
  case 'q':
    cy -= 0.1;
    break;
  case 'e':
    cy += 0.1;
    break;

  // Движение источника света
  case 'W':
    lz += 0.1;
    break;
  case 'S':
    lz -= 0.1;
    break;
  case 'A':
    lx += 0.1;
    break;
  case 'D':
    lx -= 0.1;
    break;
  case 'Q':
    ly += 0.1;
    break;
  case 'E':
    ly -= 0.1;
    break;

  // Сброс движения
  case 'x':
    cx = 0;
    cy = 0;
    cz = 0;
    break;

  // Выбор режима просмотра
  case '0':
    mode = Wired;
    break;
  case '1':
    mode = Color;
    break;
  case '2':
    mode = WiredAndColor;
    break;
  case '3':
    mode = Perspective;
    break;
  case '4':
    mode = Aksonometric;
    break;
  case '5':
    mode = Textured;
    break;
  }
  glutPostRedisplay();
}

void processSpecialKeys(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_PAGE_UP:
    r -= 0.5f;
    break;
  case GLUT_KEY_PAGE_DOWN:
    r += 0.5f;
    break;
  }
  glutPostRedisplay();
}

// Загрузка текстуры по названию файла, поиск в корне программы
GLuint LoadTexture(const char *filename) {
  GLuint texture;
  int width, height;
  unsigned char *data;
  FILE *file;

  file = fopen(filename, "rb");

  if (file == NULL)
    return 0;
  width = 1024;
  height = 512;
  data = (unsigned char *)malloc(width * height * 3);
  fread(data, width * height * 3, 1, file);
  fclose(file);

  for (int i = 0; i < width * height; ++i) {
    int index = i * 3;
    unsigned char B, R;
    B = data[index];
    R = data[index + 2];

    data[index] = R;
    data[index + 2] = B;
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_NEAREST);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE,
                    data);
  free(data);

  return texture;
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(700, 700);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("РГР Горшков АВТ-113. Вариант 12: Чеснок");
  glShadeModel(GL_SMOOTH);
  texture = LoadTexture(TEXTURE_PATH);
  glutDisplayFunc(display);
  glutSpecialFunc(processSpecialKeys);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
