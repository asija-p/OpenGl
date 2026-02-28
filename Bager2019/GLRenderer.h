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
	void UpdateCamera();
	UINT LoadTexture(char* fileName);
	void DrawPolygon(POINTF* points, POINTF* texCoords, int n);
	void DrawExtrudedPolygon(POINTF* points,
		POINTF* texCoords, int n, float zh, float r, float g, float b);
	void DrawBase();
	void DrawBody();
	void DrawArm(double zh);
	void DrawFork();
	void DrawExcavator();
	void DrawEnvCube(double a);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 


public:
	float m_eyex, m_eyey, m_eyez;
	float cameraR, cameraXY, cameraXZ;

	const float Pi = 3.14;
	inline double ToRad(double angle) { return angle * Pi / 180.0; }

	UINT excavator;

	float cab;
	float first;
	float second;
	float fork;

	UINT S[6];
};
