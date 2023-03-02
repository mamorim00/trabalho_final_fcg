#include <GL/glut.h>
#include <math.h>
float t = 0.0; // parametro da curva
float gravity = 9.81; // parametro da gravidade
float y = 10.0; // altura inicial do objeto (como podemos definir saindo da mão do jogador?)
void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  // desenhar a curva de Bézier cúbica 
  glColor3f(1.0, 1.0, 1.0);
  glLineWidth(2.0);
  glBegin(GL_LINE_STRIP);
  glVertex2f(0.0, 0.0);
  glVertex2f(1.0, 1.0);
  glVertex2f(2.0, -1.0);
  glVertex2f(3.0, 0.0);
  glEnd();
  // calcular a posição do objeto
  float x = pow(1-t,3)*0.0 + 3*pow(1-t,2)*t*1.0 + 3*(1-t)*pow(t,2)*2.0 + pow(t,3)*3.0;
  y = pow(1-t,3)*0.0 + 3*pow(1-t,2)*t*1.0 + 3*(1-t)*pow(t,2)*(-1.0) + pow(t,3)*0.0;
  y -= 0.5*gravity*pow(t,2); // aplicar a força da gravidade
  // desenhar o objeto
  glColor3f(1.0, 0.0, 0.0);
  glPointSize(10.0);
  glBegin(GL_POINTS);
  glVertex2f(x, y);
  glEnd();
  t += 0.01; // incrementar o parametro da curva
  glutSwapBuffers();
}
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutCreateWindow("Falling object from a cubic Bézier curve");
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, 3.0, -1.5, 1.5); // definir o sistema de coordenandas
  glMatrixMode(GL_MODELVIEW);
  glutMainLoop();
  return 0;
}