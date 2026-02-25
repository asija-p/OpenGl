#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "../../../../../../Downloads/DImage/DImage/DImage.h"
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_hrc = NULL;

	m_cameraR = 50.0;
	m_cameraAngleXY = 140.0;
	m_cameraAngleXZ = 40.0;

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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	m_texSpider = LoadTexture("spider.png");
	m_texEnv[0] = LoadTexture("front.jpg");
	m_texEnv[1] = LoadTexture("back.jpg");
	m_texEnv[2] = LoadTexture("left.jpg");
	m_texEnv[3] = LoadTexture("right.jpg");
	m_texEnv[4] = LoadTexture("top.jpg");
	m_texEnv[5] = LoadTexture("bot.jpg");

	//glEnable(GL_TEXTURE_2D);

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
		0, 10, 0,
		0, 1, 0
	);

	glPushMatrix();
	glTranslatef(0.0, 50, 0);
	DrawEnvCube(100);
	glPopMatrix();


	glTranslatef(0.0, 5, 0);
	DrawSpider();


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
	gluPerspective(50, (double)w / double(h), 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 

	glDeleteTextures(1, &m_texSpider);
	glDeleteTextures(6, m_texEnv);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxes()
{
	glBegin(GL_LINES);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(20, 0, 0);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 20, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 20);

	glEnd();
}

void CGLRenderer::RotateView(double dx, double dy)
{
	m_cameraAngleXY -= dx;
	m_cameraAngleXZ += dy;

	if (m_cameraAngleXZ > 90) m_cameraAngleXZ = 90;
	if (m_cameraAngleXZ < -90) m_cameraAngleXZ = -90;

	if (m_cameraAngleXZ < 5) m_cameraAngleXZ = 5;

	UpdateCameraPosition();
}

void CGLRenderer::ZoomView(double dx)
{
	m_cameraR += dx;

	if (m_cameraR > 50) m_cameraR = 50;
	if (m_cameraR < 8) m_cameraR = 8;

	UpdateCameraPosition();
}

void CGLRenderer::UpdateCameraPosition()
{
	double radXY = ToRad(m_cameraAngleXY);
	double radXZ = ToRad(m_cameraAngleXZ);

	m_eyex = m_cameraR * cos(radXZ) * cos(radXY);
	m_eyey = m_cameraR * sin(radXZ);
	m_eyez = m_cameraR * cos(radXZ) * sin(radXY);
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texID;
	DImage img;
	img.Load(CString(fileName));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());
	
	return texID;
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_TEXTURE_2D);

	//front
	glBindTexture(GL_TEXTURE_2D, m_texEnv[0]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	
	glTexCoord2f(1.0, 0.0);
	glVertex3d(-a / 2, a / 2, -a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(a / 2, -a / 2, -a / 2);
	glTexCoord2f(0.0, 0.0);
	glVertex3d(a / 2, a / 2, -a / 2);

	glEnd();

	//back
	glBindTexture(GL_TEXTURE_2D, m_texEnv[1]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-a / 2, a / 2, a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(a / 2, a / 2, a / 2);

	glEnd();

	//left
	glBindTexture(GL_TEXTURE_2D, m_texEnv[2]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(a / 2, a / 2, a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(a / 2, a / 2, -a / 2);

	glEnd();

	//right
	glBindTexture(GL_TEXTURE_2D, m_texEnv[3]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(-a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(-a / 2, a / 2, a / 2);

	glEnd();

	//top
	glBindTexture(GL_TEXTURE_2D, m_texEnv[4]);

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-a / 2, a / 2, -a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(a / 2, a / 2, -a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(a / 2, a / 2, a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(-a / 2, a / 2, a / 2);

	glEnd();

	//bot
	glBindTexture(GL_TEXTURE_2D, m_texEnv[5]);

	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3d(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0.0, 1.0);
	glVertex3d(a / 2, -a / 2, -a / 2);
	glTexCoord2f(1.0, 1.0);
	glVertex3d(a / 2, -a / 2, a / 2);
	glTexCoord2f(1.0, 0.0);
	glVertex3d(-a / 2, -a / 2, a / 2);
	glEnd();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texSpider);

	int longSeg = 2 * nSeg;
	int latSeg = nSeg;

	double dAlpha = ToRad(180.0 / longSeg);
	double dBeta = ToRad(360.0 / latSeg);

	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i <= longSeg; i++)
	{
		double alpha1 =- PI/2 + i * dAlpha;
		double alpha2 = alpha1 + dAlpha;

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= latSeg; j++)
		{
			double x1 = r * cos(alpha1) * cos(j * dBeta);
			double y1 = r * sin(alpha1);
			double z1 = r * cos(alpha1) * sin(j * dBeta);

			double x2 = r * cos(alpha2) * cos(j * dBeta);
			double y2 = r * sin(alpha2);
			double z2 = r * cos(alpha2) * sin(j * dBeta);

			double tx1 = texR * x1 / r + texU;
			double ty1 = texR * z1 / r + texV;

			double tx2 = texR * x2 / r + texU;
			double ty2 = texR * z2 / r + texV;

			glTexCoord2f(tx2, ty2);
			glVertex3d(x2, y2, z2);
			glTexCoord2f(tx1, ty1);
			glVertex3d(x1, y1, z1);
		}
		glEnd();
	}


	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texSpider);

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);

	double dAlpha = 360.0 / nSeg;
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r * cos(i * dAlpha);
		double z = r * sin(i * dAlpha);

		glVertex3f(x, 0, z);
	}

	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0, 1.0, 1.0);
	glTexCoord2f(texU, texV);
	glVertex3f(0.0, h, 0.0);

	dAlpha = 360.0 / nSeg;
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r * cos(i * dAlpha);
		double z = r * sin(i * dAlpha);

		double tx = texR * x / r + texU;
		double ty = texR * z / r + texV;

		glTexCoord2f(tx, ty);
		glVertex3f(x, 0, z);
	}

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg)
{
	glPushMatrix();

	glTranslatef(0, r, 0);
	DrawSphere(r, 2 * nSeg, 0.25, 0.25, 0.24);
	DrawCone(r, h, nSeg, 0.75, 0.75, 0.24);

	glPopMatrix();
}

void CGLRenderer::DrawLeg()
{
	glPushMatrix();

	DrawLegSegment(1, 10, 5);
	glTranslatef(0, 11, 0);
	glRotatef(85, 1.0, 0.0, 0.0);
	DrawLegSegment(1, 15, 5);

	glPopMatrix();
}

void CGLRenderer::DrawSpiderBody()
{
	glPushMatrix();

	//grudi
	glPushMatrix();
	glScalef(1.0, 0.5, 1.0);
	DrawSphere(3, 10, 0.25, 0.25, 0.24);
	glPopMatrix();

	//tail
	glPushMatrix();
	glTranslatef(6.0, 0.0, 0.0);
	glScalef(1.0, 0.8, 1.0);
	DrawSphere(5, 10, 0.25, 0.25, 0.24);
	glPopMatrix();

	//head
	glPushMatrix();
	glTranslatef(-4.0, 0.0, 0.0);
	glScalef(1.0, 0.5, 2.0);
	DrawSphere(2, 10, 0.75, 0.25, 0.24);
	glPopMatrix();
	
	glPopMatrix();
}

void CGLRenderer::DrawSpider()
{
	DrawSpiderBody();

	double angle = 45.0;

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(angle-30*i, 0.0, 1.0, 0);
		glRotatef(45, 1, 0, 0);
		DrawLeg();
		glPopMatrix();
	}

	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(angle - 30 * i, 0.0, 1.0, 0);
		glRotatef(-180, 0, 1, 0);
		glRotatef(45, 1, 0, 0);
		DrawLeg();
		glPopMatrix();
	}
}
