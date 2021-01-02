#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>
#include <imgui/imgui.h>
#include "includes/GLUtils.hpp"
#include <time.h>
#include <random>

CMyApp::CMyApp(void)
{
	m_camera.SetView(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_mesh = nullptr;
}

CMyApp::~CMyApp(void)
{
}

glm::vec3 CMyApp::GetPos(float u, float v)
{
	u *= float(2 * M_PI);
	v *= float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	float r = 1;

	return glm::vec3(r * cu * sv, r * cv, r * su * sv);
}

glm::vec3 CMyApp::GetNorm(float u, float v)
{
	// Képlettel
	u *= float(2 * M_PI);
	v *= float(M_PI);
	float cu = cosf(u), su = sinf(u), cv = cosf(v), sv = sinf(v);
	return glm::vec3(cu * sv, cv, su * sv);
}

void CMyApp::InitSphere()
{
	// NxM darab négyszöggel közelítjük a parametrikus felületünket => (N+1)x(M+1) pontban kell kiértékelni
	Vertex vert[(N + 1) * (M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vert[i + j * (N + 1)].p = GetPos(u, v);
			vert[i + j * (N + 1)].n = GetNorm(u, v);
			vert[i + j * (N + 1)].t = GetTex(u, v);
		}

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
	GLushort indices[3 * 2 * (N) * (M)];
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{

			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1) * (N + 1);
		}

	m_SphereVertexBuffer.BufferData(vert);

	// és a primitíveket alkotó csúcspontok indexei (az elõzõ tömbökbõl) - triangle list-el való kirajzolásra felkészülve
	m_SphereIndices.BufferData(indices);

	// geometria VAO-ban való regisztrálása
	m_SphereVao.Init(
		{
			// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_CubeVertexBuffer GPU pufferben vannak
			{ CreateAttribute<		0,						// attribútum: 0
									glm::vec3,				// CPU oldali adattípus amit a 0-ás attribútum meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
									0,						// offset: az attribútum tároló elejétõl vett offset-je, byte-ban
									sizeof(Vertex)			// stride: a következõ csúcspont ezen attribútuma hány byte-ra van az aktuálistól
								>, m_SphereVertexBuffer },
			{ CreateAttribute<1, glm::vec3, (sizeof(glm::vec3)), sizeof(Vertex)>, m_SphereVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)), sizeof(Vertex)>, m_SphereVertexBuffer },
		},
		m_SphereIndices
	);
}

glm::vec3 CMyApp::GetgroundNorm(float u, float v)
{
	glm::vec3 du = GetgroundPos(u + 0.01, v) - GetgroundPos(u - 0.01, v);
	glm::vec3 dv = GetgroundPos(u, v + 0.01) - GetgroundPos(u, v - 0.01);
	return glm::normalize(glm::cross(du, dv));
}

glm::vec2 CMyApp::GetTex(float u, float v)
{
	return glm::vec2(1 - u, 1 - v);
}

glm::vec3 CMyApp::GetgroundPos(float u, float v)
{
	//return glm::vec3((-u * 20.f) + 10.f, sinf(u * v * 20), (v * 20.f) - 10.f);
	return glm::vec3((-u * 20.f) + 10.f, 0, (v * 20.f) - 10.f);
}

void CMyApp::InitGround()
{
	Vertex vert[(N + 1) * (M + 1)];
	for (int i = 0; i <= N; ++i)
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vert[i + j * (N + 1)].p = GetgroundPos(u, v);
			vert[i + j * (N + 1)].n = GetgroundNorm(u, v);
			vert[i + j * (N + 1)].t = GetTex(u, v);
		}

	// indexpuffer adatai: NxM négyszög = 2xNxM háromszög = háromszöglista esetén 3x2xNxM index
	GLushort indices[3 * 2 * (N) * (M)];
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
		{
			indices[6 * i + j * 3 * 2 * (N)+0] = (i)+(j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+1] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+2] = (i)+(j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+3] = (i + 1) + (j) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+4] = (i + 1) + (j + 1) * (N + 1);
			indices[6 * i + j * 3 * 2 * (N)+5] = (i)+(j + 1) * (N + 1);
		}

	m_groundPosVBO.BufferData(vert);

	// és a primitíveket alkotó csúcspontok indexei (az elõzõ tömbökbõl) - triangle list-el való kirajzolásra felkészülve
	m_groundIndices.BufferData(indices);

	// geometria VAO-ban való regisztrálása
	m_groundVAO.Init(
		{
			// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_CubeVertexBuffer GPU pufferben vannak
			{ CreateAttribute<		0,						// attribútum: 0
									glm::vec3,				// CPU oldali adattípus amit a 0-ás attribútum meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
									0,						// offset: az attribútum tároló elejétõl vett offset-je, byte-ban
									sizeof(Vertex)			// stride: a következõ csúcspont ezen attribútuma hány byte-ra van az aktuálistól
								>, m_groundPosVBO },
			{ CreateAttribute<		1,
									glm::vec3,
									sizeof(glm::vec3),
									sizeof(Vertex)
								>, m_groundPosVBO },
			{ CreateAttribute<		2,
									glm::vec2,
									sizeof(glm::vec3) * 2,
									sizeof(Vertex)
								>, m_groundPosVBO },
		},
		m_groundIndices
	);
}

void CMyApp::InitErdo()
{
	//struct Vertex{ glm::vec3 position; glm::vec3 normals; glm::vec2 texture; };
	std::vector<Vertex>vertices;

	//front									 
	vertices.push_back({ glm::vec3(-0.5, -0.5, +0.5), glm::vec3(0, 0, -1), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(+0.5, -0.5, +0.5), glm::vec3(0, 0, -1), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, +0.5), glm::vec3(0, 0, -1), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, +0.5), glm::vec3(0, 0, -1), glm::vec2(1, 1) });
	//back
	vertices.push_back({ glm::vec3(+0.5, -0.5, -0.5), glm::vec3(0, 0, 1), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, 1), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, -0.5), glm::vec3(0, 0, 1), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, -0.5), glm::vec3(0, 0, 1), glm::vec2(1, 1) });
	//right									 
	vertices.push_back({ glm::vec3(+0.5, -0.5, +0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(+0.5, -0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, +0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(+0.5, +0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 1) });
	//left									 
	vertices.push_back({ glm::vec3(-0.5, -0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(-0.5, -0.5, +0.5), glm::vec3(1, 0, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 1) });
	vertices.push_back({ glm::vec3(-0.5, +0.5, +0.5), glm::vec3(1, 0, 0), glm::vec2(1, 1) });

	std::vector<int> indices(4 * 6);
	int index = 0;
	for (int i = 0; i < 4 * 4; i += 4)
	{
		indices[index + 0] = i + 0;
		indices[index + 1] = i + 2;
		indices[index + 2] = i + 1;

		indices[index + 3] = i + 1;
		indices[index + 4] = i + 2;
		indices[index + 5] = i + 3;
		index += 6;
	}

	m_ErdoVertexBuffer.BufferData(vertices);

	// és a primitíveket alkotó csúcspontok indexei (az elõzõ tömbökbõl) - triangle list-el való kirajzolásra felkészülve
	m_ErdoIndices.BufferData(indices);

	// geometria VAO-ban való regisztrálása
	m_ErdoVao.Init(
		{
			// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_ErdoVertexBuffer GPU pufferben vannak
			{ CreateAttribute<		0,						// attribútum: 0
									glm::vec3,				// CPU oldali adattípus amit a 0-ás attribútum meghatározására használtunk <- az eljárás a glm::vec3-ból kikövetkezteti, hogy 3 darab float-ból áll a 0-ás attribútum
									0,						// offset: az attribútum tároló elejétõl vett offset-je, byte-ban
									sizeof(Vertex)			// stride: a következõ csúcspont ezen attribútuma hány byte-ra van az aktuálistól
								>, m_ErdoVertexBuffer },
			{ CreateAttribute<1, glm::vec3, (sizeof(glm::vec3)), sizeof(Vertex)>, m_ErdoVertexBuffer },
			{ CreateAttribute<2, glm::vec2, (2 * sizeof(glm::vec3)), sizeof(Vertex)>, m_ErdoVertexBuffer },
		},
		m_ErdoIndices
	);
}

void CMyApp::InitSkyBox()
{
	m_SkyboxPos.BufferData(
		std::vector<glm::vec3>{
		// hátsó lap
		glm::vec3(-1, -1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, 1, -1),
		glm::vec3(-1, 1, -1),
		// elülsõ lap
		glm::vec3(-1, -1, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(-1, 1, 1),
	}
	);

	// és a primitíveket alkotó csúcspontok indexei (az elõzõ tömbökbõl) - triangle list-el való kirajzolásra felkészülve
	m_SkyboxIndices.BufferData(
		std::vector<int>{
			// hátsó lap
			0, 1, 2,
			2, 3, 0,
			// elülsõ lap
			4, 6, 5,
			6, 4, 7,
			// bal
			0, 3, 4,
			4, 3, 7,
			// jobb
			1, 5, 2,
			5, 6, 2,
			// alsó
			1, 0, 4,
			1, 4, 5,
			// felsõ
			3, 2, 6,
			3, 6, 7,
	}
	);

	// geometria VAO-ban való regisztrálása
	m_SkyboxVao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_SkyboxPos },
		}, m_SkyboxIndices
	);

	// skybox texture
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &m_skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	TextureFromFileAttach("assets/xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	TextureFromFileAttach("assets/xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	TextureFromFileAttach("assets/ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	TextureFromFileAttach("assets/yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	TextureFromFileAttach("assets/zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	TextureFromFileAttach("assets/zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void CMyApp::InitShaders()
{
	m_program.Init(
		{
			{ GL_VERTEX_SHADER, "myVert.vert" },
			{ GL_FRAGMENT_SHADER, "myFrag.frag" }
		},
		{
			{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
			{ 1, "vs_in_norm" },			// VAO 1-es csatorna menjen a vs_in_norm-ba
			{ 2, "vs_in_tex" },
		}
	);

	m_program2.Init(
		{
			{ GL_VERTEX_SHADER, "myVert2.vert" },
			{ GL_FRAGMENT_SHADER, "myFrag2.frag" }
		},
		{
			{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
			{ 1, "vs_in_norm" },			// VAO 1-es csatorna menjen a vs_in_norm-ba
			{ 2, "vs_in_tex" },
		}
		);

	// shader program rövid létrehozása, egyetlen függvényhívással a fenti három:
	m_programSkybox.Init(
		{
			{ GL_VERTEX_SHADER, "skybox.vert" },
			{ GL_FRAGMENT_SHADER, "skybox.frag" }
		},
		{
			{ 0, "vs_in_pos" },				// VAO 0-as csatorna menjen a vs_in_pos-ba
		}
	);
}

bool CMyApp::Init()
{
	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	InitShaders();
	InitSkyBox();
	InitErdo();
	InitGround();
	InitSphere();

	random();

	// egyéb textúrák betöltése
	m_woodTexture.FromFile("assets/wood.jpg");
	m_suzanneTexture.FromFile("assets/marron.jpg");
	m_erdoTexture.FromFile("assets/forest.jpg");
	m_grassTexture.FromFile("assets/grass.jpg");
	m_barkTexture.FromFile("assets/bark.jpg");
	m_leavesTexture.FromFile("assets/leaves.jpg");

	// mesh betoltese
	m_mesh = ObjParser::parse("assets/Suzanne.obj");
	m_mesh->initBuffers();

	// mesh cylinder betoltese
	m_cmesh = ObjParser::parse("assets/henger.obj");
	m_cmesh->initBuffers();
	
	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &m_skyboxTexture);

	delete m_mesh;
	delete m_cmesh;
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	//std::cout << last_time <<" "<<sdl<<" "<<sdl2<< std::endl;
	if (sec2 >= 1.f)
	{
		sec2 = 0.0f;
		checkTrees();
	}

	sec2 += delta_time * 0.5;
	growing += delta_time / growingNum;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::cutAllTrees() {
	for (int i = 2; i < N - 2; ++i)
	{
		for (int j = 2; j < M - 2; ++j)
		{
			//std::cout <<i<<" "<<j <<" "<< m_trees[i][j] << std::endl;
			if (m_trees[i][j] != 0.0f)
			{
				treeCuted += 1;
				m_trees[i][j] = 0.0f;
			}
		}
	}
}

void CMyApp::checkTreesNum() {
	int num = 0;
	//int num2 = 0;
	for (int i = 2; i < N - 2; ++i)
	{
		for (int j = 2; j < M - 2; ++j)
		{
			//std::cout <<i<<" "<<j <<" "<< m_trees[i][j] << std::endl;
			if (m_trees[i][j] == 0.0f)
			{
				num += 1;
			}
			else
			{
				treeCuted += 1;
			}
		}
	}
	//std::cout << num <<"!"<<std::endl;
	//std::cout << num2 <<"!!"<<std::endl;
}

void CMyApp::checkTrees()
{
	srand(time(0));
	//checkTreesNum();
	for (int i = 2; i < N - 2; ++i)
	{
		for (int j = 2; j < M - 2; ++j)
		{
			if (m_trees[i][j] == 0.0f)
			{
				float b = rand() % 20 + 1;
				//float b = rand() % 2 + 1;
				if (b == 1)
				{
					m_trees[i][j] = 0.05f;
				}
			}
		}
	}
}

void CMyApp::random() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rnd(0, 1);

	srand(time(0));
	r = rand() % 51 + 50;
	int n = 0;

	for (int i = 2; i < N - 2; ++i)
	{
		for (int j = 2; j < M - 2; ++j)
		{
			float b = rand() % 5;
			if (b == 0 && n <=r)
			{
				m_trees[i][j] = rnd(gen);
				//std::cout << rnd(gen) <<"!"<< std::endl;
				n += 1;
			}
			else
			{
				m_trees[i][j] = 0;
			}
		}
	}
}

void CMyApp::ForestRender()
{
	// erdö háttér

	glm::mat4 viewProj = m_camera.GetViewProj();
	m_ErdoVao.Bind();
	m_program.SetTexture("texImage", 0, m_erdoTexture);
	glm::mat4 erdoWorld =glm::translate(glm::vec3(0,8.f,0)) * glm::scale(glm::vec3(20)) ;
	m_program.SetUniform("MVP", viewProj * erdoWorld);
	m_program.SetUniform("world", erdoWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(erdoWorld)));
	glDrawElements(GL_TRIANGLES, 4 * 6, GL_UNSIGNED_INT, nullptr);

	m_ErdoVao.Unbind();
	m_program.Unuse();
}

void CMyApp::RenderGround()
{
	glm::mat4 groundWorld = glm::mat4(1.0f);
	glm::mat4 viewProj = m_camera.GetViewProj();

	m_program2.Use();
	m_program2.SetTexture("texImage", 0, m_grassTexture);
	m_program2.SetUniform("MVP", viewProj * groundWorld);
	m_program2.SetUniform("world", groundWorld);
	m_program2.SetUniform("worldIT", glm::inverse(glm::transpose(groundWorld)));
	m_program2.SetUniform("eye_pos", glm::vec3(m_camera.GetEye().x, m_camera.GetEye().y, m_camera.GetEye().z));
	m_program2.SetUniform("light_dir", light_pos);

	m_groundVAO.Bind();
	glDrawElements(GL_TRIANGLES,	// primitív típus
		3 * 2 * (N) * (M),			// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,			// indexek tipusa
		0);							// indexek eltolása

	m_groundVAO.Unbind();
	m_program2.Unuse();
}

void CMyApp::RenderCylinder(glm::mat4 word)
{
	glm::mat4 viewProj = m_camera.GetViewProj();

	m_program2.Use();
	m_program2.SetTexture("texImage", 0, m_barkTexture);
	m_program2.SetUniform("MVP", viewProj * word);
	m_program2.SetUniform("world", word);
	m_program2.SetUniform("worldIT", glm::inverse(glm::transpose(word)));

	m_cmesh->draw();
	m_program2.Unuse();
}

void CMyApp::RenderSphere(glm::mat4 word)
{
	glm::mat4 viewProj = m_camera.GetViewProj();

	m_program2.Use();
	m_program2.SetTexture("texImage", 0, m_leavesTexture);
	m_program2.SetUniform("MVP", viewProj * word);
	m_program2.SetUniform("world", word);
	m_program2.SetUniform("worldIT", glm::inverse(glm::transpose(word)));

	m_SphereVao.Bind();
	glDrawElements(GL_TRIANGLES,	// primitív típus
		3 * 2 * (N) * (M),			// hany csucspontot hasznalunk a kirajzolashoz
		GL_UNSIGNED_SHORT,			// indexek tipusa
		0);							// indexek eltolása

	m_SphereVao.Unbind();
	m_program2.Unuse();
}

void CMyApp::RenderTree() {
	for (int i = 2; i < N - 2; ++i)
	{
		for (int j = 2; j < M - 2; ++j)
		{
			if (m_trees[i][j] > 0.0f)
			{
				float u = i / (float)N;
				float v = j / (float)M;

				glm::mat4 world = glm::translate(GetgroundPos(u, v));
				glm::vec3 scaleValue = glm::vec3(m_trees[i][j]) * growing;
				//std::cout <<growing<< std::endl;
				//std::cout <<i<<" "<<j<<" "<< m_trees[i][j] //<< std::endl;
				//<< scaleValue[0]<<" "<< scaleValue[1]<<" "<< scaleValue[2] << std::endl;
				RenderCylinder(world * glm::scale(scaleValue) * glm::translate(glm::vec3(0, 2.f, 0)));
				RenderSphere(world * glm::scale(scaleValue) * glm::translate(glm::vec3(0, 4.f, 0)));

				if (growing >= treeSize)
				{
					//std::cout << "-" << std::endl;
					growing = 0.f;
					checkTreesNum();
					m_trees[i][j] = 0.0f;
				}
			}
		}
	}
	
}



void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 viewProj = m_camera.GetViewProj();

	//Suzanne
	glm::mat4 suzanneWorld = glm::mat4(1.0f);
	m_program.Use();
	m_program.SetTexture("texImage", 0, m_suzanneTexture);
	m_program.SetUniform("MVP", viewProj * suzanneWorld);
	m_program.SetUniform("world", suzanneWorld);
	m_program.SetUniform("worldIT", glm::inverse(glm::transpose(suzanneWorld)));
	//m_mesh->draw();

	// Erdo
	ForestRender();

	//Cylinder
	//RenderCylinder(glm::translate(glm::vec3(0, 2, 0)));

	//ground
	RenderGround();

	//sphere
	//RenderSphere(glm::translate(glm::vec3(0, 4.f, 0)));

	// tree
	RenderTree();


	// skybox
	// mentsük el az elõzõ Z-test eredményt, azaz azt a relációt, ami alapján update-eljük a pixelt.
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenlõt használjunk, mert mindent kitolunk a távoli vágósíkokra
	glDepthFunc(GL_LEQUAL);

	m_SkyboxVao.Bind();
	m_programSkybox.Use();
	m_programSkybox.SetUniform("MVP", viewProj * glm::translate( m_camera.GetEye()) );
	
	// cube map textúra beállítása 0-ás mintavételezõre és annak a shaderre beállítása
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glUniform1i(m_programSkybox.GetLocation("skyboxTexture"), 0);
	// az elõzõ három sor <=> m_programSkybox.SetCubeTexture("skyboxTexture", 0, m_skyboxTexture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	m_programSkybox.Unuse();

	// végül állítsuk vissza
	glDepthFunc(prevDepthFnc);


	//IMGUI
	ImGui::ShowTestWindow();

	if (ImGui::Begin("My Windows"))
	{
		ImGui::SliderFloat("Growing",&growingNum,5.f,20.f);
		ImGui::SliderFloat("Year",&yearNum,5.f,20.f);
		ImGui::SliderFloat("Tree size",&treeSize,0.5f,1.2f);

		ImGui::SliderFloat3("Light dict",&light_pos.x, -1.f, 1.f);

		std::string s= std::to_string(treeCuted);
		const char* c = s.c_str();
		ImGui::LabelText("Cuted trees",c);

		if (ImGui::Button("Cut"))
		{
			cutAllTrees();
		}
	}
	ImGui::End();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}
