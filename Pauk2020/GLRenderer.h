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
	void DrawAxes();
	void RotateView(double dx, double dy);
	void ZoomView(double dx);
	void UpdateCameraPosition();
	UINT LoadTexture(char* fileName);
	void DrawEnvCube(double a);
	void DrawSphere(double r, int nSeg, double textU, double texV, double texR);
	void DrawCone(double r, double h, int nSeg, double texU, double texV, double texR);
	void DrawLegSegment(double r, double h, int nSeg);
	void DrawLeg();
	void DrawSpiderBody();
	void DrawSpider();


protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	//kamera
	double m_cameraR;
	double m_cameraAngleXY;
	double m_cameraAngleXZ;

	//oko
	double m_eyex, m_eyey, m_eyez;

	//konverzija
	const double PI = 3.14159265358979323846;
	inline double ToRad(double angle) { return angle * PI / 180.0; }

	//teksture
	UINT m_texSpider;
	UINT m_texEnv[6];

};
