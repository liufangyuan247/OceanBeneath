#include "funcDecl.h"
#include <set>
#include "camera.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <random>
using namespace std;

#define uniform 

GLFWwindow *window;

float windowWidth = 800;
float windowHeight = 600;

GLuint geometryFBO;
Texture *colorRenderTexture;
Texture *worldPosRenderTexture;
Texture *depthRenderTexture;

GLuint normalShader;
GLuint causticShader;
GLuint oceanSurfaceShader;
GLuint raymarchShader;

Mesh oceanSurface;
Mesh seabed;
Mesh skySphere;

GLuint seaVertexColorTex;
GLuint seaColor0;
GLuint seaColor1;
GLuint seaColor2;
GLuint seaNormal0;
GLuint seaNormal1;
GLuint seaNormal2;

vector<GLuint> normalTextureSeq;
vector<GLuint> causticTextureSeq;

GLuint skyboxHDRTex;

bool buttonPressed = false;
double cursorLastX;
double cursorLastY;

Camera camera;

vec3 lightDir = vec3(1);
float lightYaw = 64.6;
float lightPitch = 64.9;
float bumpGain = 37;
float grainNoiseGain = 0.157;
float grainNoiseScale = 42;

vec3 absorbColor = vec3(0, 0.1412, 0.3020);
uniform float luminanceGain = 0.02;
uniform float scatterGain = 0.005;

default_random_engine generator;
uniform_real_distribution<float> distribution(-1,1);

GLuint particleVAO;
GLuint particleVBO;
GLuint particleShader;
int particleCount = 1000000;
bool renderParticle = true;

GLuint bubbleVAO;
GLuint bubbleVBO;
GLuint bubbleClusterVBO;
GLuint bubbleShader;
GLuint bubbleTex;
int bubbleCount = 500;

struct BubbleData 
{
	vec4 pos;	//w =size
	vec3 velocity;

	void Update(float dt)
	{
		pos += vec4(vec3(dt*velocity),0);
		velocity.y += dt * 0.5;
		if (length(velocity) > 3)
		{
			velocity = normalize(velocity) * 3.0f;
		}
		if (pos.y>90)
		{
			pos = vec4(0, distribution(generator)*5.0f-5,0, distribution(generator)*0.4f+0.6f);
			velocity = vec3(distribution(generator), 0, distribution(generator))*0.2f;
		}
	}
};

void InitParticle();
void UpdateParticle(float dt);


void Render();
void InitializeOpenGL();
void Resize(GLFWwindow *, int w, int h);
void CursorMove(GLFWwindow *, double, double);
void MouseButton(GLFWwindow *, int button, int action, int flag);
void KeyPress(GLFWwindow*, int, int, int, int);

int main(int, char **)
{
	glfwInit();

	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Sea", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetWindowSizeCallback(window, Resize);
	glfwSetCursorPosCallback(window, CursorMove);
	glfwSetMouseButtonCallback(window, MouseButton);
	glfwSetKeyCallback(window, KeyPress);

	if (glewInit() != GLEW_OK)
	{
		cout << "glew init failed!" << endl;
	}

	InitializeOpenGL();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	while (!glfwWindowShouldClose(window))
	{
		static auto lt = glfwGetTime();
		auto t = glfwGetTime();
		float dt = t - lt;
		Update(dt);
		lt = t;

		glfwPollEvents();
		/*
		*/
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Setting Window");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			static float lastFPST = t;
			static int fpsCount = 0;
			static float fps = 0;
			if (t-lastFPST>1)
			{
				fps = fpsCount / (t - lastFPST);
				lastFPST = t;
				fpsCount = 0;
			}
			++fpsCount;
			ImGui::Text("fps:%2.03f", fps);

			ImGui::SliderFloat("lightYaw", &lightYaw, 0, 360);
			ImGui::SliderFloat("lightPitch", &lightPitch, 0, 90);
			lightDir = vec3(cos(radians(lightPitch))*sin(radians(lightYaw)), sin(radians(lightPitch)), cos(radians(lightPitch))*cos(radians(lightYaw)));


			ImGui::SliderFloat("bumpGain", &bumpGain, 5, 100);
			ImGui::SliderFloat("grainNoiseGain", &grainNoiseGain, 0, 1);
			ImGui::SliderFloat("grainNoiseScale", &grainNoiseScale, 1, 100);

			ImGui::ColorEdit3("absorbColor", (float *)&absorbColor);
			ImGui::SliderFloat("luminanceGain", &luminanceGain, 0.00001, 0.05);
			ImGui::SliderFloat("scatterGain", &scatterGain, 0.0001, 0.1);

			ImGui::Checkbox("render particle", &renderParticle);

			ImGui::End();
		}
		Render();
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void InitializeOpenGL()
{
	//normalShader = CreateShaderProgramFromFile("assets\\shaders//normal.vs", "assets\\shaders//normal.fs");
	normalShader = CreateShaderProgramFromFile("assets\\shaders//normal.vs", "assets\\shaders//normal.gs", "assets\\shaders//normal.fs");
	causticShader = CreateShaderProgramFromFile("assets\\shaders//caustic.vs", "assets\\shaders//caustic.fs");
	oceanSurfaceShader = CreateShaderProgramFromFile("assets\\shaders//oceanSurface.vs", "assets\\shaders//oceanSurface.fs");
	raymarchShader = CreateShaderProgramFromFile("assets\\shaders//raymarch.vs", "assets\\shaders//raymarch.fs");

	skyboxHDRTex = LoadTextureF("assets//textures//Etnies_Park_Center_3k_1.hdr");
	bubbleTex = LoadTexture("assets//bubbles//sheet.png");

	oceanSurface.LoadRawFloatData("assets\\models\\quad.model");
	seabed.LoadRawFloatData("assets\\models\\sea wrok flow ready.model");
	skySphere.LoadRawFloatData("assets\\models\\skySphere.model");

	for (int i = 1; i < 40; ++i)
	{
		char texFile[260] = "";
		sprintf_s(texFile, "assets\\normals\\%04d.png", i);
		normalTextureSeq.push_back(LoadTexture(texFile));
	}
	for (int i = 1; i < 40; ++i)
	{
		char texFile[260] = "";
		sprintf_s(texFile, "assets\\caustics\\%04d.png", i);
		causticTextureSeq.push_back(LoadTexture(texFile));
	}

	seaVertexColorTex = LoadTexture("assets//textures//VertexColorBake.png");

	GLuint *tex[] = {
	&seaColor0 ,
	&seaNormal0,
	&seaColor1 ,
	&seaNormal1,
	&seaColor2 ,
	&seaNormal2,
	};

	const char *texNames[] = {
		"assets//textures//image.png",
		"assets//textures//image_n.png",
		"assets//textures//Tan Slate.jpg",
		"assets//textures//Tan Slate_n.png",
		"assets//textures//Fen Stone.jpg",
		"assets//textures//Fen Stone_n.png",
	};

	for (int i = 0; i < 6; ++i)
	{
		*tex[i] = LoadTexture(texNames[i]);
	}


	glGenFramebuffers(1,&geometryFBO);
	colorRenderTexture = new Texture(GL_RGBA32F,windowWidth,windowHeight);
	worldPosRenderTexture = new Texture(GL_RGB32F, windowWidth, windowHeight);
	depthRenderTexture = new Texture(GL_DEPTH24_STENCIL8, windowWidth, windowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, geometryFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorRenderTexture->id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, worldPosRenderTexture->id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthRenderTexture->id, 0);

	GLuint drawbuffers[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(size(drawbuffers), drawbuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "frame buffer error!" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	camera.cameraPos = vec3(0, -100, 0);


	InitParticle();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void InitParticle()
{
	{
		glGenVertexArrays(1, &particleVAO);
		glBindVertexArray(particleVAO);
		glGenBuffers(1, &particleVBO);
		glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*particleCount, 0, GL_STATIC_DRAW);
		vec3 *buffer = (vec3*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		vec3 scale = vec3(300, 50, 150);
		for (int i = 0; i < particleCount; ++i)
		{
			*buffer++ = vec3(distribution(generator), distribution(generator), distribution(generator))*scale;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		particleShader = CreateShaderProgramFromFile("assets\\shaders//particle.vs", "assets\\shaders//particle.gs", "assets\\shaders//particle.fs");

	}

	{
		glGenVertexArrays(1, &bubbleVAO);
		glBindVertexArray(bubbleVAO);
		glGenBuffers(1, &bubbleVBO);
		glBindBuffer(GL_ARRAY_BUFFER, bubbleVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(BubbleData)*bubbleCount, 0, GL_STATIC_DRAW);
		BubbleData *buffer = (BubbleData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		for (int i = 0; i < bubbleCount; ++i)
		{
			buffer->pos = vec4(0, distribution(generator) * 50 + 50, 0, distribution(generator)*0.4f + 0.6f);
			buffer->velocity = vec3(distribution(generator),0, distribution(generator))*0.2f;
			buffer++;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BubbleData), 0);

		glGenBuffers(1, &bubbleClusterVBO);
		glBindBuffer(GL_ARRAY_BUFFER, bubbleClusterVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)* 1000, 0, GL_STATIC_DRAW);
		void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		FILE*fp = NULL;
		fopen_s(&fp, "assets//bubbles//bubbles.bin", "rb");
		if (fp)
		{
			fread(ptr, sizeof(vec3), 1000, fp);
			fclose(fp);
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(1, 1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		bubbleShader = CreateShaderProgramFromFile("assets\\shaders//bubbles.vs", "assets\\shaders//bubbles.gs", "assets\\shaders//bubbles.fs");
	}


}

void UpdateParticle(float dt)
{

	glBindBuffer(GL_ARRAY_BUFFER, bubbleVBO);
	BubbleData *buffer = (BubbleData*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	for (int i = 0; i < bubbleCount; ++i)
	{
		buffer->Update(dt);
		buffer++;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}


void Render()
{
	int animaitonFrameIndex = int(glfwGetTime() * 20) % normalTextureSeq.size();
	//geemetry pass
	glBindFramebuffer(GL_FRAMEBUFFER, geometryFBO);
	GLuint drawbuffers[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(size(drawbuffers), drawbuffers);

	vec3 up = vec3(0, 1, 0);
	if (length(up - normalize(lightDir)) < 1e-6)
	{
		up = vec3(1, 0, 0);
	}
	mat4 lightView = lookAt(normalize(lightDir)*10.0f, vec3(), up);
	mat4 lightProjection = ortho<float>(-2, 2, -2, 2, 0, 500);


	glViewport(0, 0, windowWidth, windowHeight);
	float defaultColorData[] = { absorbColor.x,absorbColor.y,absorbColor.z,1};
	float defaultPosData[] = { 0,-1000,0,-1000 };
	float defaultNormalReflectData[] = { 0,0,0,0 };
	glClearBufferfv(GL_COLOR, 0, defaultColorData);
	glClearBufferfv(GL_COLOR, 1, defaultPosData);
	glClearBufferfv(GL_COLOR, 2, defaultNormalReflectData);
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);

	{
		glUseProgram(normalShader);

		SetUniform(normalShader, "view", camera.View());
		SetUniform(normalShader, "projection", camera.Projection());
		SetUniform(normalShader, "lightDir", lightDir);
		SetUniform(normalShader, "absorbColor", absorbColor);

		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, normalTextureSeq[animaitonFrameIndex]);
		glBindTexture(GL_TEXTURE_2D, seaVertexColorTex);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, seaColor0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, seaNormal0);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, seaColor1);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, seaNormal1);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, seaColor2);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, seaNormal2);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, causticTextureSeq[animaitonFrameIndex]);

		SetUniform(normalShader, "vertexColor", 0);
		SetUniform(normalShader, "colorTex1", 1);
		SetUniform(normalShader, "normalTex1", 2);
		SetUniform(normalShader, "colorTex2", 3);
		SetUniform(normalShader, "normalTex2", 4);
		SetUniform(normalShader, "colorTex3", 5);
		SetUniform(normalShader, "normalTex3", 6);
		SetUniform(normalShader, "causticsTex", 7);
		

		SetUniform(normalShader, "bumpGain", bumpGain);
		SetUniform(normalShader, "grainNoiseGain", grainNoiseGain);
		SetUniform(normalShader, "grainNoiseScale", grainNoiseScale);

		SetUniform(normalShader, "eyePos", camera.cameraPos);

		SetUniform(normalShader, "lightView", mat3(lightView));

		//seabed.model = scale(mat4(1), vec3(0.01));
		mat3 tangentSpaceToWorld = transpose(inverse(mat3(seabed.model)));
		SetUniform(normalShader, "tangentSpaceToWorld", tangentSpaceToWorld);
		seabed.model = translate(mat4(1),vec3(0,-100,0));
		seabed.Draw(normalShader);
	}

	{
		glUseProgram(oceanSurfaceShader);

		SetUniform(oceanSurfaceShader, "view", camera.View());
		SetUniform(oceanSurfaceShader, "projection", camera.Projection());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, normalTextureSeq[animaitonFrameIndex]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, skyboxHDRTex);
		
		SetUniform(oceanSurfaceShader, "absorbColor", absorbColor);

		SetUniform(oceanSurfaceShader, "eyePos", camera.cameraPos);

		SetUniform(oceanSurfaceShader, "normalTex", 0);
		SetUniform(oceanSurfaceShader, "skyboxHDRTex", 1);

		skySphere.model = scale(mat4(1), vec3(1000));
		skySphere.Draw(oceanSurfaceShader);

		oceanSurface.model = scale(mat4(1), vec3(1000));
		oceanSurface.Draw(oceanSurfaceShader);

	}
	//particle pass
	{
		if (renderParticle)
		{

			GLuint drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(size(drawbuffers), drawbuffers);

			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(particleShader);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, causticTextureSeq[animaitonFrameIndex]);

			SetUniform(particleShader, "causticTex", 0);
			SetUniform(particleShader, "time", (float)glfwGetTime() / 30.0f);

			SetUniform(particleShader, "view", camera.View());
			SetUniform(particleShader, "projection", camera.Projection());
			SetUniform(particleShader, "model", translate(mat4(1), vec3(0, -60, 0)));

			SetUniform(particleShader, "eyePos", camera.cameraPos);
			SetUniform(particleShader, "lightView", mat3(lightView));

			glBindVertexArray(particleVAO);
			glDrawArrays(GL_POINTS, 0, particleCount);
			glBindVertexArray(0);

			glUseProgram(bubbleShader);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, bubbleTex);

			SetUniform(bubbleShader, "bubbleTex", 0);
			SetUniform(bubbleShader, "time", (float)glfwGetTime() / 30.0f);

			SetUniform(bubbleShader, "view", camera.View());
			SetUniform(bubbleShader, "projection", camera.Projection());

			SetUniform(bubbleShader, "eyePos", camera.cameraPos);

			SetUniform(bubbleShader, "model", translate(mat4(1), vec3(0,-110,0)));
			glBindVertexArray(bubbleVAO);
			glDrawArraysInstanced(GL_POINTS, 0, bubbleCount,1000);
			glBindVertexArray(0);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}

	}

	//ray marching pass
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(raymarchShader);

		SetUniform(raymarchShader, "view", lookAt(vec3(0,-1,0),vec3(),vec3(1,0,0)));
		SetUniform(raymarchShader, "projection", ortho<float>(-1,1,-1,1,-10,10));

		colorRenderTexture->Bind(0);
		worldPosRenderTexture->Bind(1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, causticTextureSeq[animaitonFrameIndex]);

		SetUniform(raymarchShader, "colorTexture", 0);
		SetUniform(raymarchShader, "worldPosTexture", 1);
		SetUniform(raymarchShader, "causticTexture", 2);

		SetUniform(raymarchShader, "lightView", mat3(lightView));

		SetUniform(raymarchShader, "eyePos", camera.cameraPos);
		SetUniform(raymarchShader, "luminanceGain", luminanceGain);
		SetUniform(raymarchShader, "scatterGain", scatterGain);

		oceanSurface.Draw(raymarchShader);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
	}

	glUseProgram(0);
}


void Update(float dt)
{
	camera.Update(dt);
	UpdateParticle(dt);
}

void Resize(GLFWwindow *, int w, int h)
{
	windowWidth = w;
	windowHeight = h;
	camera.ResizeCamera(w, h);

	colorRenderTexture->Resize(w,h);
	worldPosRenderTexture->Resize(w, h);
	depthRenderTexture->Resize(w, h);

}

void CursorMove(GLFWwindow *, double x, double y)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}
	double sensitivity = 0.1;
	double dx = x - cursorLastX;
	double dy = y - cursorLastY;

	if (buttonPressed)
	{
		camera.RotateLeftRight(dx);
		camera.RotateUpDown(dy);
	}

	cursorLastX = x;
	cursorLastY = y;
}

void MouseButton(GLFWwindow *, int button, int action, int flag)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		buttonPressed = action != GLFW_RELEASE;
		glfwSetInputMode(window, GLFW_CURSOR, buttonPressed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

void KeyPress(GLFWwindow *, int key, int scanCode, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureKeyboard)
	{
		return;
	}
	switch (key)
	{
	case 'a':
	case 'A':
		camera.CameraMove(Camera::LEFT, action != GLFW_RELEASE);
		break;
	case 'w':
	case 'W':
		camera.CameraMove(Camera::FORWARD, action != GLFW_RELEASE);
		break;
	case 's':
	case 'S':
		camera.CameraMove(Camera::BACKWARD, action != GLFW_RELEASE);
		break;
	case 'd':
	case 'D':
		camera.CameraMove(Camera::RIGHT, action != GLFW_RELEASE);
		break;
	}
}
