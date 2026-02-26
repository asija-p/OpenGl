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

	//crtanje
	void DrawAxes();
	void DrawTriangle(float d1, float d2, float rep);
	void DrawShip();
	void DrawSpaceCube(double a);

	//kamera
	void UpdateCamera();
	void RotateView(double dx, double dy);
	void ZoomView(double dr);

	//helpers
	UINT LoadTexture(char* fileName);

	//svetla
	void ShowLight();
	void ShowNormale();
protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	//kamera
	double m_eyex, m_eyey, m_eyez;
	double cameraR, cameraXY, cameraXZ;

	//konverzija
	const double PI = 3.14159265358979323846;
	inline double ToRad(double angle) { return angle * PI / 180.0; }

	//teksture
	UINT ship;
	UINT bg[6];

	//svetla
	bool m_bLight;

	//
	bool m_norm;

};
