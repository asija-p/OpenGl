#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_hrc = NULL;

	// inicijalni polozaj kamere
	m_cameraR = 30.0;
	m_cameraAngleXY = 90.0;
	m_cameraAngleXZ = 0.0; 

	y_rotAngle = 0.0;

	UpdateCameraPosition();
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(0.4f, 0.7f, 1.0f, 1.0f); // boja kojom se brise scena
	glEnable(GL_DEPTH_TEST);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(
		m_eyex, m_eyey, m_eyez,
		0.0, 8.0, 0.0,
		0.0, 1.0, 0.0
	);

	DrawAxis(30);
	//DrawCylinder(10, 5, 3, 6);
	//DrawCone(10, 5, 6);
	//DrawSphere(10, 40, 40);
	DrawGrid(20, 20, 10, 10);
	DrawFigure(y_rotAngle);

	glFlush();
	//---------------------------------
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	double aspect = (double)w / (double)h;
	glMatrixMode(GL_PROJECTION); //podesavanje kamere
	glLoadIdentity();
	gluPerspective(45.0, aspect, 0.1, 100);
	glMatrixMode(GL_MODELVIEW); //transformacije modela
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::UpdateCameraPosition()
{
	double radXY = ToRad(m_cameraAngleXY);
	double radXZ = ToRad(m_cameraAngleXZ);

	m_eyex = m_cameraR * cos(radXZ) * cos(radXY);
	m_eyey = m_cameraR * sin(radXZ);
	m_eyez = m_cameraR * cos(radXZ) * sin(radXY);
}

void CGLRenderer::RotateView(double dx, double dy)
{
	m_cameraAngleXY -= dx;
	m_cameraAngleXZ += dy;

	if (m_cameraAngleXZ > 90) m_cameraAngleXZ = 90;
	if (m_cameraAngleXZ < -90) m_cameraAngleXZ = -90;

	UpdateCameraPosition();
}

void CGLRenderer::DrawSphere(double r, int nSegAlpha, int nSegBeta)
{
	glTranslatef(0, r, 0);

	double dAlpha = ToRad(180.0 / nSegAlpha);
	double dBeta = ToRad(360.0 / nSegBeta);

	for (int j = 0; j < nSegAlpha; j++)
	{
		double alpha1 = -PI / 2 + j * dAlpha;
		double alpha2 = alpha1 + dAlpha;

		glBegin(GL_QUAD_STRIP);
		for (int i = 0; i <= nSegBeta; i++)
		{
			double x = r * cos(alpha1) * cos(i * dBeta);
			double y = r * sin(alpha1);
			double z = r * cos(alpha1) * sin(i * dBeta);

			glVertex3d(x, y, z);

			x = r * cos(alpha2) * cos(i * dBeta);
			y = r * sin(alpha2);
			z = r * cos(alpha2) * sin(i * dBeta);

			glVertex3d(x, y, z);
		}
		glEnd();
	}
	
}

void CGLRenderer::DrawCylinder(double h, double r1, double r2, int nSeg)
{
	double alpha = ToRad(360.0 / nSeg);

	glBegin(GL_TRIANGLE_FAN);

	glVertex3d(0, h, 0);
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r1 * cos(i * alpha);
		double z = r1 * sin(i * alpha);

		glVertex3d(x, h, z);
	}

	glEnd();

	glBegin(GL_TRIANGLE_FAN);

	glVertex3d(0, 0, 0);
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r2 * cos(i * alpha);
		double z = r2 * sin(i * alpha);

		glVertex3d(x, 0, z);
	}

	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r1 * cos(i * alpha);
		double z = r1 * sin(i * alpha);

		glVertex3d(x, h, z);

		x = r2 * cos(i*alpha);
		z = r2 * sin(i*alpha);

		glVertex3d(x, 0, z);
	}
	glEnd();
	
}

void CGLRenderer::DrawCone(double h, double r, int nSeg)
{
	double alpha = ToRad(360.0 / nSeg);

	glBegin(GL_TRIANGLE_FAN);

	glVertex3d(0, 0, 0);
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r * cos(i * alpha);
		double z = r * sin(i * alpha);

		glVertex3d(x, 0, z);
	}

	glEnd();

	glBegin(GL_TRIANGLE_FAN);

	glVertex3d(0, h, 0);
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r * cos(i * alpha);
		double z = r * sin(i * alpha);

		x = r * cos(i * alpha);
		z = r * sin(i * alpha);

		glVertex3d(x, 0, z);
	}
	glEnd();
}

void CGLRenderer::DrawAxis(double width)
{
	glBegin(GL_LINES);

	glColor3f(1.0, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(width, 0, 0);

	glColor3f(0, 1.0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, width, 0);

	glColor3f(0.0, 0, 1.0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, width);

	glEnd();
}

void CGLRenderer::DrawGrid(double width, double height, int nSegW, int nSegH)
{
	double dx = width / nSegW;
	double dy = height / nSegH;

	glColor3f(0, 0, 0);
	glBegin(GL_LINES);

	for (int i = 0; i <= nSegH; i++)
	{
		glVertex3d(-width / 2, 0, -height / 2 + i*dy);
		glVertex3d(width / 2, 0, -height / 2 + i*dy);

		glVertex3d(-width / 2 + i*dx, 0, -height / 2);
		glVertex3d(-width / 2 + i*dx, 0, height / 2);
	}

	glEnd();
}

void CGLRenderer::DrawFigure(double angle)
{
	glColor3f(0.6, 0.3, 0);
	//donji deo
	DrawCylinder(2.5, 2.5, 2, 8);
	///gornjideo
	//glPushMatrix();
	glTranslatef(0, 2.5, 0);
	DrawCylinder(1, 3, 3, 8);

	//prvi deo
	glTranslatef(0, 1, 0);
	glColor3f(1.0, 1.0, 0);

	//rotacija
	glRotatef(angle, 1, 0, 0);

	DrawCylinder(2.5, 1, 1, 8);
	//prvi kruzic
	glTranslatef(0, 2.5, 0);
	glColor3f(1.0, 1.0, 1.0);
	DrawSphere(.5, 30, 30);

	glPushMatrix();
	glTranslatef(0, .5, 0);

	//gore
	glColor3f(0.94, 0.35, 0.13);
	DrawCylinder(2.5, 0.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	glPushMatrix();
	//gore levo
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCone(2.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	//gore levo levo
	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCone(2.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	//gore levo gore
	glPopMatrix();
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCone(2.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	//gore desno
	glPopMatrix();
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCone(2.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	glPushMatrix();
	//gore desno gore
	glPopMatrix();
	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCone(2.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	//gore desno sredina
	glPopMatrix();
	glPushMatrix();
	glRotatef(0, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCone(2.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	//gore desno desno
	glPopMatrix();
	//glPushMatrix();
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCylinder(2.5, 0.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	//levo
	glPopMatrix();
	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCylinder(2.5, 0.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
	//desno
	glPopMatrix();
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, .5, 0);
	glColor3f(0.94, 0.35, 0.13);
	DrawCone(2.5, 0.5, 8);
	glTranslatef(0, 2.5, 0);
	glColor3f(1, 1, 1);
	DrawSphere(.5, 30, 30);
}

void CGLRenderer::RotateYellow(double angle)
{
	y_rotAngle += angle;
}
