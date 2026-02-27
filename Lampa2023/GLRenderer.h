#pragma once

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

	//helpers
	void UpdateCamera();
	UINT LoadTexture(char* fileName);

	//drawings
	void DrawAxes();
	void DrawEnvCube(double a);
	void DrawCylinder(double r1, double r2, double h, int nSeg, int texMode, bool bIsOpen);
	void DrawLampBase();
	void DrawLampArm();
	void DrawLampHead();
	void DrawLamp();

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	const float Pi = 3.14159;
	inline float ToRad(float angle) { return angle * Pi / 180.0; };

public:
	float m_eyex, m_eyey, m_eyez;
	float cameraR, cameraXY, cameraXZ;
	float m_alpha, m_beta, m_dist;

	UINT lamp;
	UINT side;
	UINT top;
	UINT bot;

	float first;
	float second;
	float third;
};
