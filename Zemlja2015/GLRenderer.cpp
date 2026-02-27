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

	m_cameraR = 30.0;
	m_cameraAngleXY = 0.0;
	m_cameraAngleXZ = 0.0;

	UpdateCameraPosition();

	// 🔵 Plavi (tirkizni) prstenovi
	m_blueMaterial.PostaviAmbijent(0.0f, 0.0f, 1.0f, 1.0f);
	m_blueMaterial.PostaviDifuznu(0.0f, 0.0f, 1.0f, 1.0f);
	m_blueMaterial.PostaviSpekularnu(1.0f, 1.0f, 1.0f, 1.0f);
	m_blueMaterial.PostaviShininess(16.0f);

	// 🔴 Ljubičasti / crveni prstenovi
	m_redMaterial.PostaviAmbijent(1.0f, 0.0f, 0.0f, 1.0f);
	m_redMaterial.PostaviDifuznu(1.0f, 0.0f, 0.0f, 1.0f);
	m_redMaterial.PostaviSpekularnu(1.0f, 1.0f, 1.0f, 1.0f);
	m_redMaterial.PostaviShininess(16.0f);

	// 🏛 Osnova (postolje)
	m_pedestalMaterial.PostaviAmbijent(0.6f, 0.6f, 0.6f, 1.0f);
	m_pedestalMaterial.PostaviDifuznu(0.8f, 0.8f, 0.8f, 1.0f);
	m_pedestalMaterial.PostaviSpekularnu(0.5f, 0.5f, 0.5f, 1.0f);
	m_pedestalMaterial.PostaviShininess(64.0f);

	// 🏠 Soba (zidovi)
	m_wallMaterial.PostaviAmbijent(0.1f, 0.1f, 0.1f, 1.0f);
	m_wallMaterial.PostaviDifuznu(0.8f, 0.8f, 0.8f, 1.0f);
	m_wallMaterial.PostaviSpekularnu(0.2f, 0.2f, 0.2f, 1.0f);
	m_wallMaterial.PostaviEmisivnu(0.1f, 0.1f, 0.1f, 1.0f);
	m_wallMaterial.PostaviShininess(0.0f);

	m_blueLight.PostaviAmbijent(0, 1, 1, 1);
	m_blueLight.PostaviDifuznu(0, 1, 1, 0);
	m_blueLight.PostaviSpekularnu(0, 1, 1, 1);
	m_blueLight.PostaviEmisivnu(0, 1, 1, 1);
	m_blueLight.PostaviShininess(0);
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
	glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
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
		0.0, 10.0, 0.0,
		0.0, 1.0, 0.0
	);

	SetUpAmbient();
	SetUpDirection();
	PositionalLight1();

	glEnable(GL_LIGHTING);
	
	DrawAxis(20);
	DrawWalls();

	glPushMatrix();
	glRotatef(45, 0, 1, 0);
	DrawPedestal();
	glPopMatrix();

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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, aspect, 1, 500);
	glMatrixMode(GL_MODELVIEW);
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

void CGLRenderer::ZoomView(double r)
{
	m_cameraR += r;

	UpdateCameraPosition();
}

void CGLRenderer::DrawAxis(double width)
{
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(width, 0, 0);
	glEnd();

	glColor3f(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, width, 0);
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, width);
	glEnd();

}

void CGLRenderer::DrawWalls()
{
	m_wallMaterial.IzaberiMaterijal();

	int sides = 4;
	int length = 80;
	int height = 80;
	int step = 1;
	double angle = 0;
	double r = (sqrt(pow(length, 2) + pow(length, 2))) / 2; double dAlpha = ToRad(360.0 / sides);
	int half = length / 2;

	glPushMatrix();

	for (int side = 0; side < 4; side++)
	{

		glNormal3f(0, 0, -1);

		for (int i = 0; i < height; i += step)
		{
			glBegin(GL_QUAD_STRIP);
			for (int j = -half; j <= half; j += step)
			{
				glVertex3f(j, i, half);
				glVertex3f(j, i + step, half);
			}
			glEnd();
		}

		glRotatef(90, 0, 1, 0);
	}

	glPopMatrix();

	//top
	glNormal3f(0, -1, 0);
	for (int i =-half; i <=half; i += step)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = -half; j <= half; j+=step)
		{
			glVertex3f(j, length, i);
			glVertex3f(j, length, i+step);

		}
		glEnd();
	}
	

	//bottom
	glNormal3f(0, 1, 0);
	for (int i = -half; i <= half; i += step)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = -half; j <= half; j += step)
		{
			glVertex3f(j, 0, i);
			glVertex3f(j, 0, i+step);

		}
		glEnd();
	}

}

void CGLRenderer::DrawPedestal()
{
	m_pedestalMaterial.IzaberiMaterijal();

	DrawHalfSphere();
	glTranslatef(0, 4.5, 0);
	DrawCylinder(2, 8, 3);
	glTranslatef(0, 3, 0);
	DrawCylinder(7, 4, 2);
	glTranslatef(0, 2, 0);

	DrawRing(4, 3, false);
	glTranslatef(0, 1, 0);
	DrawRing(3, 2, true);
	glTranslatef(0, 1, 0);
	DrawRing(2, 2, false);
	glTranslatef(0, 1, 0);
	DrawRing(2, 2, true);
	glTranslatef(0, 1, 0);
	DrawRing(2, 3, false);
	glTranslatef(0, 1, 0);
	DrawRing(3, 2, true);
	glTranslatef(0, 1, 0);
	DrawRing(2, 3, false);
	glTranslatef(0, 1, 0);
	DrawRing(3, 4, true);
	glTranslatef(0, 1, 0);
	DrawRing(4, 3, false);
	glTranslatef(0, 1, 0);
	DrawRing(3, 4, true);
	glTranslatef(0, 1, 0);
	DrawRing(4, 3, false);
	glTranslatef(0, 1, 0);
	DrawRing(3, 2, true);
	glTranslatef(0, 1, 0);
	DrawRing(2, 3, false);
	glTranslatef(0, 1, 0);
	DrawRing(3, 4, true);
	
}

void CGLRenderer::DrawCylinder(int r, int nSeg, int h)
{

	double dAlpha = ToRad(360.0 / nSeg);

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++)
	{
		double angle = i * dAlpha;
		double x = r * cos(angle);
		double z = r * sin(angle);

		glNormal3f(cos(angle), 0, sin(angle));
		glVertex3f(x, h, z);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, -1, 0);
	glVertex3f(0, 0, 0);
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r * cos(i * dAlpha);
		double z = r * sin(i * dAlpha);

		glVertex3f(x, 0, z);
	}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 1, 0);
	glVertex3f(0, h, 0);
	for (int i = 0; i <= nSeg; i++)
	{
		double x = r * cos(i * dAlpha);
		double z = r * sin(i * dAlpha);

		glVertex3f(x, h, z);
	}
	glEnd();
}

void CGLRenderer::DrawHalfSphere()
{
	int nSegAlpha = 40;
	int nSegBeta = 40;
	int r = 5;

	double dAlpha = ToRad(90.0 / nSegAlpha);
	double dBeta = ToRad(360.0 / nSegBeta);

	for (int i = 0; i < nSegAlpha; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= nSegBeta; j++)
		{
			double x = r * cos(dAlpha * (i + 1)) * cos(dBeta * j);
			double y = r * sin(dAlpha * (i + 1));
			double z = r * cos(dAlpha * (i + 1)) * sin(dBeta * j);

			glNormal3f(x / r, y / r, z / r);
			glVertex3f(x, y, z);

			x = r * cos(dAlpha * i) * cos(dBeta * j);
			y = r * sin(dAlpha * i);
			z = r * cos(dAlpha * i) * sin(dBeta * j);

			glNormal3f(x / r, y / r, z / r);
			glVertex3f(x, y, z);
		}
		glEnd();
	}
}

void CGLRenderer::DrawRing(int r1, int r2, bool red)
{
	if (red)
		m_redMaterial.IzaberiMaterijal();
	else
		m_blueMaterial.IzaberiMaterijal();

	int nSeg = 20;
	double dBeta = ToRad(360.0 / nSeg);

	int h = 1;

	double r = abs(r1 - r2);
	double L = sqrt(pow(r, 2) + pow(h, 2));
	double ny = r / L;
	double nr = h / L;
	int v = r2 > r1 ? -1 : 1;

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++)
	{
		double angle = i * dBeta;

		double x1 = r1 * cos(0) * cos(angle);
		double y1 = r1 * sin(0);
		double z1 = r1 * cos(0) * sin(angle);

		double x2 = r2 * cos(0) * cos(angle);
		double y2 = r2 * sin(0);
		double z2 = r2 * cos(0) * sin(angle);


		double nx = nr * cos(angle);
		double nz = nr * sin(angle);
		
		glNormal3f(nx, ny, nz);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2 + h, z2);
	}
	glEnd();

	if (normale)
	{
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for (int i = 0; i <= nSeg; i++)
		{
			double angle = i * dBeta;

			double x1 = r1 * cos(0) * cos(angle);
			double y1 = r1 * sin(0);
			double z1 = r1 * cos(0) * sin(angle);

			double x2 = r2 * cos(0) * cos(angle);
			double y2 = r2 * sin(0);
			double z2 = r2 * cos(0) * sin(angle);


			double nx = nr * cos(angle);
			double nz = nr * sin(angle);

			glVertex3f(x1, y1, z1);
			glVertex3f(x1+nx, y1+ny*v, z1+nz);

			glVertex3f(x2, y2 + h, z2);
			glVertex3f(x2+nx, y2 + h+ny*v, z2+nz);
		}
		glEnd();
	}
}

void CGLRenderer::ShowNormale()
{
	normale = !normale;
}

void CGLRenderer::SetUpAmbient()
{
	GLfloat ambijentalno_svetlo[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambijentalno_svetlo);

}

void CGLRenderer::SetUpDirection()
{
	GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };		// Ambijentalna komponenta izvora
	GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };		// Jaka bela svetlost
	GLfloat light_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };		// Beli odsjaj

	GLfloat light_position[] = { 13.0f, 39.0f, 13.0f, 0.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 180.0f);


	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

	glEnable(GL_LIGHT1);
}

void CGLRenderer::DrawSphere(int r)
{
	int nSegAlpha = 40;
	int nSegBeta = 40;

	double dAlpha = ToRad(1800.0 / nSegAlpha);
	double dBeta = ToRad(360.0 / nSegBeta);

	for (int i = 0; i < nSegAlpha; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= nSegBeta; j++)
		{
			double x = r * cos(dAlpha * (i + 1)) * cos(dBeta * j);
			double y = r * sin(dAlpha * (i + 1));
			double z = r * cos(dAlpha * (i + 1)) * sin(dBeta * j);

			glNormal3f(x / r, y / r, z / r);
			glVertex3f(x, y, z);

			x = r * cos(dAlpha * i) * cos(dBeta * j);
			y = r * sin(dAlpha * i);
			z = r * cos(dAlpha * i) * sin(dBeta * j);

			glNormal3f(x / r, y / r, z / r);
			glVertex3f(x, y, z);
		}
		glEnd();
	}
}

void CGLRenderer::PositionalLight1()
{
	float light_ambient[] = { 0.0, 1.0, 1.0, 1.0 };
	float light_diffuse[] = { 0.0, 1.0, 1.0, 1.0 };
	float light_specular[] = { 0.0, 1.0, 1.0, 1.0 };

	GLfloat light_position[] = { 20, 15, 0,1 };

	// boja i intenzitet svetlosti
	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);

	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 1.0f);

	glLightfv(GL_LIGHT2, GL_POSITION, light_position);

	GLfloat spot_direction[] = { -1, 0, 0 };
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
	// slabljenje
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0);

	// usmerenje izvora
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 35);

	if (normale) {
		glPushMatrix();
		m_blueLight.IzaberiMaterijal();
		glTranslatef(30, 50, 0);
		DrawSphere(1);
		glPopMatrix();
		glEnable(GL_LIGHT2);
	}
	else
		glDisable(GL_LIGHT2);
}
