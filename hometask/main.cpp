#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Init.h"
#include "Settings.h"

// инициализаця OpenGL
bool GLWindowInit(const GLWindow &window)
{
	InitGL(window);
	if(!LoadShaders())
		return false;

	InitLights();
	InitTextures();
	CreateDepth(window);
	InitGround();
	InitFigures();
	InitCamera(window);
	if(!CreateFBO())
		return false;
	
	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow &window)
{
	(void)window;

	TextureDestroy(settings().colorTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	for (uint32_t i = 0; i < settings().meshCount; ++i)
		MeshDestroy(settings().meshes[i]);

	for(int i = 0; i < settings().lightsCount; i++)
	{
		ShaderProgramDestroy(settings().programs[i]);
		glDeleteFramebuffers(1, &settings().depthFBO[i]);
		TextureDestroy(settings().depthTextures[i].first);
	}
	
	InputShowCursor(true);
}

void RenderScene(GLuint program, const Camera &camera)
{
	// делаем шейдерную программу активной
	ShaderProgramBind(program);

	for(int i = 0; i < settings().lightsCount; i++)
	{
		LightSetup(program, settings().lights[i], settings().lightNames[i]);
		CameraSetupLight(program, settings().lightCamera[i], settings().lightNames[i]);
		TextureSetup(program, i+1, settings().depthTextures[i].second.c_str(), settings().depthTextures[i].first);
		if(settings().lightsEnabled[i])
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	}

	for (uint32_t i = 0; i < settings().meshCount; ++i)
	{
		CameraSetup(program, camera, MeshGetModelMatrix(settings().meshes[i]));
		MaterialSetup(program, settings().materials[i]);
		MeshRender(settings().meshes[i]);
	}
}

void RenderQuad(GLuint program, const Camera &camera)
{
	// делаем шейдерную программу активной
	ShaderProgramBind(program);

	CameraSetup(program, camera, MeshGetModelMatrix(settings().quadMesh));
	MaterialSetup(program, settings().quadMaterial);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	MeshRender(settings().quadMesh);
}

// функция рендера
void GLWindowRender(const GLWindow &window)
{
	for(int i = 0; i < settings().lightsCount; i++) 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, settings().depthFBO[i]);
		glViewport(0, 0, window.width * 2, window.height * 2);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		RenderScene(settings().programs[depth], settings().lightCamera[i]);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window.width, window.height);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);

	if (settings().doRenderQuad)
		RenderQuad(settings().programs[quad], settings().quadCamera);
	else
		RenderScene(settings().programs[shadowmap], settings().mainCamera);

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

// функция обновления
void GLWindowUpdate(const GLWindow &window, double deltaTime)
{
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	(void)window;

	// зададим углы поворота куба с учетом времени
	float angles[] = {30.0f, 15.0f};
	for(int i = 0; i < settings().torusCount; i++)
		if ((settings().torusRotation[i] += angles[i] * (float)deltaTime) > 360.0f)
			settings().torusRotation[i] -= 360.0f;

	// зададим матрицу вращения куба
	settings().meshes[1].rotation = GLFromEuler(settings().torusRotation[0], settings().torusRotation[1], settings().torusRotation[2]);
	settings().meshes[2].rotation = GLFromEuler(-settings().torusRotation[0], settings().torusRotation[1], -settings().torusRotation[2]);

	// вращаем камеру
	CameraRotate(settings().mainCamera, (float)deltaTime * settings().rotateDelta[1], (float)deltaTime * settings().rotateDelta[0], 0.0f);
	// двигаем камеру
	CameraMove(settings().mainCamera, (float)deltaTime * settings().moveDelta[0], 0.0f, (float)deltaTime * settings().moveDelta[1]);

	settings().rotateDelta[0] = settings().rotateDelta[1] = 0;
	settings().moveDelta[0] = settings().moveDelta[1] = 0;

	OPENGL_CHECK_FOR_ERRORS();
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow &window)
{
	// центр окна
	int32_t xCenter = window.width / 2, yCenter = window.height / 2;

	// выход из приложения по кнопке Esc
	if (InputIsKeyPressed(VK_ESCAPE))
		GLWindowDestroy();

	if (InputIsKeyPressed(VK_SPACE))
		settings().doRenderQuad = !settings().doRenderQuad;

	for(int i = 0; i < settings().lightsCount; i++)
		if (InputIsKeyPressed(VK_F1 + i))
			settings().lightsEnabled[i] = !settings().lightsEnabled[i];

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (InputIsKeyDown(VK_MENU) && InputIsKeyPressed(VK_RETURN))
	{
		GLWindowSetSize(window.width, window.height, !window.fullScreen);
		//InputShowCursor(window.fullScreen ? false : true);
	}

	settings().moveDelta[0] = 10 * ((int)InputIsKeyDown('D') - (int)InputIsKeyDown('A'));
	settings().moveDelta[1] = 10 * ((int)InputIsKeyDown('S') - (int)InputIsKeyDown('W'));

	InputGetCursorPos(settings().cursorPos, settings().cursorPos + 1);
	settings().rotateDelta[0] += settings().cursorPos[0] - xCenter;
	settings().rotateDelta[1] += settings().cursorPos[1] - yCenter;
	InputSetCursorPos(xCenter, yCenter);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int result;

	LoggerCreate("hometask.log");

	if (!GLWindowCreate("hometask", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
