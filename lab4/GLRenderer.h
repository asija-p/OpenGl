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

	//dodate
	void UpdateCameraPosition();
	void RotateView(double dx, double dy);
	void DrawSphere(double r, int nSegAlpha, int nSegBeta);
	void DrawCylinder(double h, double r1, double r2, int nSeg);
	void DrawCone(double h, double r, int nSeg);
	void DrawAxis(double width);
	void DrawGrid(double width, double height, int nSegW, int  nSegH);
	void DrawFigure(double angle);
	void RotateYellow(double angle);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	//kamera
	double m_cameraR;//radius
	double m_cameraAngleXY; //horizontalni ugao
	double m_cameraAngleXZ; //vertikalni ugao

	//zuti
	double y_rotAngle;

	//oko
	double m_eyex, m_eyey, m_eyez;

	//konverzija
	const double PI = 3.14159265358979323846;
	inline double ToRad(double angle) { return angle * PI / 180.0; }
};
