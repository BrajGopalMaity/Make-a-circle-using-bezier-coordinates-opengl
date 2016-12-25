// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <sstream>
#include <fstream>
#include <conio.h>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
// Include AntTweakBar
#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

typedef struct Vertex {
	float XYZW[4];
	float RGBA[4];
	void SetCoords(float *coords) {
		XYZW[0] = coords[0];
		XYZW[1] = coords[1];
		XYZW[2] = coords[2];
		XYZW[3] = coords[3];
	}
	void SetColor(float *color) {
		RGBA[0] = color[0];
		RGBA[1] = color[1];
		RGBA[2] = color[2];
		RGBA[3] = color[3];
	}
};

// ATTN: USE POINT STRUCTS FOR EASIER COMPUTATIONS
typedef struct point {
	float x, y, z;
	point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z) {};
	point(float *coords) : x(coords[0]), y(coords[1]), z(coords[2]) {};
	point operator -(const point& a)const {
		return point(x - a.x, y - a.y, z - a.z);
	}
	point operator +(const point& a)const {
		return point(x + a.x, y + a.y, z + a.z);
	}
	point operator *(const float& a)const {
		return point(x*a, y*a, z*a);
	}
	point operator /(const float& a)const {
		return point(x / a, y / a, z / a);
	}
	float* toArray() {
		float array[] = { x, y, z, 1.0f };
		return array;
	}
};

// function prototypes
int initWindow(void);
void initOpenGL(void);
void createVAOs(Vertex[], unsigned short[], size_t, size_t, int);
void createObjects(void);
void pickVertex(void);
void moveVertex(void);
void drawScene(void);
void cleanup(void);
void drawTangent(void);
void drawNormal(void);
void drawBinormal(void);
void drawTNB(void);
static void mouseCallback(GLFWwindow*, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;
const GLuint window_width = 1024, window_height = 768;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;
glm::mat4 gViewMatrix2;

GLuint gPickedIndex;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;


// ATTN: INCREASE THIS NUMBER AS YOU CREATE NEW OBJECTS
const GLuint NumObjects = 11;	// number of different "objects" to be drawn
GLuint VertexArrayId[NumObjects] = { 0,1,2,3,4,5,6,7,8,9,10 };
GLuint VertexBufferId[NumObjects] = { 0,1,2,3,4,5,6,7,8,9,10 };
GLuint IndexBufferId[NumObjects] = { 0,1,2,3,4,5,6,7,8,9,10 };
size_t NumVert[NumObjects] = { 0,1,2,3,4,5,6,7,8,9,10 };

GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorArrayID;
GLuint pickingColorID;
GLuint LightID;
int cut=0;
int shift = 0;
int key5 = 0;
int point = 0;
glm::vec3 rad;
glm::vec3 tang;
glm::vec3 norm;
glm::vec3 bino;

// Define objects
Vertex Vertices[] =
{
	{ { 1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 0
	{ { 0.0f, 1.41f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, //1
	{ { -1.0f, 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 2
	{ { -1.41f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 3
	{ { -1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 4
	{ { 0.0f, -1.41f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, //5
	{ { 1.0f, -1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } }, // 6
	{ { 1.41f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },  //7

};

Vertex Close[] =
{
	{ { 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 0
	{ { 0.0f, 1.41f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, //1
	{ { -1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 2
	{ { -1.41f, 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 3
	{ { -1.0f, -1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 4
	{ { 0.0f, -1.41f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, //5
	{ { 1.0f, -1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } }, // 6
	{ { 1.41f, 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f, 1.0f } },  //7

};

Vertex Tangent[2];
Vertex Normal[2];
Vertex Binormal[2];

Vertex TangentV[240];
Vertex NormalV[240];
Vertex BinormalV[240];
Vertex nml[240];


unsigned short InTangent[] = {
	0,1
};

unsigned short InNormal[] = {
	0,1
};

unsigned short InBinormal[] = {
	0,1
};

unsigned short Indices[] = {
	0, 1, 2, 3, 4, 5, 6, 7
};

unsigned short lineIndices[] = {
	0, 1, 2, 3, 4, 5, 6, 7
};


const size_t IndexCount = sizeof(Indices) / sizeof(unsigned short);
// ATTN: DON'T FORGET TO INCREASE THE ARRAY SIZE IN THE PICKING VERTEX SHADER WHEN YOU ADD MORE PICKING COLORS
float pickingColor[IndexCount] = { 0 / 255.0f, 1 / 255.0f, 2 / 255.0f, 3 / 255.0f,4 / 255.0f, 5 / 255.0f, 6 / 255.0f, 7 / 255.0f };

// ATTN: ADD YOU PER-OBJECT GLOBAL ARRAY DEFINITIONS HERE

int m = 0;
int k = 8 * pow(2, m);


//std::vector<Vertex> newVertices;
Vertex Vertices1[16];
Vertex Vertices2[32];
Vertex Vertices3[64];
Vertex Vertices4[128];
Vertex c[32], t[240], Q[5];
//std::vector<unsigned short> newIndices;
unsigned short Indices1[16];
unsigned short Indices2[32];
unsigned short Indices3[64];
unsigned short Indices4[128];
unsigned short IndicesC[32];
unsigned short IndicesT[240];
int bezier = 0;
void createIndices(void) {
	for (int i = 0; i < 240; i++) {
		if (i < 16) {
			Indices1[i] = i;
			Indices2[i] = i;
			Indices3[i] = i;
			Indices4[i] = i;
			IndicesC[i] = i;
			IndicesT[i] = i;
		}
		else if (i < 32) {
			Indices2[i] = i;
			Indices3[i] = i;
			Indices4[i] = i;
			IndicesC[i] = i;
			IndicesT[i] = i;
		}
		else if (i < 64) {
			Indices3[i] = i;
			Indices4[i] = i;
			IndicesT[i] = i;
		}
		else if (i < 128) {
			Indices4[i] = i;
			IndicesT[i] = i;
		}
		else if (i < 240)
			IndicesT[i] = i;
	}

}
void createBezierCurve() {

	int i, j, u, k = 0;
	for (i = 0; i < 8; i++, k = k + 4) {
		for (j = 0; j < 3; j++) {
			c[k].XYZW[j] = (Vertices[(8 + i - 2) % 8].XYZW[j] + Vertices[(8 + i + 1) % 8].XYZW[j] + 11.0f * (Vertices[(8 + i - 1) % 8].XYZW[j]) + 11.0f * (Vertices[(8 + i) % 8].XYZW[j])) / 24.0f;
			c[k + 1].XYZW[j] = (4.0f * (Vertices[(8 + i - 1) % 8].XYZW[j]) + 7.0f * (Vertices[(8 + i) % 8].XYZW[j]) + Vertices[(8 + i + 1) % 8].XYZW[j]) / 12.0f;
			c[k + 2].XYZW[j] = (4.0f * (Vertices[(8 + i - 1) % 8].XYZW[j]) + 16.0f * (Vertices[(8 + i) % 8].XYZW[j]) + 4.0f * (Vertices[(8 + i + 1) % 8].XYZW[j])) / 24.0f;
			c[k + 3].XYZW[j] = (Vertices[(8 + i - 1) % 8].XYZW[j] + 7.0f * (Vertices[(8 + i) % 8].XYZW[j]) + 4.0f * (Vertices[(8 + i + 1) % 8].XYZW[j])) / 12.0f;

		}
		//printf("%f %f %f %f ", c[k].XYZW[0], c[k+1].XYZW[0], c[k+2].XYZW[0], c[k+3].XYZW[0]);
	}
	for (i = 0; i < 32; i++) {
		c[i].XYZW[3] = 1.0f;
		c[i].RGBA[0] = 1.0f;
		c[i].RGBA[1] = 0.0f;
		c[i].RGBA[2] = 0.0f;
		c[i].RGBA[3] = 1.0f;
	}
	int p = 0;
	float temp;
	for (i = 0; i < 32; i = i + 4) {
		for (u = 0; u < 30; u++) {
			temp = u / 30.0f;
			for (j = 0; j < 5; j++) {
				Q[j].XYZW[0] = c[i + j].XYZW[0];
				Q[j].XYZW[1] = c[i + j].XYZW[1];
				Q[j].XYZW[2] = c[i + j].XYZW[2];
			}
			TangentV[p].XYZW[0] = (4.0f*temp*temp*temp*Q[4].XYZW[0] + Q[3].XYZW[0] * (12.0f*temp*temp - 16.0f*temp*temp*temp) + Q[2].XYZW[0] * (12.0f*temp - 36.0f*temp*temp + 24.0f*temp*temp*temp) + Q[1].XYZW[0] * (4.0f - 24.0f * temp + 36.0f * temp*temp - 16.0f * temp*temp*temp) + Q[0].XYZW[0] * (4.0f * temp*temp*temp - 12.0f * temp*temp + 12.0f * temp - 4.0f));
			TangentV[p].XYZW[1] = (4.0f*temp*temp*temp*Q[4].XYZW[1] + Q[3].XYZW[1] * (12.0f*temp*temp - 16.0f*temp*temp*temp) + Q[2].XYZW[1] * (12.0f*temp - 36.0f*temp*temp + 24.0f*temp*temp*temp) + Q[1].XYZW[1] * (4.0f - 24.0f * temp + 36.0f * temp*temp - 16.0f * temp*temp*temp) + Q[0].XYZW[1] * (4.0f * temp*temp*temp - 12.0f * temp*temp + 12.0f * temp - 4.0f));
			TangentV[p].XYZW[2] = (4.0f*temp*temp*temp*Q[4].XYZW[2] + Q[3].XYZW[2] * (12.0f*temp*temp - 16.0f*temp*temp*temp) + Q[2].XYZW[2] * (12.0f*temp - 36.0f*temp*temp + 24.0f*temp*temp*temp) + Q[1].XYZW[2] * (4.0f - 24.0f * temp + 36.0f * temp*temp - 16.0f * temp*temp*temp) + Q[0].XYZW[2] * (4.0f * temp*temp*temp - 12.0f * temp*temp + 12.0f * temp - 4.0f));
			TangentV[p].XYZW[3] = 1.0f;
			for (j = 1; j <= 4; j++)
				for (k = 0; k <= 4 - j; k++)
				{
					Q[k].XYZW[0] = (1.0f - temp)*(Q[k].XYZW[0]) + temp*(Q[k + 1].XYZW[0]);
					Q[k].XYZW[1] = (1.0f - temp)*(Q[k].XYZW[1]) + temp*(Q[k + 1].XYZW[1]);
					Q[k].XYZW[2]= (1.0f - temp)*(Q[k].XYZW[2]) + temp*(Q[k + 1].XYZW[2]);

				}
			t[p].XYZW[0] = Q[0].XYZW[0];
			t[p].XYZW[1] = Q[0].XYZW[1];
			t[p].XYZW[2] = Q[0].XYZW[2];
			t[p].XYZW[3] = 1.0f;
			t[p].RGBA[0] = 1.0f;
			t[p].RGBA[1] = 1.0f;
			t[p].RGBA[2] = 0.0f;
			t[p].RGBA[3] = 1.0f;
			//printf("%f \n", t[p].XYZW[0]);
			p++;

		}

	}

}

void drawTangent()
{
	for (int i = 0; i < 240; i++)
	{
		TangentV[i].XYZW[0] = TangentV[i].XYZW[0] + t[i].XYZW[0];
		TangentV[i].XYZW[1] = TangentV[i].XYZW[1] + t[i].XYZW[1];
		TangentV[i].XYZW[2] = TangentV[i].XYZW[2] + t[i].XYZW[2];
		TangentV[i].XYZW[3] = 1.0f;
		}
}

void drawNormal()
{
	for (int i = 0; i < 240; i++)
	{
		rad = { t[i].XYZW[0],t[i].XYZW[1],t[i].XYZW[2] };
		tang = { TangentV[i].XYZW[0],TangentV[i].XYZW[1],TangentV[i].XYZW[2] };
		norm = glm::cross(rad, tang);
		NormalV[i].XYZW[0] = norm[0] + t[i].XYZW[0];
		NormalV[i].XYZW[1] = norm[1] + t[i].XYZW[1];
		NormalV[i].XYZW[2] = norm[2] + t[i].XYZW[2];
		NormalV[i].XYZW[3] = 1.0f;
		
	}
}


void drawBinormal()
{
	for (int i = 0; i < 240; i++)
	{
		norm = { NormalV[i].XYZW[0]-t[i].XYZW[0],NormalV[i].XYZW[1] - t[i].XYZW[1],NormalV[i].XYZW[2] - t[i].XYZW[2] };
		tang = { TangentV[i].XYZW[0] - t[i].XYZW[0],TangentV[i].XYZW[1] - t[i].XYZW[1],TangentV[i].XYZW[2] - t[i].XYZW[2] };
		bino = glm::cross(norm, tang);
		BinormalV[i].XYZW[0] = bino[0] + t[i].XYZW[0];
		BinormalV[i].XYZW[1] = bino[1] + t[i].XYZW[1];
		BinormalV[i].XYZW[2] = bino[2] + t[i].XYZW[2];
		BinormalV[i].XYZW[3] = 1.0f;
	}
}

void drawTNB()
{
	Tangent[0].XYZW[0] = t[point].XYZW[0];
	Tangent[0].XYZW[1] = t[point].XYZW[1];
	Tangent[0].XYZW[2] = t[point].XYZW[2];
	Tangent[0].XYZW[3] = t[point].XYZW[3];
	Tangent[0].RGBA[0] = 1.0f;
	Tangent[0].RGBA[1] = 0.0f;
	Tangent[0].RGBA[2] = 0.0f;
	Tangent[0].RGBA[3] = 1.0f;
	Tangent[1].XYZW[0] = TangentV[point].XYZW[0];
	Tangent[1].XYZW[1] = TangentV[point].XYZW[1];
	Tangent[1].XYZW[2] = TangentV[point].XYZW[2];
	Tangent[1].XYZW[3] = TangentV[point].XYZW[3];
	Tangent[1].RGBA[0] = 1.0f;
	Tangent[1].RGBA[1] = 0.0f;
	Tangent[1].RGBA[2] = 0.0f;
	Tangent[1].RGBA[3] = 1.0f;

	Normal[0].XYZW[0] = t[point].XYZW[0];
	Normal[0].XYZW[1] = t[point].XYZW[1];
	Normal[0].XYZW[2] = t[point].XYZW[2];
	Normal[0].XYZW[3] = t[point].XYZW[3];
	Normal[0].RGBA[0] = 0.0f;
	Normal[0].RGBA[1] = 1.0f;
	Normal[0].RGBA[2] = 0.0f;
	Normal[0].RGBA[3] = 1.0f;
	Normal[1].XYZW[0] = NormalV[point].XYZW[0];
	Normal[1].XYZW[1] = NormalV[point].XYZW[1];
	Normal[1].XYZW[2] = NormalV[point].XYZW[2];
	Normal[1].XYZW[3] = NormalV[point].XYZW[3];
	Normal[1].RGBA[0] = 0.0f;
	Normal[1].RGBA[1] = 1.0f;
	Normal[1].RGBA[2] = 0.0f;
	Normal[1].RGBA[3] = 1.0f;
	
	Binormal[0].XYZW[0] = t[point].XYZW[0];
	Binormal[0].XYZW[1] = t[point].XYZW[1];
	Binormal[0].XYZW[2] = t[point].XYZW[2];
	Binormal[0].XYZW[3] = t[point].XYZW[3];
	Binormal[0].RGBA[0] = 0.0f;
	Binormal[0].RGBA[1] = 0.0f;
	Binormal[0].RGBA[2] = 1.0f;
	Binormal[0].RGBA[3] = 1.0f;
	Binormal[1].XYZW[0] = BinormalV[point].XYZW[0];
	Binormal[1].XYZW[1] = BinormalV[point].XYZW[1];
	Binormal[1].XYZW[2] = BinormalV[point].XYZW[2];
	Binormal[1].XYZW[3] = BinormalV[point].XYZW[3];
	Binormal[1].RGBA[0] = 0.0f;
	Binormal[1].RGBA[1] = 0.0f;
	Binormal[1].RGBA[2] = 1.0f;
	Binormal[1].RGBA[3] = 1.0f;
	point = (point + 1) % 240;
}

void newVertex(float b[][4]) {

	int i, j;
	switch (m) {
	case 1: for (i = 0; i < k; i++)
		for (j = 0; j < 4; j++) {
			Vertices1[i].XYZW[j] = b[i][j];
			Vertices1[i].RGBA[j] = 1.0f;
		}
			break;
	case 2: for (i = 0; i < k; i++)
		for (j = 0; j < 4; j++) {
			Vertices2[i].XYZW[j] = b[i][j];
			Vertices2[i].RGBA[j] = 1.0f;
		}
			break;
	case 3: for (i = 0; i < k; i++)
		for (j = 0; j < 4; j++) {
			Vertices3[i].XYZW[j] = b[i][j];
			Vertices3[i].RGBA[j] = 1.0f;
		}
			break;
	case 4: for (i = 0; i < k; i++)
		for (j = 0; j < 4; j++) {
			Vertices4[i].XYZW[j] = b[i][j];
			Vertices4[i].RGBA[j] = 1.0f;
		}
			break;
	}
}
void createObjects(void)
{
	float a[128][4], prev[128][4];
	int n, i, j;
	if (m > 0) {
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 4; j++) {
				prev[i][j] = Vertices[i].XYZW[j];
			}


		// ATTN: DERIVE YOUR NEW OBJECTS HERE:
		// each has one vertices {pos;color} and one indices array (no picking needed here)
		for (i = 1; i <= m; i++) {
			n = 8 * pow(2, i - 1);
			for (j = 0; j < n; j++) {
				a[2 * j][0] = (prev[(n + j - 2) % (n)][0] + 10 * (prev[(n + j - 1) % (n)][0]) + 5 * (prev[(n + j) % (n)][0])) / 16;
				a[2 * j][1] = (prev[(n + j - 2) % (n)][1] + 10 * (prev[(n + j - 1) % (n)][1]) + 5 * (prev[(n + j) % (n)][1])) / 16;
				a[2 * j][2] = 0.0f;
				a[2 * j][3] = 1.0f;
				a[(2 * j + 1) % (2 * n)][0] = (5 * prev[(n + j - 1) % (n)][0] + 10 * prev[(n + j) % (n)][0] + prev[(n + j + 1) % (n)][0]) / 16;
				a[(2 * j + 1) % (2 * n)][1] = (5 * prev[(n + j - 1) % (n)][1] + 10 * prev[(n + j) % (n)][1] + prev[(n + j + 1) % (n)][1]) / 16;
				a[(2 * j + 1) % (2 * n)][2] = 0.0f;
				a[(2 * j + 1) % (2 * n)][3] = 1.0f;
			}
			for (j = 0; j < (2 * n); j++) {
				prev[j][0] = a[j][0];
				prev[j][1] = a[j][1];
			}
		}
		newVertex(a);
	}
}

void drawScene(void)
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);
	{
		if (cut == 1) {
			gProjectionMatrix = glm::ortho(-7.5f, 7.5f, -3.0f, 3.0f, 0.0f, 100.0f);
			glViewport(0, window_height / 2, window_width, window_height / 2);
		}
		else
		{
			gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f);
			glViewport(0, 0, window_width, window_height);
		}
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		///////////////////////////////////////
		//	glUniform3f(uOuter0, 1.0f, 3.0f, 5.0f);
		//	glUniform3f(uOuter1, 3.0f, 5.0f, 50.0f);
		///////////////////////////////////////
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		glEnable(GL_PROGRAM_POINT_SIZE);

		glBindVertexArray(VertexArrayId[1]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Close), Close);				// update buffer data
																				//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		glDrawElements(GL_LINE_STRIP, NumVert[1], GL_UNSIGNED_SHORT, (void*)0);
		// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
		//glBindVertexArray(VertexArrayId[<x>]); etc etc
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayId[0]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);				// update buffer data
																						//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
		//glBindVertexArray(VertexArrayId[<x>]); etc etc
		glBindVertexArray(0);
		////////////////////////////////////////////////////////////////////////////
		/*		glPatchParameteri(GL_PATCH_VERTICES, 5);
		glBegin(GL_PATCHES);
		glVertex3f(t[0].XYZW[0], t[0].XYZW[1], t[0].XYZW[2]);
		glVertex3f(t[1].XYZW[0], t[1].XYZW[1], t[1].XYZW[2]);
		glVertex3f(t[2].XYZW[0], t[2].XYZW[1], t[2].XYZW[2]);
		glVertex3f(t[3].XYZW[0], t[3].XYZW[1], t[3].XYZW[2]);
		glVertex3f(t[4].XYZW[0], t[4].XYZW[1], t[4].XYZW[2]);
		glEnd();*/
		//////////////////////////////////////////////////////////////////////////////
		if (m == 1) {
			glBindVertexArray(VertexArrayId[2]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[2]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices1), Vertices1);				// update buffer data
																							//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_LINE_STRIP, NumVert[2], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);
		}
		else if (m == 2) {
			glBindVertexArray(VertexArrayId[3]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);				// update buffer data
																							//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_LINE_STRIP, NumVert[3], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);
		}
		else if (m == 3) {
			glBindVertexArray(VertexArrayId[4]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices3), Vertices3);				// update buffer data
																							//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_LINE_STRIP, NumVert[4], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);
		}
		else if (m == 4) {
			glBindVertexArray(VertexArrayId[5]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices4), Vertices4);				// update buffer data
																							//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_LINE_STRIP, NumVert[5], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);
		}
		if (bezier == 1) {
			glBindVertexArray(VertexArrayId[6]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(c), c);				// update buffer data
																			//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_POINTS, NumVert[6], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);

			glBindVertexArray(VertexArrayId[7]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(t), t);				// update buffer data
																			//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_LINE_STRIP, NumVert[7], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);
		}
		glBindVertexArray(VertexArrayId[8]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[8]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Tangent), Tangent);
		glDrawElements(GL_LINES, NumVert[8], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayId[9]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[9]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Normal), Normal);
		glDrawElements(GL_LINES, NumVert[9], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);

		glBindVertexArray(VertexArrayId[10]);	// draw Vertices
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[10]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Binormal), Binormal);
		glDrawElements(GL_LINES, NumVert[10], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
		
	}
	glUseProgram(0);
	// Draw GUI
	if (cut == 1) {
		glUseProgram(programID);
		{
			glViewport(0, 0, window_width, window_height / 2);
			glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
			glm::mat4 MVP = gProjectionMatrix * gViewMatrix2 * ModelMatrix;

			///////////////////////////////////////
			//	glUniform3f(uOuter0, 1.0f, 3.0f, 5.0f);
			//	glUniform3f(uOuter1, 3.0f, 5.0f, 50.0f);
			///////////////////////////////////////
			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
			glm::vec3 lightPos = glm::vec3(4, 4, 4);
			glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

			glEnable(GL_PROGRAM_POINT_SIZE);

			glBindVertexArray(VertexArrayId[1]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Close), Close);				// update buffer data
																					//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_LINE_STRIP, NumVert[1], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);

			glBindVertexArray(VertexArrayId[0]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);				// update buffer data
																							//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
			// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
			//glBindVertexArray(VertexArrayId[<x>]); etc etc
			glBindVertexArray(0);
			////////////////////////////////////////////////////////////////////////////
			/*		glPatchParameteri(GL_PATCH_VERTICES, 5);
			glBegin(GL_PATCHES);
			glVertex3f(t[0].XYZW[0], t[0].XYZW[1], t[0].XYZW[2]);
			glVertex3f(t[1].XYZW[0], t[1].XYZW[1], t[1].XYZW[2]);
			glVertex3f(t[2].XYZW[0], t[2].XYZW[1], t[2].XYZW[2]);
			glVertex3f(t[3].XYZW[0], t[3].XYZW[1], t[3].XYZW[2]);
			glVertex3f(t[4].XYZW[0], t[4].XYZW[1], t[4].XYZW[2]);
			glEnd();*/
			//////////////////////////////////////////////////////////////////////////////
			if (m == 1) {
				glBindVertexArray(VertexArrayId[2]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[2]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices1), Vertices1);				// update buffer data
																								//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
				glDrawElements(GL_LINE_STRIP, NumVert[2], GL_UNSIGNED_SHORT, (void*)0);
				// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
				//glBindVertexArray(VertexArrayId[<x>]); etc etc
				glBindVertexArray(0);
			}
			else if (m == 2) {
				glBindVertexArray(VertexArrayId[3]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);				// update buffer data
																								//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
				glDrawElements(GL_LINE_STRIP, NumVert[3], GL_UNSIGNED_SHORT, (void*)0);
				// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
				//glBindVertexArray(VertexArrayId[<x>]); etc etc
				glBindVertexArray(0);
			}
			else if (m == 3) {
				glBindVertexArray(VertexArrayId[4]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices3), Vertices3);				// update buffer data
																								//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
				glDrawElements(GL_LINE_STRIP, NumVert[4], GL_UNSIGNED_SHORT, (void*)0);
				// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
				//glBindVertexArray(VertexArrayId[<x>]); etc etc
				glBindVertexArray(0);
			}
			else if (m == 4) {
				glBindVertexArray(VertexArrayId[5]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices4), Vertices4);				// update buffer data
																								//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
				glDrawElements(GL_LINE_STRIP, NumVert[5], GL_UNSIGNED_SHORT, (void*)0);
				// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
				//glBindVertexArray(VertexArrayId[<x>]); etc etc
				glBindVertexArray(0);
			}
			if (bezier == 1) {
				glBindVertexArray(VertexArrayId[6]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(c), c);				// update buffer data
																				//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
				glDrawElements(GL_POINTS, NumVert[6], GL_UNSIGNED_SHORT, (void*)0);
				// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
				//glBindVertexArray(VertexArrayId[<x>]); etc etc
				glBindVertexArray(0);

				glBindVertexArray(VertexArrayId[7]);	// draw Vertices
				glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(t), t);				// update buffer data
																				//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
				glDrawElements(GL_LINE_STRIP, NumVert[7], GL_UNSIGNED_SHORT, (void*)0);
				// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
				//glBindVertexArray(VertexArrayId[<x>]); etc etc
				glBindVertexArray(0);
			}
			glBindVertexArray(VertexArrayId[8]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[8]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Tangent), Tangent);
			glDrawElements(GL_LINES, NumVert[8], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);

			glBindVertexArray(VertexArrayId[9]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[9]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Normal), Normal);
			glDrawElements(GL_LINES, NumVert[9], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);

			glBindVertexArray(VertexArrayId[10]);	// draw Vertices
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[10]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Binormal), Binormal);
			glDrawElements(GL_LINES, NumVert[10], GL_UNSIGNED_SHORT, (void*)0);
			glBindVertexArray(0);
		}
	}
	TwDraw(); 

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void pickVertex(void)
{
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1fv(pickingColorArrayID, NumVert[0], pickingColor);	// here we pass in the picking marker array

																		// Draw the ponts
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(VertexArrayId[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);	// update buffer data
		glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

																					 // Convert the color back to an integer ID
	gPickedIndex = int(data[0]);
	Vertices[gPickedIndex].RGBA[0] = 0.0f;
	Vertices[gPickedIndex].RGBA[1] = 1.0f;
	Vertices[gPickedIndex].RGBA[2] = 0.0f;
	Vertices[gPickedIndex].RGBA[3] = 1.0f;
	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

// fill this function in!
void moveVertex(void)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
	// retrieve your cursor position
	// get your world coordinates
	// move points

	if (gPickedIndex == 255) { // Full white, must be the background !
		gMessage = "background";
	}
	else {
		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str();
	}
	Vertices[gPickedIndex].RGBA[0] = 1.0f;
	Vertices[gPickedIndex].RGBA[1] = 1.0f;
	Vertices[gPickedIndex].RGBA[2] = 1.0f;
	Vertices[gPickedIndex].RGBA[3] = 1.0f;
	glm::vec3 position = glm::unProject(glm::vec3(xpos, 768 - ypos, 0.0), ModelMatrix, gProjectionMatrix, vp);
	if (shift == 1) {
		Vertices[gPickedIndex].XYZW[1] = position[1];
		Vertices[gPickedIndex].XYZW[2] = -position[0] - Vertices[gPickedIndex].XYZW[0];
		Close[gPickedIndex].XYZW[1] = position[1];
		Close[gPickedIndex].XYZW[2] = -position[0] - Vertices[gPickedIndex].XYZW[0];
	}
	else if (shift == 0) {
		Vertices[gPickedIndex].XYZW[1] = position[1];
		Vertices[gPickedIndex].XYZW[0] = -position[0];
		Close[gPickedIndex].XYZW[1] = position[1];
		Close[gPickedIndex].XYZW[0] = -position[0];
	}

}

int initWindow(void)
{
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Maity,Braja Gopal(81490468)", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

	// Set up inputs
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetMouseButtonCallback(window, mouseCallback);

	return 0;
}

void initOpenGL(void)
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

																			// Camera matrix
	gViewMatrix = glm::lookAt(
		glm::vec3(0, 0, -5), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	gViewMatrix2 = glm::lookAt(
		glm::vec3(5, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	// Create and compile our GLSL program from the shaders


	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorArrayID = glGetUniformLocation(pickingProgramID, "PickingColorArray");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	createVAOs(Vertices, Indices, sizeof(Vertices), sizeof(Indices), 0);
	createVAOs(Close, lineIndices, sizeof(Close), sizeof(lineIndices), 1);
	createVAOs(Vertices1, Indices1, sizeof(Vertices1), sizeof(Indices1), 2);
	createVAOs(Vertices2, Indices2, sizeof(Vertices2), sizeof(Indices2), 3);
	createVAOs(Vertices3, Indices3, sizeof(Vertices3), sizeof(Indices3), 4);
	createVAOs(Vertices4, Indices4, sizeof(Vertices4), sizeof(Indices4), 5);
	createVAOs(c, IndicesC, sizeof(c), sizeof(IndicesC), 6);
	createVAOs(t, IndicesT, sizeof(t), sizeof(IndicesT), 7);
	createVAOs(Tangent, InTangent, sizeof(Tangent), sizeof(InTangent), 8);
	createVAOs(Normal, InNormal, sizeof(Normal), sizeof(InNormal), 9);
	createVAOs(Binormal, InBinormal, sizeof(Binormal), sizeof(InBinormal), 10);

	// ATTN: create VAOs for each of the newly created objects here:
	// createVAOs(<fill this appropriately>);

}

void createVAOs(Vertex Vertices[], unsigned short Indices[], size_t BufferSize, size_t IdxBufferSize, int ObjectId) {

	NumVert[ObjectId] = IdxBufferSize / (sizeof GLubyte);

	GLenum ErrorCheckValue = glGetError();
	size_t VertexSize = sizeof(Vertices[0]);
	size_t RgbOffset = sizeof(Vertices[0].XYZW);

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);
	glBindVertexArray(VertexArrayId[ObjectId]);

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	glGenBuffers(1, &IndexBufferId[ObjectId]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IdxBufferSize, Indices, GL_STATIC_DRAW);

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color

									// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
		);
	}
}

void cleanup(void)
{
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickVertex();
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		Vertices[gPickedIndex].RGBA[0] = 0.0f;
		Vertices[gPickedIndex].RGBA[1] = 1.0f;
		Vertices[gPickedIndex].RGBA[2] = 0.0f;
		Vertices[gPickedIndex].RGBA[3] = 1.0f;
	}
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		m = (m + 1) % 5;
		k = 8 * pow(2, m);
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		if (bezier == 0)
			bezier = 1;
		else
			bezier = 0;

	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		cut = (cut + 1) % 2;
	}
	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		shift = (shift + 1) % 2;
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
		key5 = (key5 + 1) % 2;
	}
}

int main(void)
{
	// initialize window
	createIndices();
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	// initialize OpenGL pipeline
	initOpenGL();

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1sec ago
											 // printf and reset
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// DRAGGING: move current (picked) vertex with cursor
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			moveVertex();


		// DRAWING SCENE
		createBezierCurve();
		if (key5 == 1) {
			drawTangent();
			drawNormal();
			drawBinormal();
			drawTNB();
		}
		//getch();
		createObjects();	// re-evaluate curves in case vertices have been moved
							//getch();
		drawScene();

		glfwSetKeyCallback(window, key_callback);
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}