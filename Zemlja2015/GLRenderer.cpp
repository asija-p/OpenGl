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
	m_hrc = NULL;

	m_eyex = 0.0;
	m_eyey = 0.0;
	m_eyez = 0.0;

	cameraR = 50;
	cameraXY = 0;
	cameraXZ = 0;

	normale = false;
	m_light = false;
	moon_rot = 0;

	CameraUpdate();
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

	TSC[0] = LoadTexture("TSC0.jpg");
	TSC[1] = LoadTexture("TSC1.jpg");
	TSC[2] = LoadTexture("TSC2.jpg");
	TSC[3] = LoadTexture("TSC3.jpg");
	TSC[4] = LoadTexture("TSC4.jpg");
	TSC[5] = LoadTexture("TSC5.jpg");

	M[0] = LoadTexture("M0.jpg");
	M[1] = LoadTexture("M1.jpg");
	M[2] = LoadTexture("M2.jpg");
	M[3] = LoadTexture("M3.jpg");
	M[4] = LoadTexture("M4.jpg");
	M[5] = LoadTexture("M5.jpg");

	S[0] = LoadTexture("S0.jpg");
	S[1] = LoadTexture("S1.jpg");
	S[2] = LoadTexture("S2.jpg");
	S[3] = LoadTexture("S3.jpg");
	S[4] = LoadTexture("S4.jpg");
	S[5] = LoadTexture("S5.jpg");

	glEnable(GL_TEXTURE_2D);

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
		0, 0, 0,
		0, 1, 0);
	DrawTriangle();

	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glDisable(GL_LIGHTING);

	DrawSpace(100, 1);

	if (m_light)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}

	DrawEarth(10, 1);
	glRotatef(moon_rot, 0, 1, 0);
	glTranslatef(0, 0, 50);
	DrawMoon(3, 1);

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

	gluPerspective(40, (double)w / (double)h, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 

	glDeleteTextures(6, TSC);
	glDeleteTextures(6, M);
	glDeleteTextures(6, S);

	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

UINT CGLRenderer::LoadTexture(char* fileName)
{
	UINT texId;
	DImage img;
	img.Load(CString(fileName));

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texId;
;
}

void CGLRenderer::DrawTriangle()
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLES);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(1, 0, 0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, 1, 0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, 1);

	glEnd();

	glEnable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawPatch(double R, int n)
{
	float step = 2.0 / n;

	for (float i = -1; i < 1; i += step) //y
	{
		float i2 = i + step;
		glBegin(GL_TRIANGLE_STRIP);
		for (float j = -1; j <= 1; j += step) //x
		{
			float fi = atan(j);
			float teta = atan(i * cos(fi));

			double x1 = R * cos(teta) * sin(fi);
			double z1 = R * cos(teta) * cos(fi);
			double y1 = R * sin(teta);

			glTexCoord2f((j + 1) / 2, 1.0 -(i + 1) / 2.0);
			glNormal3f(x1 / R, y1 / R, z1 / R);
			glVertex3f(x1, y1, z1);

			float teta2 = atan(i2 * cos(fi));

			double x2 = R * cos(teta2) * sin(fi);
			double z2 = R * cos(teta2) * cos(fi);
			double y2 = R * sin(teta2);

			glTexCoord2f((j + 1) / 2, 1.0 - (i2 + 1) / 2.0);
			glNormal3f(x2 / R, y2 / R, z2 / R);
			glVertex3f(x2, y2, z2);
		}
		glEnd();
	}	

	if (normale)
	{
		glDisable(GL_TEXTURE_2D);
		for (float i = -1; i < 1; i += step) //y
		{
			float i2 = i + step;
			glBegin(GL_LINES);

			glColor3f(1, 1, 0);
			for (float j = -1; j <= 1; j += step) //x
			{
				float fi = atan(j);
				float teta = atan(i * cos(fi));

				double x1 = R * cos(teta) * sin(fi);
				double z1 = R * cos(teta) * cos(fi);
				double y1 = R * sin(teta);

				glVertex3f(x1, y1, z1);
				float len = R * 0.2f;
				glVertex3f(x1 + (x1 / R) * len, y1 + (y1 / R) * len, z1 + (z1 / R) * len);

				float teta2 = atan(i2 * cos(fi));

				double x2 = R * cos(teta2) * sin(fi);
				double z2 = R * cos(teta2) * cos(fi);
				double y2 = R * sin(teta2);

				glVertex3f(x2, y2, z2);
				glVertex3f(x2 + (x2 / R) * len, y2 + (y2 / R) * len, z2 + (z2 / R) * len);
			}
			glEnd();
		}
		glEnable(GL_TEXTURE_2D);
	}
}

void CGLRenderer::DrawEarth(double R, int tes)
{
	glBindTexture(GL_TEXTURE_2D, TSC[0]);
	glColor3f(1.0, 1.0, 1.0);
	DrawPatch(R, 10);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, TSC[1]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);

	glBindTexture(GL_TEXTURE_2D, TSC[2]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);

	glBindTexture(GL_TEXTURE_2D, TSC[3]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, TSC[4]);
	glRotatef(-90, 1, 0, 0);
	DrawPatch(R, 10);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, TSC[5]);
	glRotatef(90, 1, 0, 0);
	DrawPatch(R, 10);
	glPopMatrix();

}

void CGLRenderer::DrawMoon(double R, int tes)
{
	glBindTexture(GL_TEXTURE_2D, M[0]);
	glColor3f(1.0, 1.0, 1.0);
	DrawPatch(R, 10);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, M[1]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);

	glBindTexture(GL_TEXTURE_2D, M[2]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);

	glBindTexture(GL_TEXTURE_2D, M[3]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, M[4]);
	glRotatef(-90, 1, 0, 0);
	DrawPatch(R, 10);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, M[5]);
	glRotatef(90, 1, 0, 0);
	DrawPatch(R, 10);
	glPopMatrix();
}

void CGLRenderer::DrawSpace(double R, int tes)
{
	glBindTexture(GL_TEXTURE_2D, S[0]);
	glColor3f(1.0, 1.0, 1.0);
	DrawPatch(R, 10);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, S[1]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);

	glBindTexture(GL_TEXTURE_2D, S[2]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);

	glBindTexture(GL_TEXTURE_2D, S[3]);
	glRotatef(90, 0, 1, 0);
	DrawPatch(R, 10);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, S[4]);
	glRotatef(-90, 1, 0, 0);
	DrawPatch(R, 10);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, S[5]);
	glRotatef(90, 1, 0, 0);
	DrawPatch(R, 10);
	glPopMatrix();
}

void CGLRenderer::CameraUpdate()
{
	if (cameraXY > 90) cameraXY = 90;
	if (cameraXY < -90) cameraXY = -90;

	double alpha = ToRad(cameraXY);
	double beta = ToRad(cameraXZ);

	m_eyex = cameraR * cos(alpha) * cos(beta);
	m_eyey = cameraR * sin(alpha);
	m_eyez = cameraR * cos(alpha) * sin(beta);
}


