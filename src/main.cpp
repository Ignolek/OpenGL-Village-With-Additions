#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION  

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "SceneGraphNode.h"
#include "Orbita.h"

#include<vector>

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <stdlib.h>
#include <crtdbg.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void buffer(Model* model, vector<SceneGraphNode*> object);
void QuickSetUp(Model* model, vector<SceneGraphNode*> object, bool h_r);
void QuickSetUpTank(SceneGraphNode* object);
void QuickSetUpRoof(Model* model, vector<SceneGraphNode*> object, bool h_r);
void setLight2(Shader* shader);
unsigned int loadTexture(char const* path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1244;
const unsigned int SCR_HEIGHT = 700;

// kamera
Camera camera(glm::vec3(-300.f, 150.0f, 450.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// przesuniêcia
float villageX = 0.f;
float villageY = 0.f;
float villageZ = 0.f;
float villageRotZ = 0.f;

float houseX = 0.f;
float houseY = 100.f;
float houseZ = 0.f;

float roofX = 0.f;
float roofY = 0.f;
float roofZ = 0.f;

float alpha = 0;

// czas
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lights
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool lightmove = true;
static float pointLightXoffset = 0.0f, pointLightYoffset = 0.0f, pointLightZoffset = 0.0f;

bool reflect = true;
bool point = true;
bool dir = false;

bool render_reflect = true;
bool render_point = true;
bool render_direction = true;

static float x = 0, y = -1, z = 0;
static float x_R1 = 100, y_R1 = 300, z_R1 = 100;
static float xR1 = 0, yR1 = -1, zR1 = 0;
static float x_R2 = 400, y_R2 = 300, z_R2 = 400;
static float xR2 = 0, yR2 = -1, zR2 = 0;

static float coff = 10.0f, ocoff = 20.0f, constant = 0.5f, linear = 0.001f, quadratic = 0.000001f;

static float pointLightX = 0, pointLightY = 75, pointLightZ = 0;
static float pointLightR = 150;

static float dir_shiny = 1.0f;
static float point_shiny = 1.0f;
static float refl1_shiny = 1.0f;
static float refl2_shiny = 1.0f;

static float dir_ambient = 0.25f, dir_diffuse = 1.0f, dir_specular = 0.8f;
static float point_ambient = 0.25f, point_diffuse = 1.0f, point_specular = 0.8f;
static float refl1_ambient = 0.25f, refl1_diffuse = 1.0f, refl1_specular = 0.8f;
static float refl2_ambient = 0.25f, refl2_diffuse = 1.0f, refl2_specular = 0.8f;

static glm::vec3 dir_color = glm::vec3(0.95f, 1.0f, 0.8f);
static glm::vec3 point_color = glm::vec3(1.0f, 0.0f, 0.0f);
static glm::vec3 spot1_color = glm::vec3(0.0f, 1.0f, 0.0f);
static glm::vec3 spot2_color = glm::vec3(0.0f, 0.0f, 1.0f);

static bool mouse = false;
static bool gameMode = false;
static bool kamera = false;
static int drive = false;


// -- Caterpilar --
static float caterX = 0.0f, caterY = 0.0f, caterZ = 0.0f;
static float caterRotZ = 0.0f;
static float armPosY = 0.0f;
static float shuffleRotX = 0.0f;
static float caterWheelRotXL = 0.0f;
static float caterWheelRotXR = 0.0f;
static float caterWheelRotZ = 0.0f;

static glm::vec3 caterpilarDirection = glm::vec3(0.0f, 0.0f, 1.0f);
// ----------------

void readMat4(string title, glm::mat4 m) 
{
	std::cout << title << std::endl;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << m[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

void readVec3(string title, glm::vec3 v) {
	std::cout << title << std::endl;
	std::cout << v.x << "\t" << v.y << "\t" << v.z << "\n";
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main()
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// GL 4.3 + GLSL 430
	const char* glsl_version = "#version 400";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Village + caterpilar Ignacy Olesinski 224392", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
// SET UP FUNCTIONS FOR CAMERA
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// build and compile shaders
	// -------------------------
	Shader basicShader("./res/shaders/model.vert", "./res/shaders/model.frag");
	Shader basicLightShader("./res/shaders/model.vert", "./res/shaders/lightings.frag");
	Shader basicCaterpilarShader("./res/shaders/model.vert", "./res/shaders/lightings.frag");
	Shader basicPointLightShader("./res/shaders/model.vert", "./res/shaders/model.frag");
	Shader instancehouseShader("./res/shaders/instanceBuilding.vert", "./res/shaders/lightings.frag");
	Shader instanceroofShader("./res/shaders/instanceBuilding.vert", "./res/shaders/lightings.frag");
	Shader instancegrassShader("./res/shaders/instanceBuilding.vert", "./res/shaders/lightings.frag");
	Shader cubeShader("./res/shaders/cubemap.vert", "./res/shaders/cubemap.frag");
	Shader cubeShader2("./res/shaders/cubemap.vert", "./res/shaders/cubemap2.frag");
	Shader skyboxShader("./res/shaders/skybox.vert", "./res/shaders/skybox.frag");

	// load models
	// -----------
	Model m_house("./res/models/dom/dom.obj");
	Model m_roof("./res/models/dach/dach.obj");
	Model m_point("./res/models/punktowe/punkt.obj");
	Model m_grass("./res/models/trawa/trawa.obj");
	Model m_directional("./res/models/kierunkowe/kierunek.obj");
	Model m_spot("./res/models/reflektor/refl.obj");
	Model m_caterpilar("./res/models/caterpillar/caterpilar.obj");
	Model m_arm("./res/models/caterpillar/arm.obj");
	Model m_shuffle("./res/models/caterpillar/shuffle.obj");
	Model m_wheel("./res/models/caterpillar/wheel.obj");
	Model m_cabinet("./res/models/caterpillar/cabinet.obj");
	Model m_mirror("./res/models/caterpillar/mirror.obj");
	Model m_phantom("./res/models/caterpillar/phantom.obj");

	float skyboxVertices[] = 
	{
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// scene graph

	SceneGraphNode root(&m_point, &basicShader);

	SceneGraphNode lightTestCube(&m_cabinet, &cubeShader);
	lightTestCube.transform.position = glm::vec3(-500, 0, 500);
	lightTestCube.transform.scale = glm::vec3(1, 1, 1);

	root.addChild(&lightTestCube);
	lightTestCube.setParent(&root);

	SceneGraphNode lightTestCube2(&m_cabinet, &cubeShader2);
	lightTestCube2.transform.position = glm::vec3(-700, 0, 500);
	lightTestCube2.transform.scale = glm::vec3(1, 1, 1);

	root.addChild(&lightTestCube2);
	lightTestCube2.setParent(&root);
	
	SceneGraphNode phantomLF(&m_phantom, &basicCaterpilarShader);
	SceneGraphNode phantomRF(&m_phantom, &basicCaterpilarShader);
	SceneGraphNode caterpilar(&m_caterpilar, &basicCaterpilarShader);
	SceneGraphNode arm(&m_arm, &basicCaterpilarShader);
	SceneGraphNode shuffle(&m_shuffle, &basicCaterpilarShader);
	SceneGraphNode wheelLF(&m_wheel, &basicCaterpilarShader);
	SceneGraphNode wheelRF(&m_wheel, &basicCaterpilarShader);
	SceneGraphNode wheelLB(&m_wheel, &basicCaterpilarShader);
	SceneGraphNode wheelRB(&m_wheel, &basicCaterpilarShader);
	SceneGraphNode cabinet(&m_cabinet, &cubeShader2);
	SceneGraphNode mirror(&m_mirror, &cubeShader);

	phantomLF.transform.position = glm::vec3(20, 0, 27);
	phantomRF.transform.position = glm::vec3(-20, 0, 27);
	caterpilar.transform.position = glm::vec3(-100, 18, 0);
	arm.transform.position = glm::vec3(0, 5, 50);
	armPosY = arm.transform.position.y;
	shuffle.transform.position = glm::vec3(0, 15, 20);
	wheelLF.transform.position = glm::vec3(0, 0, 0);
	wheelRF.transform.position = glm::vec3(0, 0, 0);
	wheelLB.transform.position = glm::vec3(20, 0, -25);
	wheelRB.transform.position = glm::vec3(-20, 0, -25);
	cabinet.transform.position = glm::vec3(0, 63, 0);
	mirror.transform.position = glm::vec3(0, 60, 0);

	root.addChild(&caterpilar);

	//TEST
	caterpilar.addChild(&phantomLF);
		phantomLF.setParent(&caterpilar);
		phantomLF.addChild(&wheelLF);
			wheelLF.setParent(&phantomLF);

	caterpilar.addChild(&phantomRF);
		phantomRF.setParent(&caterpilar);
		phantomRF.addChild(&wheelRF);
			wheelRF.setParent(&phantomRF);
	//-----

	caterpilar.addChild(&arm);
	arm.setParent(&caterpilar);
		arm.addChild(&shuffle);
		shuffle.setParent(&arm);
	caterpilar.addChild(&wheelLB);
	wheelLB.setParent(&caterpilar);
	caterpilar.addChild(&wheelRB);
	wheelRB.setParent(&caterpilar);

	caterpilar.addChild(&cabinet);
	cabinet.setParent(&caterpilar);

	caterpilar.addChild(&mirror);
	mirror.setParent(&caterpilar);

	caterpilar.transform.rotation = caterpilarDirection;
	SceneGraphNode spot1(&m_spot, &basicShader);
	spot1.transform.position = glm::vec3(x_R1 - 100, y_R1, z_R1 - 100);
	spot1.transform.scale = glm::vec3(1, 1, 1);
	//root.addChild(&spot1);
	//spot1.setParent(&root);

	SceneGraphNode spot2(&m_spot, &basicShader);
	spot2.transform.position = glm::vec3(x_R2 - 100, y_R2, z_R2 - 100);
	spot2.transform.scale = glm::vec3(1, 1, 1);
	//root.addChild(&spot2);
	//spot2.setParent(&root);

	SceneGraphNode directional(&m_directional, &basicShader);
	directional.transform.position = glm::vec3(0, 500, 0);

	vector<SceneGraphNode*> Grass;
	for (int i = 0; i < 40000; i++) {
		Grass.push_back(new SceneGraphNode(&m_grass, &instancegrassShader));
		root.addChild(Grass[i]);
		Grass[i]->setParent(&root);
	}
	root.addChildInst(Grass[0]);
	Grass[0]->size = 40000;
	QuickSetUp(&m_house, Grass, 0);

	vector<SceneGraphNode*> Houses;
	for (int i = 0; i < 40000; i++) {
		Houses.push_back(new SceneGraphNode(&m_house, &instancehouseShader));
		root.addChild(Houses[i]);
		Houses[i]->setParent(&root);
	}
	root.addChildInst(Houses[0]);
	Houses[0]->size = 40000;
	QuickSetUp(&m_house, Houses, 0);

	vector<SceneGraphNode*> Roofs;
	for (int i = 0; i < 40000; i++) {
		Roofs.push_back(new SceneGraphNode(&m_roof, &instanceroofShader));
		Houses[i]->addChild(Roofs[i]);
		Roofs[i]->setParent(Houses[i]);
	}
	root.addChildInst(Roofs[0]);
	Roofs[0]->size = 40000;
	QuickSetUpRoof(&m_roof, Roofs, 1);

	SceneGraphNode PointLightSceneGraphNode(&m_house, &basicPointLightShader);
	root.addChild(&PointLightSceneGraphNode);
	Orbita PLO_Orbita(&PointLightSceneGraphNode);

	// cube reflect
	SceneGraphNode reflect_cube(&m_house, &cubeShader);
	reflect_cube.transform.position = glm::vec3(-24.5, 23, 45);
	reflect_cube.transform.scale = glm::vec3(0.04, 0.15, 0.015);

	// cube refract
	SceneGraphNode refract_cube(&m_house, &cubeShader2);
	refract_cube.transform.position = glm::vec3(0, 22, 23.75);
	refract_cube.transform.scale = glm::vec3(0.4, 0.20, 0.3);

	/*caterpilar.addChild(&refract_cube);
	refract_cube.setParent(&caterpilar);
	caterpilar.addChild(&reflect_cube);
	reflect_cube.setParent(&caterpilar);*/

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"./res/textures/skybox/right.jpg",
		"./res/textures/skybox/left.jpg",
		"./res/textures/skybox/bottom.jpg",
		"./res/textures/skybox/top.jpg",
		"./res/textures/skybox/front.jpg",
		"./res/textures/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// cubeShader configuration
	// --------------------
	reflect_cube.shader->use();
	reflect_cube.shader->setInt("texture1", 0);
	reflect_cube.shader->setInt("ref", 1);

	refract_cube.shader->use();
	refract_cube.shader->setInt("texture1", 0);
	refract_cube.shader->setInt("ref", 2);

	lightTestCube.shader->use();

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	root.dirty = true;
	root.Update_World();
	buffer(&m_house, Houses);
	buffer(&m_roof, Roofs);
	buffer(&m_grass, Grass);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// POLL EVENTS
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// input
		// -----
		processInput(window);

		// --------------IMGUI------------------
		{

			ImGui::Begin("Scene graph's transformations");

			if (ImGui::CollapsingHeader("Village position"))
			{
				if (ImGui::SliderFloat("X##village", &villageX, -100.0f, 100.0f)) {
					root.transform.position = glm::vec3(villageX, 0, 0);
					root.dirty = true;
					root.Update_World();
					buffer(&m_house, Houses);
					buffer(&m_roof, Roofs);
					buffer(&m_grass, Grass);
				}
				if (ImGui::SliderFloat("Y##village", &villageY, -100.0f, 100.0f)) {
					root.transform.position = glm::vec3(0, villageY, 0);
					root.dirty = true;
					root.Update_World();
					buffer(&m_house, Houses);
					buffer(&m_roof, Roofs);
					buffer(&m_grass, Grass);
				}
				if (ImGui::SliderFloat("Z##village", &villageZ, -100.0f, 100.0f)) {
					root.transform.position = glm::vec3(0, 0, villageZ);
					root.dirty = true;
					root.Update_World();
					buffer(&m_house, Houses);
					buffer(&m_roof, Roofs);
					buffer(&m_grass, Grass);
				}
				if (ImGui::SliderFloat("Rot##village", &villageRotZ, -90.0, 90.0))
				{
					root.transform.e_y = villageRotZ;
					root.dirty = true;
					root.Update_World();
					buffer(&m_house, Houses);
					buffer(&m_roof, Roofs);
					buffer(&m_grass, Grass);
				}
			}

			ImGui::Separator();

			if (ImGui::CollapsingHeader("First house position"))
			{
				if (ImGui::SliderFloat("X##House", &houseX, -100.0f, 100.0f)) {
					Houses[0]->transform.position = glm::vec3(0 + houseX, Houses[0]->transform.position.y, Houses[0]->transform.position.z);
					Houses[0]->dirty = true;
					Houses[0]->Update_World();
					root.DrawI();
					buffer(&m_house, Houses);
					buffer(&m_roof, Roofs);
				}
				if (ImGui::SliderFloat("Y##House", &houseY, -100.0f, 100.0f)) {
					Houses[0]->transform.position = glm::vec3(Houses[0]->transform.position.x, 0 + houseY, Houses[0]->transform.position.z);
					Houses[0]->dirty = true;
					Houses[0]->Update_World();
					root.DrawI();
					buffer(&m_house, Houses);
					buffer(&m_roof, Roofs);
				}
				if (ImGui::SliderFloat("Z##House", &houseZ, -100.0f, 100.0f)) {
					Houses[0]->transform.position = glm::vec3(Houses[0]->transform.position.x, Houses[0]->transform.position.y, 0 + houseZ);
					Houses[0]->dirty = true;
					Houses[0]->Update_World();
					root.DrawI();
					buffer(&m_house, Houses);
					buffer(&m_roof, Roofs);
				}
			}

			ImGui::Separator();

			if (ImGui::CollapsingHeader("First roof position"))
			{
				if (ImGui::SliderFloat("X##roof", &roofX, -100.0f, 100.0f)) {
					Roofs[0]->transform.position = glm::vec3(0 + roofX, Roofs[0]->transform.position.y, Roofs[0]->transform.position.z);
					Roofs[0]->dirty = true;
					Roofs[0]->Update_World();
					root.DrawI();
					buffer(&m_roof, Roofs);
				}
				if (ImGui::SliderFloat("Y##roof", &roofY, -100.0f, 100.0f)) {
					Roofs[0]->transform.position = glm::vec3(Roofs[0]->transform.position.x, 100.0f + roofY, Roofs[0]->transform.position.z);
					Roofs[0]->dirty = true;
					Roofs[0]->Update_World();
					root.DrawI();
					buffer(&m_roof, Roofs);
				}
				if (ImGui::SliderFloat("Z##roof", &roofZ, -100.0f, 100.0f)) {
					Roofs[0]->transform.position = glm::vec3(Roofs[0]->transform.position.x, Roofs[0]->transform.position.y, 0 + roofZ);
					Roofs[0]->dirty = true;
					Roofs[0]->Update_World();
					root.DrawI();
					buffer(&m_roof, Roofs);
				}
			}

			ImGui::End();
		}

		{
			ImGui::Begin("Switches");
			ImGui::Checkbox("Mouse Camera [R]", &mouse);
			ImGui::Separator();
			ImGui::Checkbox("Move caterpilar [M]", &gameMode);
			ImGui::Separator();
			ImGui::Checkbox("Camera on caterpilar [C]", &kamera);
			ImGui::Separator();
			ImGui::Text("[E] show cursor / [Q] hide cursor");
			ImGui::End();
		}

		{
			ImGui::Begin("Lights");

			if (ImGui::CollapsingHeader("Render and direction"))
			{
				ImGui::Checkbox("Point Light", &point);
				ImGui::Checkbox("Point Render", &render_point);

				ImGui::Separator();

				ImGui::Checkbox("Directional Light", &dir);
				ImGui::Checkbox("Directional Render", &render_direction);
				ImGui::SliderFloat("Directional x", &x, -1.0f, 1.0f);
				ImGui::SliderFloat("Directional y", &z, -1.0f, 1.0f);

				ImGui::Separator();

				ImGui::Checkbox("Spots Light", &reflect);
				ImGui::Checkbox("Spots Render", &render_reflect);
				if (ImGui::SliderFloat("Spot1 xRot", &xR1, -1.0f, 1.0f)) {
				}
				if (ImGui::SliderFloat("Spot1 zRot", &zR1, -1.0f, 1.0f)) {
				}
				ImGui::SliderFloat("Spot1 xPos", &x_R1, -1000, 1000);
				ImGui::SliderFloat("Spot1 yPos", &y_R1, -1000, 1000);
				ImGui::SliderFloat("Spot1 zPos", &z_R1, -1000, 1000);

				if (ImGui::SliderFloat("Spot2 xRot", &xR2, -1.0f, 1.0f)) {
				}
				if (ImGui::SliderFloat("Spot2 zRot", &zR2, -1.0f, 1.0f)) {
				}
				ImGui::SliderFloat("Spot2 xPos", &x_R2, -1000, 1000);
				ImGui::SliderFloat("Spot2 yPos", &y_R2, -1000, 1000);
				ImGui::SliderFloat("Spot2 zPos", &z_R2, -1000, 1000);

				ImGui::Separator();

				ImGui::ColorEdit3("Clear color", (float*)&clear_color);
			}

			if (ImGui::CollapsingHeader("Light Attributes"))
			{
				if (ImGui::TreeNode("Directional"))
				{
					ImGui::ColorEdit3("Color", (float*)&dir_color);
					ImGui::SliderFloat("Ambient", &dir_ambient, 0.0f, 1.0f);
					ImGui::SliderFloat("Diffuse", &dir_diffuse, 0.0f, 1.0f);
					ImGui::SliderFloat("Specular", &dir_specular, 0.0f, 1.0f);
					ImGui::SliderFloat("Shiny", &dir_shiny, 0.0f, 32.0f);
					ImGui::TreePop();
				}
				ImGui::Separator();

				if (ImGui::TreeNode("Point"))
				{
					ImGui::ColorEdit3("Color", (float*)&point_color);
					ImGui::SliderFloat("Ambient", &point_ambient, 0.0f, 1.0f);
					ImGui::SliderFloat("Diffuse", &point_diffuse, 0.0f, 1.0f);
					ImGui::SliderFloat("Specular", &point_specular, 0.0f, 1.0f);
					ImGui::SliderFloat("Shiny", &point_shiny, 0.0f, 32.0f);
					ImGui::TreePop();
				}
				ImGui::Separator();

				if (ImGui::TreeNode("Spots"))
				{
					ImGui::SliderFloat("constant", &constant, 0.0f, 10.0f);
					ImGui::SliderFloat("linear", &linear, 0.0f, 0.1f);
					ImGui::SliderFloat("quadratic", &quadratic, 0.0f, 0.01f);
					ImGui::SliderFloat("cutoff", &coff, 0.0f, 100.0f);
					ImGui::SliderFloat("outer cutoff", &ocoff, 0.0f, 100.0f);
					if (ImGui::TreeNode("Spot 1"))
					{
						ImGui::ColorEdit3("Color", (float*)&spot1_color);
						ImGui::SliderFloat("Ambient", &refl1_ambient, 0.0f, 1.0f);
						ImGui::SliderFloat("Diffuse", &refl1_diffuse, 0.0f, 1.0f);
						ImGui::SliderFloat("pecular", &refl1_specular, 0.0f, 1.0f);
						ImGui::SliderFloat("Shiny", &refl1_shiny, 0.0f, 32.0f);
						ImGui::TreePop();
					}
					ImGui::Separator();

					if (ImGui::TreeNode("Spot 2"))
					{
						ImGui::ColorEdit3("Color", (float*)&spot2_color);
						ImGui::SliderFloat("Ambient", &refl2_ambient, 0.0f, 1.0f);
						ImGui::SliderFloat("Diffuse", &refl2_diffuse, 0.0f, 1.0f);
						ImGui::SliderFloat("Specular", &refl2_specular, 0.0f, 1.0f);
						ImGui::SliderFloat("Shiny", &refl2_shiny, 0.0f, 32.0f);
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}

			if (ImGui::CollapsingHeader("Point position"))
			{
				ImGui::Checkbox("Move", &lightmove);
				ImGui::SliderFloat("XPos ", &pointLightXoffset, -1000.0f, 1000.0f);
				ImGui::SliderFloat("YPos", &pointLightYoffset, -500.0f, 500.0f);
				ImGui::SliderFloat("ZPos", &pointLightZoffset, -1000.0f, 1000.0f);
				ImGui::SliderFloat("Radius", &pointLightR, -1000.0f, 1000.0f);
			}

			ImGui::End();
		}

		// ------------------- Rendering ---------------------
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 90000.0f);
		if (kamera)
		{
			camera.Position = caterpilar.transform.position + glm::vec3(0, 100, -100);
		}
		glm::mat4 view = camera.GetViewMatrix();

		instancehouseShader.use();
		setLight2(&instancehouseShader);
		instancehouseShader.setMat4("projection", projection);
		instancehouseShader.setMat4("view", view);
		instancehouseShader.setMat4("model", glm::mat4(1.0f));
		instancehouseShader.setBool("point", point);
		instancehouseShader.setBool("ref", reflect);
		instancehouseShader.setBool("dir", dir);

		instanceroofShader.use();
		setLight2(&instanceroofShader);
		instanceroofShader.setMat4("projection", projection);
		instanceroofShader.setMat4("view", view);
		instanceroofShader.setMat4("model", glm::mat4(1.0f));
		instanceroofShader.setBool("point", point);
		instanceroofShader.setBool("ref", reflect);
		instanceroofShader.setBool("dir", dir);

		instancegrassShader.use();
		setLight2(&instancegrassShader);
		instancegrassShader.setMat4("projection", projection);
		instancegrassShader.setMat4("view", view);
		instancegrassShader.setMat4("model", glm::mat4(1.0f));
		instancegrassShader.setBool("point", point);
		instancegrassShader.setBool("ref", reflect);
		instancegrassShader.setBool("dir", dir);

		if (point) 
		{
			PointLightSceneGraphNode.shader->use();
			PointLightSceneGraphNode.shader->setMat4("projection", projection);
			PointLightSceneGraphNode.shader->setMat4("view", view);

			if (lightmove) 
			{
				alpha += 0.05;
				PointLightSceneGraphNode.dirty = true;
				PointLightSceneGraphNode.transform.position = glm::vec3(cos(alpha) * pointLightR + pointLightX + pointLightXoffset, pointLightY + pointLightYoffset, sin(alpha) * pointLightR + pointLightZ + pointLightZoffset);
				PointLightSceneGraphNode.Update_World();
				PointLightSceneGraphNode.shader->setMat4("model", PointLightSceneGraphNode.transform.World_mat);
				lightPos = PointLightSceneGraphNode.transform.position;
			}
			else 
			{
				PointLightSceneGraphNode.dirty = true;
				PointLightSceneGraphNode.transform.position = glm::vec3(pointLightX + pointLightXoffset, pointLightY + pointLightYoffset, pointLightZ + pointLightZoffset);
				PointLightSceneGraphNode.Update_World();
				PointLightSceneGraphNode.shader->setMat4("model", PointLightSceneGraphNode.transform.World_mat);
				lightPos = PointLightSceneGraphNode.transform.position;
			}
			if (render_point) 
			{
				m_point.Draw(*PointLightSceneGraphNode.shader);
			}
		}

		if (reflect) {
			spot1.shader->use();
			spot1.shader->setMat4("projection", projection);
			spot1.shader->setMat4("view", view);
			spot1.transform.position = glm::vec3(x_R1 - 100, y_R1, z_R1 - 100);
			spot1.transform.e_y = xR1;
			spot1.transform.e_x = -zR1;

			spot2.shader->use();
			spot2.shader->setMat4("projection", projection);
			spot2.shader->setMat4("view", view);
			spot2.transform.position = glm::vec3(x_R2 - 100, y_R2, z_R2 - 100);
			spot2.transform.e_y = xR2;
			spot2.transform.e_x = -zR2;

			if (render_reflect) {
				spot1.Draw();
				spot2.Draw();
			}
		}

		if (dir) {
			directional.shader->use();
			directional.shader->setMat4("projection", projection);
			directional.shader->setMat4("view", view);
			directional.transform.e_y = x; //-x
			directional.transform.e_x = -z; //y

			if (render_direction) 
			{
				directional.Draw();
			}
		}

		root.DrawI();

		caterpilar.shader->use();
		setLight2(&basicCaterpilarShader);
		caterpilar.shader->setMat4("projection", projection);
		caterpilar.shader->setMat4("view", view);
		caterpilar.shader->setMat4("model", caterpilar.transform.World_mat);
		caterpilar.shader->setBool("point", point);
		caterpilar.shader->setBool("ref", reflect);
		caterpilar.shader->setBool("dir", dir);

		caterpilar.transform.e_z = caterRotZ;
		arm.transform.position.y = armPosY;
		shuffle.transform.e_x = shuffleRotX;

		// Wheel rot
		wheelLF.transform.e_x = caterWheelRotXL;
		wheelLB.transform.e_x = caterWheelRotXL;
		wheelRF.transform.e_x = caterWheelRotXR;
		wheelRB.transform.e_x = caterWheelRotXR;

		phantomLF.transform.e_z = caterWheelRotZ;
		phantomRF.transform.e_z = caterWheelRotZ;
		
		glm::mat4 catRot = glm::eulerAngleXZY(caterpilar.transform.e_x, caterpilar.transform.e_y, caterpilar.transform.e_z);
		caterpilar.transform.Local_mat += catRot;

		if (drive == 1)
		{
			caterpilar.transform.position += glm::vec3(sin(caterRotZ), 0, cos(caterRotZ)) * 3.0f;
			drive = 0;
		}
		else if (drive == -1)
		{
			caterpilar.transform.position -= glm::vec3(sin(caterRotZ), 0, cos(caterRotZ)) * 1.0f;
			drive = 0;
		}
		caterpilar.Draw();

		glm::mat4 model = glm::mat4(1.0f);

		reflect_cube.shader->use();
		reflect_cube.shader->setMat4("model", model);
		reflect_cube.shader->setMat4("view", view);
		reflect_cube.shader->setMat4("projection", projection);
		reflect_cube.shader->setVec3("cameraPos", camera.Position);

		//reflect_cube.Draw();

		refract_cube.shader->use();
		refract_cube.shader->setMat4("model", model);
		refract_cube.shader->setMat4("view", view);
		refract_cube.shader->setMat4("projection", projection);
		refract_cube.shader->setVec3("cameraPos", camera.Position);

		//refract_cube.Draw();

		lightTestCube.shader->use();
		lightTestCube.shader->setMat4("model", model);
		lightTestCube.shader->setMat4("view", view);
		lightTestCube.shader->setMat4("projection", projection);
		lightTestCube.shader->setVec3("cameraPos", camera.Position);

		//lightTestCube.Draw();
		
		lightTestCube2.shader->use();
		lightTestCube2.shader->setMat4("model", model);
		lightTestCube2.shader->setMat4("view", view);
		lightTestCube2.shader->setMat4("projection", projection);
		lightTestCube2.shader->setVec3("cameraPos", camera.Position);

		//lightTestCube2.Draw();

		// ------------ SKYBOX --------------
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default


		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	///////////////////////////////////
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		gameMode = !gameMode;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		mouse = !mouse;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		kamera = !kamera;
	}

	if (!gameMode)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime * 200);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime * 200);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime * 200);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime * 200);
	}
	else
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			drive = 1;
			caterWheelRotXL += 0.1;
			caterWheelRotXR += 0.1;

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				caterRotZ += 0.03;

				if (caterWheelRotZ < 0.3)
				{
					caterWheelRotZ += 0.05;
				}

				caterWheelRotXL += 0.08;
				caterWheelRotXR += 0.08;
			}
			else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				caterRotZ -= 0.03;

				if (caterWheelRotZ > -0.3)
				{
					caterWheelRotZ -= 0.05;
				}

				caterWheelRotXL += 0.08;
				caterWheelRotXR += 0.08;
			}
			else
			{
				caterWheelRotZ = 0;
			}
		}

		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			drive = -1;
			caterWheelRotXL -= 0.1;
			caterWheelRotXR -= 0.1;

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			{
				caterRotZ -= 0.03;

				if (caterWheelRotZ < 0.3)
				{
					caterWheelRotZ += 0.05;
				}

				caterWheelRotXL -= 0.08;
				caterWheelRotXR -= 0.08;
			}
			else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			{
				caterRotZ += 0.03;

				if (caterWheelRotZ > -0.3)
				{
					caterWheelRotZ -= 0.05;
				}

				caterWheelRotXL -= 0.08;
				caterWheelRotXR -= 0.08;
			}

			else
			{
				caterWheelRotZ = 0;
			}
			
		}

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
		{
			if (shuffleRotX > -0.7f)
				shuffleRotX -= 0.02;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
		{
			if (shuffleRotX < 0.7f)
				shuffleRotX += 0.02;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			if(armPosY > 0.2)
			armPosY -= 0.2;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			if(armPosY < 20)
			armPosY += 0.2;
		}
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (mouse) {
		return;
	}
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)

{
	camera.ProcessMouseScroll(yoffset);
}

void buffer(Model* model, vector<SceneGraphNode*> object) 
{   //robiona jest tablica instancji, przypisywane s¹ wskazniki na atryburty wierzcho³ków
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[object.size()];
	for (unsigned int i = 0; i < object.size(); i++)
	{
		modelMatrices[i] = object[i]->transform.World_mat;
	}

	// configure instanced array
	// -------------------------
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glm::mat4 m(1.f);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, object.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	// -----------------------------------------------------------------------------------------------------------------------------------
	for (unsigned int i = 0; i < model->meshes.size(); i++)
	{
		unsigned int VAO = model->meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindVertexArray(0);
	}
}

void QuickSetUp(Model* model, vector<SceneGraphNode*> object, bool h_r) {  //obliczane po³o¿enia instancji
	int placed = 0;
	float x = 0, z = 0, y = 0;

	for (unsigned int i = 0; i < object.size(); i++)
	{
		if (placed == 200) 
		{
			z += 300.0f;
			placed = 0;
			x = 0.0f;
		}

		object[i]->transform.position = glm::vec3(x, y, z);
		object[i]->transform.World_mat = glm::translate(object[i]->transform.World_mat, object[i]->transform.position);
		placed++;
		x += 300.0f;
	}
}

void QuickSetUpRoof(Model* model, vector<SceneGraphNode*> object, bool h_r) {
	int placed = 0;
	float x = 0, z = 0, y = 100.0f;
	for (unsigned int i = 0; i < object.size(); i++)
	{
		object[i]->transform.position = glm::vec3(x, y, z);

		// transaltion	
		object[i]->transform.World_mat = glm::translate(object[i]->transform.World_mat, object[i]->transform.position);

		//// rotation
		//object[i]->transform.e_x = 0;
		//object[i]->transform.e_y = 0;
		//object[i]->transform.e_z = 0;

		//// scale
		//object[i]->transform.scale = glm::vec3(1.0f);

		placed++;
	}
}

void setLight2(Shader* shader) {
	shader->setVec3("dirlight.direction", x, y, z);
	shader->setVec3("dirlight.ambient", glm::vec3(dir_ambient));
	shader->setVec3("dirlight.diffuse", glm::vec3(dir_diffuse));
	shader->setVec3("dirlight.specular", glm::vec3(dir_specular));
	shader->setFloat("dirlight.shiny", dir_shiny);
	shader->setVec3("dirlight.dircolor", dir_color);

	shader->setVec3("viewPos", camera.Position);

	shader->setVec3("lightPoint.position", lightPos);
	shader->setVec3("lightPoint.ambient", glm::vec3(point_ambient));
	shader->setVec3("lightPoint.diffuse", glm::vec3(point_diffuse));
	shader->setVec3("lightPoint.specular", glm::vec3(point_specular));
	shader->setFloat("lightPoint.shiny", point_shiny);
	shader->setFloat("lightPoint.constant", 1);
	shader->setFloat("lightPoint.linear", 0);
	shader->setFloat("lightPoint.quadratic", 0.00005);
	shader->setVec3("lightPoint.pointcolor", point_color);

	shader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	shader->setFloat("material.shininess", 64.0f);

	shader->setVec3("spotLight[0].position", glm::vec3(x_R1 - 100, y_R1, z_R1 - 100));
	shader->setVec3("spotLight[0].direction", glm::vec3(xR1, yR1, zR1));
	shader->setVec3("spotLight[0].ambient", glm::vec3(refl1_ambient));
	shader->setVec3("spotLight[0].diffuse", glm::vec3(refl1_diffuse));
	shader->setVec3("spotLight[0].specular", glm::vec3(refl1_specular));
	shader->setFloat("spotLight[0].shiny", refl1_shiny);
	shader->setFloat("spotLight[0].constant", constant);
	shader->setFloat("spotLight[0].linear", linear);
	shader->setFloat("spotLight[0].quadratic", quadratic / 100);
	shader->setFloat("spotLight[0].cutOff", glm::cos(glm::radians(coff)));
	shader->setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(ocoff)));
	shader->setVec3("spotLight[0].spot1color", spot1_color);

	shader->setVec3("spotLight[1].position", glm::vec3(x_R2 - 100, y_R2, z_R2 - 100));
	shader->setVec3("spotLight[1].direction", glm::vec3(xR2, yR2, zR2));
	shader->setVec3("spotLight[1].ambient", glm::vec3(refl2_ambient));
	shader->setVec3("spotLight[1].diffuse", glm::vec3(refl2_diffuse));
	shader->setVec3("spotLight[1].specular", glm::vec3(refl2_specular));
	shader->setFloat("spotLight[1].shiny", refl2_shiny);
	shader->setFloat("spotLight[1].constant", constant);
	shader->setFloat("spotLight[1].linear", linear);
	shader->setFloat("spotLight[1].quadratic", quadratic / 100);
	shader->setFloat("spotLight[1].cutOff", glm::cos(glm::radians(coff)));
	shader->setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(ocoff)));
	shader->setVec3("spotLight[1].spot2color", spot2_color);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// -------------------------------------------------------
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}