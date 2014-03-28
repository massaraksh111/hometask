#include "Settings.h"
#include "Init.h"

// ��������������� ������
#define LOAD_SHADER(name) \
	ShaderProgramCreateFromFile("data/" name ".vs", "data/" name ".fs")

void InitLights()
{
		// �������� ������������ ��������� ���������
	LightDefault(settings().lights[0], LT_DIRECTIONAL);
	settings().lights[0].position.set(3.0f, 3.0f, 3.0f, 0.0f);
	LightDefault(settings().lights[1], LT_DIRECTIONAL);
	settings().lights[1].position.set(-5.0f, 4.0f, 3.0f, 0.0f);

	// �������� �������� ��������� ���������
	LightDefault(settings().lights[2], LT_POINT);
	settings().lights[2].position.set(-2.0f, 7.5f, -4.0f, 0.0f);
}

void InitGround()
{
	// �������� ��������� � �������� ���������
	// ��������� ��� ����������� �����
	MeshCreatePlane(settings().meshes[0], vec3(0.0f, -1.6f, 0.0f), 30.0f);
	MaterialDefault(settings().materials[0]);
	settings().materials[0].texture = settings().colorTexture;
	settings().materials[0].diffuse.set(0.3f, 1.0f, 0.5f, 1.0f);
}

void InitFigures()
{
	// ����������� ���
	MeshCreateTorus(settings().meshes[1], vec3(0.0f, 1.2f, 0.0f), 2.0f);
	MaterialDefault(settings().materials[1]);
	settings().materials[1].texture = settings().colorTexture;
	settings().materials[1].diffuse.set(0.3f, 0.5f, 1.0f, 1.0f);
	settings().materials[1].specular.set(0.8f, 0.8f, 0.8f, 1.0f);
	settings().materials[1].shininess = 20.0f;

	// ����������� ���
	MeshCreateTorus(settings().meshes[2], vec3(0.0f, 1.2f, 0.0f), 1.0f);
	MaterialDefault(settings().materials[2]);
	settings().materials[2].texture = settings().colorTexture;
	settings().materials[2].diffuse.set(1.0f, 0.5f, 0.3f, 1.0f);
	settings().materials[2].specular.set(0.8f, 0.8f, 0.8f, 1.0f);
	settings().materials[2].shininess = 20.0f;
}

// ��������������� ������
#define LOAD_SHADER(name) \
	ShaderProgramCreateFromFile("data/" name ".vs", "data/" name ".fs")

bool LoadShaders()
{
	// �������� � �������� ��������� ���������
	if ((settings().programs[depth] = LOAD_SHADER("depth")) == 0
		|| (settings().programs[quad] = LOAD_SHADER("quad")) == 0
		|| (settings().programs[shadowmap] = LOAD_SHADER("shadowmap")) == 0)
	{
		return false;
	}
	return true;
}

void InitCamera(const GLWindow &window)
{
	// �������� � �������� ������
	const float aspectRatio = (float)window.width / (float)window.height;
	CameraLookAt(settings().mainCamera, vec3(-5.0f, 10.0f, 10.0f), vec3_zero, vec3_y);
	CameraPerspective(settings().mainCamera, 45.0f, aspectRatio, 0.5f, 100.0f);

	// ������ ������������ ���������� �����
	CameraLookAt(settings().lightCamera[0], settings().lights[0].position, -settings().lights[0].position, vec3_y);
	CameraOrtho(settings().lightCamera[0], -5.0f, 5.0f, -5.0f, 5.0f, -10.0f, 10.0f);
	CameraLookAt(settings().lightCamera[1], settings().lights[1].position, -settings().lights[1].position, vec3_y);
	CameraOrtho(settings().lightCamera[1], -5.0f, 5.0f, -5.0f, 5.0f, -10.0f, 10.0f);

	// ������ ��������� ��������� �����
	CameraLookAt(settings().lightCamera[2], settings().lights[2].position, -settings().lights[2].position, vec3_y);
	CameraPerspective(settings().lightCamera[2], 45.0f, aspectRatio, 0.5f, 10.0f);

	// ������ �������������� ��������������, ��� ������� �������� �������
	CameraLookAt(settings().quadCamera, vec3_zero, -vec3_z, vec3_y);
	CameraOrtho(settings().quadCamera, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
}

void InitGL(const GLWindow &window)
{
	// ������� ������
	InputShowCursor(false);

	// ������������� ������� �� ��� ����
	glViewport(0, 0, window.width, window.height);

	// ��������� OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

bool CreateFBO()
{
	// ������� FBO ��� ������� ������� � ��������
	glGenFramebuffers(1, &settings().depthFBO[1]);
	glGenFramebuffers(1, &settings().depthFBO[2]);

	for(int i = 0; i < settings().lightsCount; i++)
	{
		glGenFramebuffers(1, &settings().depthFBO[i]);

		// �������������� FBO � ����������� ��������
		glBindFramebuffer(GL_FRAMEBUFFER, settings().depthFBO[i]);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, settings().depthTextures[i].first, 0);

		// �������� �� ������������
		GLenum fboStatus;
		if ((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG_ERROR("glCheckFramebufferStatus error 0x%X\n", fboStatus);
			return false;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	return true;
}

void CreateDepth(const GLWindow &window)
{
	// �������� �������� ��� �������� �������
	for(int i = 0; i < settings().lightsCount; i++)
		settings().depthTextures[i].first = TextureCreateDepth(window.width * 2, window.height * 2);
}

void InitTextures()
{
	settings().colorTexture = TextureCreateFromTGA("data/texture.tga"); // �������� ��������
}


