#pragma once

// C++ includes
#include <memory>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "includes/gCamera.h"

#include "includes/ProgramObject.h"
#include "includes/BufferObject.h"
#include "includes/VertexArrayObject.h"
#include "includes/TextureObject.h"

// mesh
#include "includes/ObjParser_OGL3.h"

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);

protected:
	// shaderekhez szükséges változók
	ProgramObject		m_program;			// mesh shader
	ProgramObject		m_program2;			// mesh shader
	ProgramObject		m_programSkybox;	// skybox shader

	VertexArrayObject	m_CubeVao;			// VAO
	IndexBuffer			m_CubeIndices;		// index buffer
	ArrayBuffer			m_CubeVertexBuffer;	// VBO
	VertexArrayObject	m_SkyboxVao;
	IndexBuffer			m_SkyboxIndices;	
	ArrayBuffer			m_SkyboxPos;		

	gCamera				m_camera;

	Texture2D			m_woodTexture;
	Texture2D			m_suzanneTexture;
	Texture2D			m_erdoTexture;
	Texture2D			m_grassTexture;
	Texture2D			m_barkTexture;
	Texture2D			m_leavesTexture;

	// Ground

	static const int N = 19;
	static const int M = 19;

	glm::vec3 GetgroundPos(float u, float v);
	glm::vec2 GetTex(float u, float v);
	glm::vec3 GetgroundNorm(float u, float v);

	VertexArrayObject	m_groundVAO;
	ArrayBuffer			m_groundPosVBO;
	IndexBuffer			m_groundIndices;

	//erdo

	VertexArrayObject	m_ErdoVao;				// VAO
	IndexBuffer			m_ErdoIndices;			// index buffer
	ArrayBuffer			m_ErdoVertexBuffer;		// VBO

	//lomb

	glm::vec3 GetPos(float u, float v);
	glm::vec3 GetNorm(float u, float v);

	VertexArrayObject	m_SphereVao;
	ArrayBuffer			m_SphereVertexBuffer;
	IndexBuffer			m_SphereIndices;

	// fa

	int r;
	float m_trees[N][M];
	float growing = 0.f;
	float growingNum = 10.f;
	float sec2 = 0.f;
	float treeSize = 1.f;
	int treeCuted = 0;

	glm::vec3 light_pos;

	//feny

	float yearNum = 10.f;

	// nyers OGL azonosítók
	GLuint				m_skyboxTexture;

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 n;
		glm::vec2 t;
	};

	// mesh adatok
	Mesh *m_mesh;
	Mesh *m_cmesh;

	// a jobb olvashatóság kedvéért
	void InitShaders();
	void InitSphere();
	void InitSkyBox();
	void InitGround();

	void InitErdo();
	void ForestRender();
	void RenderGround();
	void RenderCylinder(glm::mat4 word);
	void RenderSphere(glm::mat4 word);
	void RenderTree();

	void random();
	void checkTrees();
	void checkTreesNum();
	void cutAllTrees();
};

