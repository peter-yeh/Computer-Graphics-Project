#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////////////////////////////////////

#define PI                  3.1415926535897932384626433832795

#define MAX_NUM_OF_DISCS    200     // Limit the number of discs.
#define MIN_RADIUS          10.0    // Minimum radius of disc.
#define MAX_RADIUS          50.0    // Maximum radius of disc.
#define NUM_OF_SIDES        18      // Number of polygon sides to approximate a disc.

#define MIN_X_SPEED         1.0     // Minimum speed of disc in X direction.
#define MAX_X_SPEED         20.0    // Maximum speed of disc in X direction.
#define MIN_Y_SPEED         1.0     // Minimum speed of disc in Y direction.
#define MAX_Y_SPEED         20.0    // Maximum speed of disc in Y direction.

#define DESIRED_FPS         30      // Approximate desired number of frames per second.
#define FRAME_DURATION		(unsigned int) 1000 / DESIRED_FPS

/////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////

typedef struct discType
{
	double pos[2];          // The X and Y coordinates of the center of the disc.
	double speed[2];        // The velocity of the disc in X and Y directions. Can be negative.
	double radius;          // Radius of the disc.
	unsigned char color[3]; // R, G, B colors of the disc.
} discType;


int numDiscs = 0;                   // Number of discs that have been added.

discType disc[MAX_NUM_OF_DISCS];  // Array for storing discs.

bool drawWireframe = false;         // Draw polygons in wireframe if true, otherwise
									// otherwise polygons are filled.

int winWidth = 800;                 // Window width in pixels.
int winHeight = 600;                // Window height in pixels.


// Returns random double between min and max
double Rand(double min, double max)
{
	return min + (rand() / (RAND_MAX / (max - min)));
}


double RandX() {
	return Rand(0, 1) > 0.5
		? Rand(MIN_X_SPEED, MAX_X_SPEED) // positive speed
		: Rand(-MAX_X_SPEED, -MIN_X_SPEED); // negative speed
}


double RandY() {
	return Rand(0, 1) > 0.5
		? Rand(MIN_Y_SPEED, MAX_Y_SPEED) // positive speed
		: Rand(-MAX_Y_SPEED, -MIN_Y_SPEED); // negative speed
}


// Draw the disc in its color using GL_TRIANGLE_FAN.
void DrawDisc(const discType* d)
{
	static bool firstTime = true;
	static double unitDiscVertex[NUM_OF_SIDES + 1][2];

	if (firstTime)
	{
		firstTime = false;
		double angle;
		for (int i = 0; i < NUM_OF_SIDES + 1; i++) {
			angle = 2 * PI * i / NUM_OF_SIDES;
			unitDiscVertex[i][0] = cos(angle);
			unitDiscVertex[i][1] = sin(angle);
		}
	}

	GLdouble x_offset = d->pos[0];
	GLdouble y_offset = winHeight - d->pos[1];
	GLdouble r_offset = d->radius;

	glBegin(GL_TRIANGLE_FAN);
	glColor3ub((d->color)[0], (d->color)[1], (d->color)[2]);
	glVertex2d(x_offset, y_offset);

	for (int i = 0; i < NUM_OF_SIDES + 1; i++) {
		GLdouble x = unitDiscVertex[i][0] * r_offset + x_offset;
		GLdouble y = unitDiscVertex[i][1] * r_offset + y_offset;
		glVertex2d(x, y);
	}
	glEnd();
}


// The display callback function.
void MyDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (drawWireframe)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (int i = 0; i < numDiscs; i++) DrawDisc(&disc[i]);

	glFlush();
	glutSwapBuffers();
}


// If mouse left button is pressed, a new disc is created with its center 
// at the mouse cursor position. The new disc is assigned the followings:
void MyMouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (numDiscs >= MAX_NUM_OF_DISCS)
			printf("Already reached maximum number of discs.\n");
		else
		{
			double randSpeedX = RandX();
			double randSpeedY = RandY();

			double randRadius = Rand(MIN_RADIUS, MAX_RADIUS);

			unsigned char randRedColor = (char)rand() % 256;
			unsigned char randGreenColor = (char)rand() % 256;
			unsigned char randBlueColor = (char)rand() % 256;

			disc[numDiscs] = {
				{(double)x, (double)y},
				{randSpeedX, randSpeedY},
				randRadius,
				{ randRedColor, randGreenColor, randBlueColor }
			};;

			numDiscs++;
			glutPostRedisplay();
		}
	}
}


// Calls MyMouse with left click held to spawn many discs
void HoldMouseButton(int x, int y) {
	MyMouse(GLUT_LEFT_BUTTON, GLUT_DOWN, x, y);
}


// It sets up the viewing.
void MyReshape(int w, int h)
{
	winWidth = w;
	winHeight = h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// The keyboard callback function.
void MyKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		// Quit program.
	case 'q':
	case 'Q': exit(0);
		break;

		// Toggle wireframe or filled polygons.
	case 'w':
	case 'W': drawWireframe = !drawWireframe;
		glutPostRedisplay();
		break;

		// Reset and erase all discs.
	case 'r':
	case 'R': numDiscs = 0;
		glutPostRedisplay();
		break;
	}
}


// Updates the positions of all the discs.
void UpdateAllDiscPos(void)
{
	struct discType* d;

	for (int i = 0; i < numDiscs; i++)
	{		d = &disc[i];

		// I think shld be speed/FRAME_DURATION to get the distance coverd every FRAME_DURATION, 
		// But the disc is moving very slowly. Used this current method to make it move faster
		// The speed is added to the position of the disc every FRAME_DURATION.
		d->pos[0] += d->speed[0];
		d->pos[1] += d->speed[1];

		// if disc is out of the window horizontally
		if (d->pos[0] + d->radius > winWidth) {
			d->pos[0] = winWidth - d->radius;
			d->speed[0] = -d->speed[0];
		}
		else if (d->pos[0] - d->radius < 0) {
			d->pos[0] = d->radius;
			d->speed[0] = -d->speed[0];
		}

		// if disc if out of the window vertically
		if (d->pos[1] + d->radius > winHeight) {
			d->pos[1] = winHeight - d->radius;
			d->speed[1] = -d->speed[1];
		}
		else if (d->pos[1] - d->radius < 0) {
			d->pos[1] = d->radius;
			d->speed[1] = -d->speed[1];
		}
	}

	glutPostRedisplay();
}


// The timer callback function.
void MyTimer(int v)
{
	// calls UpdateAllDiscPos() every FRAME_DURATION
	UpdateAllDiscPos();
	glutTimerFunc(FRAME_DURATION, MyTimer, v);
}


// It initializes OpenGL states.
void MyInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0); // Black background color.
	glShadeModel(GL_FLAT);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Spawn Random Circles");

	MyInit();

	// Register the callback functions.
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouse);
	glutKeyboardFunc(MyKeyboard);
	glutTimerFunc(0, MyTimer, 0);

	// Display user instructions in console window.
	printf("Click LEFT MOUSE BUTTON in window to add new disc.\n");
	printf("Hold LEFT MOUSE BUTTON in window to continously add new disc.\n");
	printf("Press 'w' to toggle wireframe.\n");
	printf("Press 'r' to erase all discs.\n");
	printf("Press 'q' to quit.\n\n");

	// spawns many circles while mouse is clicked for testing
	glutMotionFunc(HoldMouseButton);

	// Enter GLUT event loop.
	glutMainLoop();
	return 0;
}
