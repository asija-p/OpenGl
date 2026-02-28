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

	cameraR = 20;
	cameraXY = 30;
	cameraXZ = 30;

	cab = 0;
	first = 0;
	second = 0;
	fork = 0;

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

	excavator = LoadTexture("excavator.png");
	S[0] = LoadTexture("front.jpg");
	S[1] = LoadTexture("left.jpg");
	S[2] = LoadTexture("right.jpg");
	S[3] = LoadTexture("back.jpg");
	S[4] = LoadTexture("top.jpg");
	S[5] = LoadTexture("bot.jpg");

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
		0, 5, 0,
		0, 1, 0);

	

	glEnable(GL_TEXTURE_2D);
	glTranslatef(0, 50, 0);
	DrawEnvCube(100);
	glTranslatef(0, -50, 0);
	//DrawBase();
	//DrawBody();
	//DrawArm(5);
	//DrawFork();
	
	DrawAxes();
	DrawExcavator();

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

	gluPerspective(55, (double)w / (double)h, 0.1, 200);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	
	glDeleteTextures(1, &excavator);
	glDeleteTextures(6, S);

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

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(10, 0, 0);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);

	glEnd();

	glEnable(GL_TEXTURE_2D);
}

void CGLRenderer::UpdateCamera()
{
	if (cameraXY > 90) cameraXY = 90;
	if (cameraXY < -90) cameraXY = -90;
	if (cameraR > 50) cameraR = 50;
	if (cameraR < 8) cameraR = 8;


	float radXY = ToRad(cameraXY);
	float radXZ = ToRad(cameraXZ);

	m_eyex = cameraR * cos(radXY) * cos(radXZ);
	m_eyey = cameraR * sin(radXY);
	m_eyez = cameraR * cos(radXY) * sin(radXZ);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return texID;
}

void CGLRenderer::DrawPolygon(POINTF* points, POINTF* texCoords, int n)
{
	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2d(texCoords[0].x, texCoords[0].y);
	glVertex3f(points[0].x, points[0].y, 0);
	for (int i = 1; i < n; i++)
	{
		glTexCoord2d(texCoords[i].x, texCoords[i].y);
		glVertex3f(points[i].x, points[i].y, 0);

	}
	glTexCoord2d(texCoords[1].x, texCoords[1].y);
	glVertex3f(points[1].x, points[1].y, 0);

	glEnd();
}

void CGLRenderer::DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, excavator);
	glColor3f(1.0, 1.0, 1.0);
	DrawPolygon(points, texCoords, n);

	glPushMatrix();
	glTranslatef(0, 0, zh);
	DrawPolygon(points, texCoords, n);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	glColor3f(r, g, b);
	glBegin(GL_QUAD_STRIP);

	for (int i = 1; i < n; i++)
	{
		glVertex3f(points[i].x, points[i].y, 0);
		glVertex3f(points[i].x, points[i].y, zh);
	}
	glVertex3f(points[1].x, points[1].y, 0);
	glVertex3f(points[1].x, points[1].y, zh);

	glEnd();


	
}

void CGLRenderer::DrawBase()
{
	float texStep = 1.0 / 16.0;
	float realStep = 0.5;

	POINTF* texCoords = new POINTF[9];
	POINTF* realCoords = new POINTF[9];

	texCoords[0].x = 8 * texStep;
	texCoords[0].y = 14 * texStep;

	texCoords[1].x = 0 * texStep;
	texCoords[1].y = 13 * texStep;

	texCoords[2].x = 1 * texStep;
	texCoords[2].y = 12 * texStep;

	texCoords[3].x = 15 * texStep;
	texCoords[3].y = 12 * texStep;

	texCoords[4].x = 16 * texStep;
	texCoords[4].y = 13 * texStep;

	texCoords[5].x = 16 * texStep;
	texCoords[5].y = 15 * texStep;

	texCoords[6].x = 15 * texStep;
	texCoords[6].y = 16 * texStep;

	texCoords[7].x = 1 * texStep;
	texCoords[7].y = 16 * texStep;

	texCoords[8].x = 0 * texStep;
	texCoords[8].y = 15 * texStep;

	//REAL
	realCoords[0].x = 0;
	realCoords[0].y = 0;

	realCoords[1].x = -8 * realStep;
	realCoords[1].y = 1 * realStep;

	realCoords[2].x = -7 * realStep;
	realCoords[2].y = 2 * realStep;

	realCoords[3].x = 7 * realStep;
	realCoords[3].y = 2 * realStep;

	realCoords[4].x = 8 * realStep;
	realCoords[4].y = 1 * realStep;

	realCoords[5].x = 8 * realStep;
	realCoords[5].y = -1 * realStep;

	realCoords[6].x = 7 * realStep;
	realCoords[6].y = -2 * realStep;

	realCoords[7].x = -7 * realStep;
	realCoords[7].y = -2 * realStep;

	realCoords[8].x = -8 * realStep;
	realCoords[8].y = -1 * realStep;

	DrawExtrudedPolygon(realCoords, texCoords, 9, 5, 0, 0, 0);
}

void CGLRenderer::DrawBody()
{
	float texStep = 1.0 / 16.0;
	float realStep = 0.5;

	POINTF* texCoords = new POINTF[6];
	POINTF* realCoords = new POINTF[6];

	texCoords[0].x = 12 * texStep;
	texCoords[0].y = 4 * texStep;

	texCoords[1].x = 8 * texStep;
	texCoords[1].y = 0 * texStep;

	texCoords[2].x = 12 * texStep;
	texCoords[2].y = 0 * texStep;

	texCoords[3].x = 16 * texStep;
	texCoords[3].y = 4 * texStep;

	texCoords[4].x = 16 * texStep;
	texCoords[4].y = 8 * texStep;

	texCoords[5].x = 8 * texStep;
	texCoords[5].y = 8 * texStep;

	//REAL
	realCoords[0].x = 4 * realStep;
	realCoords[0].y = 4 * realStep;

	realCoords[1].x = 0;
	realCoords[1].y = 8*realStep;

	realCoords[2].x = 4 * realStep;
	realCoords[2].y = 8 * realStep;

	realCoords[3].x = 8 * realStep;
	realCoords[3].y = 4 * realStep;

	realCoords[4].x = 8 * realStep;
	realCoords[4].y = 0 * realStep;

	realCoords[5].x = 0 * realStep;
	realCoords[5].y = 0 * realStep;

	DrawExtrudedPolygon(realCoords, texCoords, 6, 4, 0.96, 0.5, 0.12);
}

void CGLRenderer::DrawArm(double zh)
{
	float texStep = 1.0 / 16.0;
	float realStep = 0.5;

	POINTF* texCoords = new POINTF[9];
	POINTF* realCoords = new POINTF[9];

	texCoords[0].x = 8 * texStep;
	texCoords[0].y = 10 * texStep;

	texCoords[1].x = 0 * texStep;
	texCoords[1].y = 9 * texStep;

	texCoords[2].x = 1 * texStep;
	texCoords[2].y = 8 * texStep;

	texCoords[3].x = 4 * texStep;
	texCoords[3].y = 8 * texStep;

	texCoords[4].x = 16 * texStep;
	texCoords[4].y = 9 * texStep;

	texCoords[5].x = 16 * texStep;
	texCoords[5].y = 11 * texStep;

	texCoords[6].x = 4 * texStep;
	texCoords[6].y = 12 * texStep;

	texCoords[7].x = 1 * texStep;
	texCoords[7].y = 12 * texStep;

	texCoords[8].x = 0 * texStep;
	texCoords[8].y = 11 * texStep;

	//REAL
	realCoords[0].x = 0 * realStep;
	realCoords[0].y = 0 * realStep;

	realCoords[1].x = -8 * realStep;
	realCoords[1].y = 1 * realStep;

	realCoords[2].x = -7 * realStep;
	realCoords[2].y = 2 * realStep;

	realCoords[3].x = -4 * realStep;
	realCoords[3].y = 2 * realStep;

	realCoords[4].x = 8 * realStep;
	realCoords[4].y = 1 * realStep;

	realCoords[5].x = 8 * realStep;
	realCoords[5].y = -1 * realStep;

	realCoords[6].x = -4 * realStep;
	realCoords[6].y = -2 * realStep;

	realCoords[7].x = -7 * realStep;
	realCoords[7].y = -2 * realStep;

	realCoords[8].x = -8 * realStep;
	realCoords[8].y = -1 * realStep;

	DrawExtrudedPolygon(realCoords, texCoords, 9, zh, 0.96, 0.5, 0.12);
}

void CGLRenderer::DrawFork()
{
	float texStep = 1.0 / 16.0;
	float realStep = 0.5;

	POINTF* texCoords = new POINTF[7];
	POINTF* realCoords = new POINTF[7];

	texCoords[0].x = 4 * texStep;
	texCoords[0].y = 3 * texStep;

	texCoords[1].x = 0 * texStep;
	texCoords[1].y = 1 * texStep;

	texCoords[2].x = 1 * texStep;
	texCoords[2].y = 0 * texStep;

	texCoords[3].x = 7 * texStep;
	texCoords[3].y = 0 * texStep;

	texCoords[4].x = 8 * texStep;
	texCoords[4].y = 1 * texStep;

	texCoords[5].x = 8 * texStep;
	texCoords[5].y = 6 * texStep;

	texCoords[6].x = 0 * texStep;
	texCoords[6].y = 6 * texStep;


	//REAL
	realCoords[0].x = 4 * realStep;
	realCoords[0].y = 3 * realStep;

	realCoords[1].x = 0 * realStep;
	realCoords[1].y = 5 * realStep;

	realCoords[2].x = 1 * realStep;
	realCoords[2].y = 6 * realStep;

	realCoords[3].x = 7 * realStep;
	realCoords[3].y = 6 * realStep;

	realCoords[4].x = 8 * realStep;
	realCoords[4].y = 5 * realStep;

	realCoords[5].x = 8 * realStep;
	realCoords[5].y = 0 * realStep;

	realCoords[6].x = 0 * realStep;
	realCoords[6].y = 0 * realStep;

	DrawExtrudedPolygon(realCoords, texCoords, 7, 1, 0.7, 0.7, 0.7);
}

void CGLRenderer::DrawExcavator()
{
	glTranslatef(0, 1, -2);
	DrawBase();

	glTranslatef(0, 1, 0);
	glTranslatef(-2, 0, 0);
	glTranslatef(0, 0, 1);
	
	glTranslatef(2, 0, 2);
	glRotatef(cab, 0, 1, 0);
	glTranslatef(-2, 0, -2);

	DrawBody();

	glTranslatef(0, 0, -1);
	glTranslatef(0, 1, 0);
	glTranslatef(6.5, 0, 0);

	glTranslatef(-3, 0, 0);
	glRotatef(90, 0, 0, 1);
	glRotatef(first, 0, 0, 1);
	glTranslatef(3, 0, 0);

	DrawArm(1.0);

	glTranslatef(-3, 0, 0);
	glRotatef(-90, 0, 0, 1);

	glTranslatef(0, 6.5, 0);

	glRotatef(second, 0, 0, 1);
	
	glTranslatef(3, 0, 0);
	glTranslatef(0, 0, -0.25);
	DrawArm(1.5);
	
	glTranslatef(3.5, 0, 0);
	glRotatef(fork, 0, 0, 1);

	glTranslatef(-1, -1, 0.25);
	
	DrawFork();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, S[4]);
	glColor3f(1, 1, 1);
	float len = a / 2.0;

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(len, len, len);
	glTexCoord2f(0, 1);
	glVertex3f(-len, len, len);
	glTexCoord2f(1, 1);
	glVertex3f(-len, len, -len);
	glTexCoord2f(1, 0);
	glVertex3f(len, len, -len);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, S[5]);
	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(len, -len, -len);
	glTexCoord2f(0, 1);
	glVertex3f(-len, -len, -len);
	glTexCoord2f(1, 1);
	glVertex3f(-len, -len, len);
	glTexCoord2f(1, 0);
	glVertex3f(len, -len, len);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, S[0]);
	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(-len, len, len);
	glTexCoord2f(0, 0);
	glVertex3f(len, len, len);
	glTexCoord2f(0, 1);
	glVertex3f(len, -len, len);
	glTexCoord2f(1, 1);
	glVertex3f(-len, -len, len);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, S[1]);
	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(-len, len, -len);
	glTexCoord2f(0, 0);
	glVertex3f(-len, len, len);
	glTexCoord2f(0, 1);
	glVertex3f(-len, -len, len);
	glTexCoord2f(1, 1);
	glVertex3f(-len, -len, -len);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, S[2]);
	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(len, len, len);
	glTexCoord2f(0, 0);
	glVertex3f(len, len, -len);
	glTexCoord2f(0, 1);
	glVertex3f(len, -len, -len);
	glTexCoord2f(1, 1);
	glVertex3f(len, -len, len);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, S[3]);
	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(len, len, -len);
	glTexCoord2f(0, 0);
	glVertex3f(-len, len, -len);
	glTexCoord2f(0, 1);
	glVertex3f(-len, -len, -len);
	glTexCoord2f(1, 1);
	glVertex3f(len, -len, -len);
	glEnd();

	glDisable(GL_CULL_FACE);
}
