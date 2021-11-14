#include <GL/glut.h>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <math.h>
#define PI acos(-1)

//#define CALCULATE_NORMAL

using namespace std;

#define KEY_ESC 27

int fullscreen = 0;
int mouseDown = 0;
int mode = 0;
int idle_state = 0;

float xrot = 0.0f;
float yrot = 0.0f;
float zrot = 0.0f;

float xdiff = 0.0f;
float ydiff = 0.0f;

float rloc = 2.0;
float floc = -30.0;
float yloc = 0.0;

float rlit = 4.0;
float flit = 30.0;
float ylit = 2.0;

float grow_shrink = 70.0f;
float resize_f = 1.0f;

class Double3 {
public:
	double x;
	double y;
	double z;
	
	void add(Double3 p1, Double3 p2)
	{
		x = p1.x + p2.x;
		y = p1.y + p2.y;
		z = p1.z + p2.z;
	}

	void substract(Double3 p1, Double3 p2)
	{
		x = p1.x - p2.x;
		y = p1.y - p2.y;
		z = p1.z - p2.z;
	}

	void cross(Double3 p1, Double3 p2)
	{
		x = p1.y * p2.z - p1.z * p2.y;
		y = p1.z * p2.x - p1.x * p2.z;
		z = p1.x * p2.y - p1.y * p2.x;
	}

	void scalar(float times)
	{
		x = x * times;
		y = y * times;
		z = z * times;
	}

	void normalization()
	{
		double L = sqrt(x * x + y * y + z * z);
		x = x / L;
		y = y / L;
		z = z / L;
	}

	void reverse()
	{
		x = -x;
		y = -y;
		z = -z;
	}
};

double dot3(Double3 e1, Double3 e2)
{
	return e1.x * e2.x + e1.y * e2.y + e1.z * e2.z;
}

class Double2 {
public:
	double tu;
	double tv;
};

class Face {
public:
	int V[3];
	int T[3];
	int N[3];
};

class PIC {
public:
	vector<Double3> V;	//point
	vector<Double2> VT;	//texture
	vector<Double3> VN;	//normal
	vector<Face> F;

	float GetMaxV()
	{
		int vlength = V.size();
		int xmax = 0;
		int ymax = 0;
		int zmax = 0;

		for (int i = 0; i < vlength; i++)
		{
			if (xmax < V[i].x)
				xmax = V[i].x;
			if (ymax < V[i].y)
				ymax = V[i].y;
			if (zmax < V[i].z)
				zmax = V[i].z;
		}
		return sqrt(xmax * xmax + ymax * ymax + zmax * zmax);
	}
};

PIC m_pic;

void GetNormal()
{
	int vlength = m_pic.V.size();
	int flength = m_pic.F.size();
	vector<Double3> VN_new;
	VN_new.clear();
	VN_new.swap(m_pic.VN);

	for (int i = 0; i < vlength; i++)
	{
		Double3 p = m_pic.V[i];
		Double3 p1;
		Double3 p2;
		Double3 vn_pre;
		Double3* vn = new Double3();
		vn->x = 0;
		vn->y = 0;
		vn->z = 0;

		for (int j = 0; j < flength; j++)
		{
			if (m_pic.F[j].V[0] == i)
			{
				p1 = m_pic.V[m_pic.F[j].V[1]];
				p2 = m_pic.V[m_pic.F[j].V[2]];
				vn_pre = VN_new[m_pic.F[j].N[0]];
			}
			else if (m_pic.F[j].V[1] == i)
			{

				p1 = m_pic.V[m_pic.F[j].V[0]];
				p2 = m_pic.V[m_pic.F[j].V[2]];
				vn_pre = VN_new[m_pic.F[j].N[1]];
			}
			else if (m_pic.F[j].V[2] == i)
			{

				p1 = m_pic.V[m_pic.F[j].V[0]];
				p2 = m_pic.V[m_pic.F[j].V[1]];
				vn_pre = VN_new[m_pic.F[j].N[2]];
			}
			else
				continue;

			Double3 e1;
			e1.substract(p1, p);
			Double3 e2;
			e2.substract(p2, p);

			e1.normalization();
			e2.normalization();
			float angle = acos(dot3(e1, e2));

			Double3 fn;
			fn.cross(e1, e2);
			fn.normalization();
			fn.scalar(angle);
			if (dot3(fn, vn_pre) < 0)
				fn.reverse();

			vn->add(*vn, fn);
		}
		vn->normalization();
		m_pic.VN.push_back(*vn);
	}
	for (int i = 0; i < flength; i++)
	{
		m_pic.F[i].N[0] = m_pic.F[i].V[0];
		m_pic.F[i].N[1] = m_pic.F[i].V[1];
		m_pic.F[i].N[2] = m_pic.F[i].V[2];
	}
}

void ReadPIC() {
	ifstream ifs("./bunny.obj");
	string s;
	Face* f;
	Double3* v;
	Double3* vn;
	Double2* vt;
	while (getline(ifs, s)) {
		if (s.length() < 2) continue;

		if (s[0] == 'v') {
			if (s[1] == 't') {
				istringstream in(s);
				vt = new Double2();
				string head;
				in >> head >> vt->tu >> vt->tv;
				m_pic.VT.push_back(*vt);
			}
			else if (s[1] == 'n') {
				istringstream in(s);
				vn = new Double3();
				string head;
				in >> head >> vn->x >> vn->y >> vn->z;
				m_pic.VN.push_back(*vn);
			}
			else {
				istringstream in(s);
				v = new Double3();
				string head;
				in >> head >> v->x >> v->y >> v->z;
				m_pic.V.push_back(*v);
			}
		}
		else if (s[0] == 'f') {
			int k = s.size();
			for (int j = 0; j < k; j++) {
				if (s[j] == '/')
					s[j] = ' ';
			}

			istringstream in(s);
			f = new Face();
			string head;
			in >> head;
			int i = 0;
			while (i < 3) {
				if (m_pic.V.size()) {
					in >> f->V[i];
					f->V[i]--;
				}
				if (m_pic.VT.size()) {
					in >> f->T[i];
					f->T[i]--;
				}
				if (m_pic.VN.size()) {
					in >> f->N[i];
					f->N[i]--;
				}
				i++;
			}
			m_pic.F.push_back(*f);
		}
	}
#ifdef CALCULATE_NORMAL
	GetNormal();
#endif
}

void Init()
{
	ReadPIC();
	glEnable(GL_TEXTURE_2D);
}

void draw()
{
	float DIV = m_pic.GetMaxV();
	glTranslatef(0, 0, 0);

	static GLfloat glfMatAmbient[] = { 0.000f, 0.450f, 1.000f, 1.0f };
	static GLfloat glfMatDiffuse[] = { 0.750f, 0.580f, 0.580f, 1.0f };
	static GLfloat glfMatSpecular[] = { 1.000f, 1.000f, 1.000f, 1.0f };
	static GLfloat glfMatEmission[] = { 0.000f, 0.000f, 0.000f, 1.0f };
	static GLfloat fShininess = 128.000f;

	float xlit = rlit * sin(flit * PI / 180);
	float zlit = rlit * cos(flit * PI / 180);
	GLfloat light_position[] = {xlit, ylit, zlit, 1.0 }; //µ„π‚‘¥
	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glMaterialfv(GL_FRONT, GL_AMBIENT, glfMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, glfMatDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, glfMatSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, glfMatEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, fShininess);

	for (int i = 0; i < m_pic.F.size(); i++)
	{
		glBegin(GL_TRIANGLES);
			if (m_pic.VT.size())
				glTexCoord2f(m_pic.VT[m_pic.F[i].T[0]].tu, m_pic.VT[m_pic.F[i].T[0]].tv);
		
			if (m_pic.VN.size())
				glNormal3f(m_pic.VN[m_pic.F[i].N[0]].x, m_pic.VN[m_pic.F[i].N[0]].y, m_pic.VN[m_pic.F[i].N[0]].z);
		
			glVertex3f(m_pic.V[m_pic.F[i].V[0]].x / DIV, m_pic.V[m_pic.F[i].V[0]].y / DIV, m_pic.V[m_pic.F[i].V[0]].z / DIV);


			if (m_pic.VT.size())
				glTexCoord2f(m_pic.VT[m_pic.F[i].T[1]].tu, m_pic.VT[m_pic.F[i].T[1]].tv);

			if (m_pic.VN.size())
				glNormal3f(m_pic.VN[m_pic.F[i].N[1]].x, m_pic.VN[m_pic.F[i].N[1]].y, m_pic.VN[m_pic.F[i].N[1]].z);

			glVertex3f(m_pic.V[m_pic.F[i].V[1]].x / DIV, m_pic.V[m_pic.F[i].V[1]].y / DIV, m_pic.V[m_pic.F[i].V[1]].z / DIV);


			if (m_pic.VT.size())
				glTexCoord2f(m_pic.VT[m_pic.F[i].T[2]].tu, m_pic.VT[m_pic.F[i].T[2]].tv);

			if (m_pic.VN.size())
				glNormal3f(m_pic.VN[m_pic.F[i].N[2]].x, m_pic.VN[m_pic.F[i].N[2]].y, m_pic.VN[m_pic.F[i].N[2]].z);

			glVertex3f(m_pic.V[m_pic.F[i].V[2]].x / DIV, m_pic.V[m_pic.F[i].V[2]].y / DIV, m_pic.V[m_pic.F[i].V[2]].z / DIV);

		glEnd();
	}
}

int init(void)
{
	glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1.0f);

	return 1;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	float xloc = rloc * sin(floc * PI / 180);
	float zloc = rloc * cos(floc * PI / 180);
	gluLookAt(
		xloc,  yloc, zloc,
		0.0f,  0.0f, 0.0f,
		0.0f,  1.0f, 0.0f);

	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);

	draw();

	glFlush();
	glutSwapBuffers();
}

void resize(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	gluPerspective(grow_shrink, resize_f * w / h, resize_f, 100 * resize_f);
	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	gluPerspective(grow_shrink, resize_f * w / h, resize_f, 100 * resize_f);
}

void idle(void)
{
	if (!mouseDown && idle_state)
	{
		floc += 0.2;
	}

	glutPostRedisplay();
}

void help(int key, int x, int y)
{
	system("cls");
	cout << "M or m-------> change the mode (move the camera (default)/light)\n";
	cout << "w -----------> rotate the object (Y axis)\n";
	cout << "s -----------> rotate the object (Y axis)\n";
	cout << "d -----------> rotate the object (X axis)\n";
	cout << "a -----------> rotate the object (X axis)\n";
	cout << "q -----------> rotate the object (Z axis)\n";
	cout << "e -----------> rotate the object (Z axis)\n";
	cout << "W -----------> adjust the height of the camera/light\n";
	cout << "S -----------> adjust the height of the camera/light\n";
	cout << "D -----------> adjust the distance to the camera/light\n";
	cout << "A -----------> adjust the distance to the camera/light\n";
	cout << "Q -----------> rotate the camera/light (Z axis)\n";
	cout << "E -----------> rotate the camera/light (Z axis)\n";
	cout << "Z or z ------> larger\n";
	cout << "X or x ------> smaller\n";
	cout << "R or r ------> reset\n";
	cout << "H or h ------> help\n";
	cout << "I or i ------> enable/diable rotating automatically\n";
	cout << "F1 ----------> fullscreen/exit fullscreen\n";
	cout << "Esc ---------> exit\n\n";
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(1);
		break;

	case 'r':
	case 'R':
		mode = 0;
		fullscreen = 0;
		mouseDown = 0;
		mode = 0;

		xrot = 0.0f;
		yrot = 0.0f;
		zrot = 0.0f;

		xdiff = 0.0f;
		ydiff = 0.0f;

		rloc = 2.0;
		floc = -30.0;
		yloc = 0.0;

		rlit = 4.0;
		flit = 30.0;
		ylit = 2.0;

		idle_state = 0;
		grow_shrink = 70.0f;
		break;

	case 'm':
	case 'M':
		mode = !mode;
		break;

	case 'w':
		yrot += 0.25f;
		break;

	case 'W':
		if (mode)
			flit += 0.8f;
		else
			floc += 0.8f;
		break;

	case 's':
		yrot -= 0.25f;
		break;

	case 'S':
		if (mode)
			flit -= 0.8f;
		else
			floc -= 0.8f;
		break;

	case 'a':
		xrot -= 0.25f;
		break;

	case 'A':
		if (mode)
			rlit -= 0.01f;
		else
			rloc -= 0.01f;
		break;

	case 'd':
		xrot += 0.25f;
		break;

	case 'D':
		if (mode)
			rlit += 0.01f;
		else
			rloc += 0.01f;
		break;

	case 'q':
		zrot -= 0.25f;
		break;

	case 'Q':
		if (mode)
			ylit -= 0.02f;
		else
			yloc -= 0.02f;
		break;

	case 'e':
		zrot += 0.25f;
		break;

	case 'E':
		if (mode)
			ylit += 0.02f;
		else
			yloc += 0.02f;
		break;

	case 'h':
	case 'H':
		help(key, x, y);
		break;
	case 'z':
	case 'Z':
		grow_shrink--;
		resize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

		break;
	case 'x':
	case 'X':
		grow_shrink++;
		resize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

		break;

	case 'i':
	case 'I':
		idle_state = !idle_state;
		break;
	}

	glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		fullscreen = !fullscreen;

		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(500, 500);
			glutPositionWindow(50, 50);
		}
	}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		mouseDown = 1;

		xdiff = x - yrot;
		ydiff = -y + xrot;
	}
	else
		mouseDown = 0;
}

void mouseMotion(int x, int y)
{
	if (mouseDown)
	{
		yrot = x - xdiff;
		zrot = y + ydiff;

		glutPostRedisplay();
	}
}

int main(int argc, char* argv[])
{
	cout << "H/h for more help...";
	glutInit(&argc, argv);
	Init();
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("bunny");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	if (!init())
		return 1;

	glutMainLoop();

	return 0;
}
