#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"

#include "shaderLoader.h" //narzŕdzie do │adowania i kompilowania shaderˇw z pliku


//funkcje algebry liniowej
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective


//Wymiary okna
int screen_width = 640;
int screen_height = 480;

GLfloat h = -2.8;
GLfloat d = 0.025;
int i = -1;

int pozycjaMyszyX; // na ekranie
int pozycjaMyszyY;
int mbutton; // wcisiety klawisz myszy

double kameraX = 0.0; //na boki
double kameraZ = 10.0; //z góry/z dołu
double kameraD = -5.0; //oddalenie
double kameraPredkosc;
double kameraKat = 20;
double kameraPredkoscObrotu;
double poprzednie_kameraX;
double poprzednie_kameraZ;
double poprzednie_kameraD;

double rotation = 0;


//macierze
glm::mat4 MV; //modelview - macierz modelu i świata
glm::mat4 P;  //projection - macierz projekcji, czyli naszej perspektywy


float vertices[] = {
-1, -1, -1,
1, -1, -1,
1, 1, -1,
-1, 1, -1,
-1, -1, 1,
1, -1, 1,
1, 1, 1,
- 1, 1, 1
};

float vertices2[1177 * 3];

GLuint elements[] = { 0, 1, 2, 3,
5, 4, 7, 6,
6, 2, 1, 5,
3, 7, 4, 0,
7, 3, 2, 6,
5, 1, 0, 4 };

GLuint elements2[2256 * 3];
int eSize; 

float angle = 0.0; //kąt do zwiększania, żeby czajnik płynnie się pochylał

//shaders
GLuint programID = 0;

unsigned int VBO,VBO2;
unsigned int ebo, ebo2;
unsigned int VAO[2];


/*###############################################################*/
void mysz(int button, int state, int x, int y)
{
	mbutton = button;
	switch (state)
	{
	case GLUT_UP:
		break;
	case GLUT_DOWN:
		pozycjaMyszyX = x;
		pozycjaMyszyY = y;
		poprzednie_kameraX = kameraX;
		poprzednie_kameraZ = kameraZ;
		poprzednie_kameraD = kameraD;
		break;

	}
}
/*******************************************/
void mysz_ruch(int x, int y)
{
	if (mbutton == GLUT_LEFT_BUTTON)
	{
		kameraX = poprzednie_kameraX - (pozycjaMyszyX - x) * 0.1;
		kameraZ = poprzednie_kameraZ - (pozycjaMyszyY - y) * 0.1;
	}
	if (mbutton == GLUT_RIGHT_BUTTON)
	{
		kameraD = poprzednie_kameraD + (pozycjaMyszyY - y) * 0.1;
	}

}
/******************************************/



void klawisz(GLubyte key, int x, int y)
{
	switch (key) {

	case 27:    /* Esc - koniec */
		exit(1);
		break;

	case 'x':
	
		break;
	case '1':

		break;
	case '2':

		break;
	}
	
	
}
/*###############################################################*/
void rysuj(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Kasowanie ekranu

	glUseProgram(programID); 

	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV, glm::vec3(0, 0, kameraD)); 
	MV = glm::rotate(MV, (float)glm::radians(kameraZ), glm::vec3(1, 0, 0)); 
	MV = glm::rotate(MV, (float)glm::radians(kameraX), glm::vec3(0, 1, 0)); 
	MV = glm::translate(MV, glm::vec3(0.0, -1.0, 0.0));
	MV = glm::scale(MV, glm::vec3(2.2, 0.1, 2.0));

	glm::mat4 MVP = P * MV;

	GLuint MVP_id = glGetUniformLocation(programID, "MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));	   // wyślij tablicę mv do lokalizacji "MV", która jest typu mat4	

	glBindVertexArray(VAO[0]);
	GLfloat attrib[3] = { 0 };
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	attrib[0] = attrib[2] = 0.4f; attrib[1] = 0.8f;
	glVertexAttrib3fv(1, attrib);
	glDrawElements(GL_QUADS, sizeof(elements)/sizeof(int), GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	attrib[1] = 0.4f; attrib[0] = attrib[2] = 0.0f;
	glVertexAttrib3fv(1, attrib);
	glDrawElements(GL_QUADS, sizeof(elements) / sizeof(int), GL_UNSIGNED_INT, 0);
	
	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV, glm::vec3(0, 0, kameraD));
	MV = glm::rotate(MV, (float)glm::radians(kameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(kameraX), glm::vec3(0, 1, 0));
	MV = glm::scale(MV, glm::vec3(0.35, 0.35, 0.35));
	MV = glm::rotate(MV, glm::radians(270.0f), glm::vec3(1, 0, 0));
	MV = glm::translate(MV, glm::vec3(0.0, 0.0, h)); //x - na boki, y - przod/tyl, z - gora/dol

	((i >= 0 && i < 180) || i==-1) ? angle=float(i/6) : angle=60.0-float(i/6);
	MV = glm::rotate(MV, glm::radians(angle), glm::vec3(0, 1, 0));

	MVP = P * MV;
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));

	glBindVertexArray(VAO[1]);
	attrib[0] = 0.6f; attrib[1] = 0;
	glVertexAttrib3fv(1, attrib);
	glDrawElements(GL_TRIANGLES, sizeof(elements2)/sizeof(int), GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	attrib[0] = 0.75f; attrib[1] = attrib[2] = 0.3f;
	glVertexAttrib3fv(1, attrib);
	glDrawElements(GL_TRIANGLES, sizeof(elements2) / sizeof(int), GL_UNSIGNED_INT, 0);

	glFlush();
	glutSwapBuffers();

}
/*###############################################################*/
void rozmiar(int width, int height)
{
	screen_width = width;
	screen_height = height;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	P = glm::perspective(glm::radians(60.0f), (GLfloat)screen_width / (GLfloat)screen_height, 1.0f, 1000.0f);

	glutPostRedisplay(); // Przerysowanie sceny
}

/*###############################################################*/
void idle()
{

	glutPostRedisplay();
}

/*###############################################################*/

void timer(int value) {
	if (i > 360) { i = -1; }
	if (i >= 0 && i <= 360) {
		i+=2;
		glutTimerFunc(20, timer, 0);
		return;
	}
	else if (h > 1.2 || h < -2.8) {
		d = -d;
	}
	if (abs(h - 1.2) <= 10e-5) {
		i++;
	}
	h += d;
	glutTimerFunc(20, timer, 0);
}
/*###############################################################*/
int main(int argc, char **argv)
{
	//****************************************************
	//wczytywanie wierzchołków dla czajnika z pliku

	FILE* fin;
	fopen_s(&fin, "teapot.txt", "r");
	if (!fin)
	{
		return -1;
	}
	char str[20];
	fgets(str, 20, fin);
	int vSize;
	fscanf_s(fin, "%d", &vSize);
	vSize *= 3;
	//float* vertices2 = (float*)calloc(vSize, sizeof(float));
	float* pv = vertices2;

	if (pv == NULL) {
		perror("Allocation error!");
		return -2;
	}

	for (int i = 0; i < vSize; i++)
	{
		fscanf_s(fin, "%f ", pv++);
	}

	fgets(str, 20, fin);
	//int eSize;
	fscanf_s(fin, "%d", &eSize);
	eSize *= 3;
	//elements2 = (GLuint*)calloc(eSize, sizeof(int));
	int* pe = (int*)elements2;

	if (pe == NULL) {
		perror("Allocation error!");
		return -3;
	}

	for (int i = 0; i < eSize; i++)
	{
		fscanf_s(fin, "%d\t", pe++);
	}

	//****************************************************

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Przyklad 5");

	glewInit(); //init rozszerzeszeń OpenGL z biblioteki GLEW

	glutDisplayFunc(rysuj);			// def. funkcji rysuj¦cej
    glutIdleFunc(idle);			// def. funkcji rysuj¦cej w czasie wolnym procesoora (w efekcie: ci¦gle wykonywanej)
	glutTimerFunc(20, timer, 0);
	glutReshapeFunc(rozmiar); // def. obs-ugi zdarzenia resize (GLUT)
									
	glutKeyboardFunc(klawisz);		// def. obsługi klawiatury
	glutMouseFunc(mysz); 		// def. obsługi zdarzenia przycisku myszy (GLUT)
	glutMotionFunc(mysz_ruch); // def. obsługi zdarzenia ruchu myszy (GLUT)


	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(2, VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glGenBuffers(1, &ebo2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements2), elements2, GL_STATIC_DRAW);


	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	programID = loadShaders("vertex_shader.glsl", "fragment_shader.glsl");


	glutMainLoop();					
	
	glDeleteBuffers(1,&VBO);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &ebo2);
	glDeleteBuffers(2, VAO);
	return(0);
}

