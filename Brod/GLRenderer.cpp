#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
#include <iostream>
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_hrc = NULL;

	cameraR = 10.0;
	cameraXY = 45.0;
	cameraXZ = 45.0;

	m_bLight = false;
	m_norm = false;

	UpdateCamera();

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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	ship = LoadTexture("ShipT1.png");
	bg[0] = LoadTexture("front.jpg");
	bg[1] = LoadTexture("left.jpg");
	bg[2] = LoadTexture("right.jpg");
	bg[3] = LoadTexture("back.jpg");
	bg[4] = LoadTexture("top.jpg");
	bg[5] = LoadTexture("bot.jpg");

	glEnable(GL_TEXTURE_2D);

	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(m_eyex, m_eyey, m_eyez,
		0, 0, 0,
		0, 1, 0);

	GLfloat light_position[] = { 0.0 , 0.0, 1.0, 0.0 }; //z
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//ship front left right back top bot
	glDisable(GL_LIGHTING);

	DrawSpaceCube(20);
	DrawAxes();

	if (m_bLight)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

	}

	double d = sqrt(5.8 * 5.8 + 2.15 * 2.15);
	glTranslatef(-d / 2., 0, 0);
	glRotatef(-60, 1, 0, 0);
	
	DrawShip();


	glFlush();
	SwapBuffers(pDC->m_hDC);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(50, (double)w / (double)h, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &ship);
	glDeleteTextures(6, bg);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	//x osa
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);

	//y osa
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);

	//z osa
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10.0);

	glEnd();

	glEnable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawTriangle(float d1, float d2, float rep)
{
	if (m_bLight)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}

	float c = sqrt(pow(d1, 2) + pow(d2, 2));

	float texD1 = d1 / c;
	float texD2 = d2 / c;

	float x = texD2*texD2;
	float h = sqrt(x - x*x);

	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, ship);
	glColor3f(1.0, 1.0, 1.0);
	
	glBegin(GL_TRIANGLES);

	glNormal3f(0, 0, 1.0);

	glTexCoord2f(0.5 * rep, 0.0 * rep);
	glVertex3f(0.0, 0.0, 0.0);

	glTexCoord2f((0.5 + h)* rep, (1-x) * rep);
	glVertex3f(d1, 0.0, 0.0);

	glTexCoord2f(0.5* rep, 1.0 * rep);
	glVertex3f(d1, d2, 0.0);

	glEnd();

	glPopMatrix();

	if (m_norm)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 0.0);

		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0, 0, 5.0);

		glVertex3f(d1, 0.0, 0.0);
		glVertex3f(d1, 0, 5.0);

		glVertex3f(d1, d2, 0.0);
		glVertex3f(d1, d2, 5.0);

		glEnd();

	}
}

void CGLRenderer::DrawShip()
{
	float d1 = 5.8;
	float d2 = 2.15;
	int rep = 3;

	double angle = atan2(d2, d1) * 180.0 / PI;

	//dole levo
	glPushMatrix();
	glRotatef(-4.75, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-angle, 0, 0, 1);
	DrawTriangle(d1, d2, rep);
	glPopMatrix();

	//dole desno
	glPushMatrix();
	glRotatef(4.75, 0, 1, 0);
	glRotatef(-13, 1, 0, 0);
	glRotatef(-angle, 0, 0, 1);
	DrawTriangle(d1, d2, rep);
	glPopMatrix();

	//gore levo
	glPushMatrix();
	glScalef(1, -1, 1);
	glRotatef(-4.75, 0, 1, 0);
	glRotatef(13, 1, 0, 0);
	glRotatef(-angle, 0, 0, 1);
	DrawTriangle(d1, d2, rep);
	glPopMatrix();

	//gore desno
	glPushMatrix();
	glScalef(1, -1, 1);
	glRotatef(4.75, 0, 1, 0);
	glRotatef(-13, 1, 0, 0);
	glRotatef(-angle, 0, 0, 1);
	DrawTriangle(d1, d2, rep);
	glPopMatrix();
}

void CGLRenderer::DrawSpaceCube(double a)
{
	//ship front left right back top bot
	
	//front
	glBindTexture(GL_TEXTURE_2D, bg[0]);
	glColor3f(1.0, 1.0, 1.0);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, -a / 2, -a / 2);

	glEnd();

	//left
	glBindTexture(GL_TEXTURE_2D, bg[1]);

	glBegin(GL_QUADS);

	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, a / 2, -a / 2);

	glEnd();

	//right
	glBindTexture(GL_TEXTURE_2D, bg[2]);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, -a / 2, a / 2);

	glEnd();

	//back
	glBindTexture(GL_TEXTURE_2D, bg[3]);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, -a / 2, a / 2);

	glEnd();


	//top - meni je bot lol
	glPushMatrix();
	glRotatef(270, 0, 1, 0);
	glBindTexture(GL_TEXTURE_2D, bg[4]);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(a / 2, -a / 2, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, -a / 2, -a / 2);

	glEnd();
	glPopMatrix();

	glPushMatrix();
	glRotatef(270, 0, 1, 0);
	//bot - meni je top lol
	glBindTexture(GL_TEXTURE_2D, bg[5]);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a / 2, a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, a / 2, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, a / 2, -a / 2);

	glEnd();

	glPopMatrix();
}

void CGLRenderer::UpdateCamera()
{
	double alpha = ToRad(cameraXY);
	double beta = ToRad(cameraXZ);

	m_eyex = cameraR * cos(alpha) * cos(beta);
	m_eyey = cameraR * sin(alpha);
	m_eyez = cameraR * cos(alpha) * sin(beta);
}

void CGLRenderer::RotateView(double dx, double dy)
{
	cameraXY += dy;
	cameraXZ += dx;

	if (cameraXY > 90) cameraXY = 90;
	if (cameraXY < -90) cameraXY = -90;

	UpdateCamera();
}

void CGLRenderer::ZoomView(double dr)
{
	cameraR += dr;

	UpdateCamera();
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(),
		GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texID;
}

void CGLRenderer::ShowLight()
{
	m_bLight = !m_bLight;
}

void CGLRenderer::ShowNormale()
{
	m_norm = !m_norm;
}
