#include <GL/glut.h>

void displayMe(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    //starting to draw the square
    glBegin(GL_POLYGON);

    glColor3f(1.0,0.0,0.0);
        glVertex3f(0.0, 0.0, 0.0);
	glColor3f(0.0,1.0,0.0);
        glVertex3f(0.5, 0.0, 0.0);
	glColor3f(0.0,0.0,1.0);
	glVertex3f(0.5, 0.5, 0.0);
	glColor3f(1.0,0.0,1.0);
        glVertex3f(0.0, 0.5, 0.0);
    glEnd();

    glFlush();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(300, 300);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hello world :D");
    glutDisplayFunc(displayMe);
    glutMainLoop();
    return 0;
}
