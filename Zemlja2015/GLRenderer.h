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
	UINT LoadTexture(char* fileName);
	void DrawTriangle();

	//drawing
	void DrawPatch(double R, int n);
	void DrawEarth(double R, int tes);
	void DrawMoon(double R, int tes);
	void DrawSpace(double R, int tes);

	//camera
	void CameraUpdate();

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	UINT M[6];
	UINT S[6];
	UINT TSC[6];

public:
	float m_eyex, m_eyey, m_eyez;
	float cameraR, cameraXY, cameraXZ;

	const double PI = 3.14159265358979323846;
	inline double ToRad(double angle) { return angle * PI / 180.0; }

	bool normale;
	bool m_light;
	float moon_rot;
};
