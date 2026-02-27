#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	cameraR = 50;
	cameraXY = 20;
	cameraXZ = 0;

	first = 30.0;
	second = 60.0;
	third = 30.0;

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

	lamp = LoadTexture("lamp.jpg");
	side = LoadTexture("side.jpg");
	top = LoadTexture("top.jpg");
	bot = LoadTexture("bot.jpg");
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(m_eyex, m_eyey, m_eyez,
		0, 20, 0,
		0, 1, 0);
	
	float a = 100;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glPushMatrix();

	glTranslatef(0, a / 2, 0);
	glEnable(GL_TEXTURE_2D);
	DrawEnvCube(a);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	DrawAxes();
	//DrawLampBase();
	//DrawLampArm();
	//DrawLampHead();
	DrawLamp();

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
	
	glDeleteTextures(1, &lamp);
	glDeleteTextures(1, &side);
	glDeleteTextures(1, &top);
	glDeleteTextures(1, &bot);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::UpdateCamera()
{
	if (cameraXY > 90) cameraXY = 90;
	if (cameraXY < -90) cameraXY = -90;
	if (cameraXY == 0) cameraXY = 5;
	if (cameraR > 50) cameraR = 50;
	if (cameraR < 5) cameraR = 5;

	float radXY = ToRad(cameraXY);
	float radXZ = ToRad(cameraXZ);

	m_eyex = cameraR * cos(radXY) * cos(radXZ);
	m_eyey = cameraR * sin(radXY);
	m_eyez = cameraR * cos(radXY) * sin(radXZ);
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texId;
	DImage img;
	img.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texId;
}

void CGLRenderer::DrawAxes()
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);

	glEnd();
}

void CGLRenderer::DrawEnvCube(double a)
{
	float len = a / 2.0;

	glBindTexture(GL_TEXTURE_2D, side);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	
	//prednja
	glTexCoord2f(1, 1);
	glVertex3f(len, -len, len);
	glTexCoord2f(0, 1);
	glVertex3f(-len, -len, len);
	glTexCoord2f(0, 0);
	glVertex3f(-len, len, len);
	glTexCoord2f(1, 0);
	glVertex3f(len, len, len);

	//desno
	glTexCoord2f(1, 1);
	glVertex3f(-len, -len, len);
	glTexCoord2f(0, 1);
	glVertex3f(-len, -len, -len);
	glTexCoord2f(0, 0);
	glVertex3f(-len, len, -len);
	glTexCoord2f(1, 0);
	glVertex3f(-len, len, len);

	//zadnja
	glTexCoord2f(1, 1);
	glVertex3f(-len, -len, -len);
	glTexCoord2f(0, 1);
	glVertex3f(len, -len, -len);
	glTexCoord2f(0, 0);
	glVertex3f(len, len, -len);
	glTexCoord2f(1, 0);
	glVertex3f(-len, len, -len);

	//levo
	glTexCoord2f(1, 1);
	glVertex3f(len, -len, -len);
	glTexCoord2f(0, 1);
	glVertex3f(len, -len, len);
	glTexCoord2f(0, 0);
	glVertex3f(len, len, len);
	glTexCoord2f(1, 0);
	glVertex3f(len, len, -len);

	glEnd();


	glBindTexture(GL_TEXTURE_2D, bot);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);

	//bot
	glTexCoord2f(1, 1);
	glVertex3f(len, -len, len);
	glTexCoord2f(0, 1);
	glVertex3f(len, -len, -len);
	glTexCoord2f(0, 0);
	glVertex3f(-len, -len, -len);
	glTexCoord2f(1, 0);
	glVertex3f(-len, -len, len);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, top);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);

	//top
	glTexCoord2f(1, 1);
	glVertex3f(-len, len, len);
	glTexCoord2f(0, 1);
	glVertex3f(-len, len, -len);
	glTexCoord2f(0, 0);
	glVertex3f(len, len, -len);
	glTexCoord2f(1, 0);
	glVertex3f(len, len, len);

	glEnd();

}

void CGLRenderer::DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lamp);
	glColor3f(1.0, 1.0, 1.0);

	float dAlpha = ToRad(360.0 / nSeg);

	if (!bIsOpen)
	{
		glBegin(GL_TRIANGLE_FAN);

		glTexCoord2d(0.5, 0.25);
		glVertex3f(0.0, 0.0, 0.0);

		
		for (int i = 0; i <= nSeg; i++)
		{
			float x = r1 * cos(i * dAlpha);
			float z = r1 * sin(i * dAlpha);

			double tx = 0.5 * x / r1 + 0.5;
			double ty = 0.25 * z / r1 + 0.25;

			glTexCoord2d(tx, ty);
			glVertex3f(x, 0, z);
		}

		glEnd();
	}

	
	glBegin(GL_QUAD_STRIP);

	float texStepS = 1.0 / nSeg;
	float th = 0;
	if (texMode) th = 0.5;

	for (int i = 0; i <= nSeg; i++)
	{
		float x1 = r1 * cos(i * dAlpha);
		float z1 = r1 * sin(i * dAlpha);

		float x2 = r2 * cos(i * dAlpha);
		float z2 = r2 * sin(i * dAlpha);

		float s = texStepS * i;

		glTexCoord2d(s, th + 0.5);
		glVertex3f(x1, 0, z1);

		glTexCoord2d(s, th);
		glVertex3f(x2, h, z2);
	}

	glEnd();

	if (!bIsOpen)
	{
		glBegin(GL_TRIANGLE_FAN);

		glTexCoord2d(0.5, 0.25);
		glVertex3f(0.0, h, 0.0);

		for (int i = 0; i <= nSeg; i++)
		{
			float x = r2 * cos(i * dAlpha);
			float z = r2 * sin(i * dAlpha);

			double tx = 0.5 * x / r1 + 0.5;
			double ty = 0.25 * z / r1 + 0.25;

			glTexCoord2d(tx, ty);
			glVertex3f(x, h, z);
		}
		glEnd();
	}
	
	

}

void CGLRenderer::DrawLampBase()
{
	DrawCylinder(8, 7, 2, 6, 1, 0);
}

void CGLRenderer::DrawLampArm()
{
	glPushMatrix();

	
	glRotatef(90, 0, 0, 1);
	glTranslatef(0, -1, 0);

	DrawCylinder(3, 3, 2, 18, 1, 0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 3, 0);
	
	DrawCylinder(1, 1, 15, 18, 1, 0);

	glPopMatrix();
	
}

void CGLRenderer::DrawLampHead()
{
	glPushMatrix();
	glTranslatef(0, 0, -3);

	glPushMatrix();
	DrawCylinder(6, 3, 5, 20, 0, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 3);
	glTranslatef(0, 7.5, 0);
	glTranslatef(0.5, 0, 0);
	glRotatef(90, 0, 0, 1);
	DrawCylinder(2, 2, 1, 18, 1, 0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 5, 0);
	DrawCylinder(3, 3, 5, 18, 1, 1);
	glTranslatef(0, 5, 0);
	DrawCylinder(3, 2, 1, 18, 1, 0);
	glPopMatrix();

	glPopMatrix();
}

void CGLRenderer::DrawLamp()
{
	
	DrawLampBase();

	glRotatef(first, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	DrawLampArm();
	
	glTranslatef(0, 18, 0);
	glRotatef(second, 1, 0, 0);
	DrawLampArm();

	glTranslatef(0, 18, 0);
	glRotatef(third, 1, 0, 0);
	glRotatef(180, 0, 1, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, -7.5, 0);
	DrawLampHead();

}
