#pragma once

#include <string>

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
#include <utility>

enum ProgramIndex
{
	depth,
	quad,
	shadowmap
};

struct Settings
{
	enum {
		One = 1,
		Two = 2,
		torusCount = 2,
		lightsCount = 3,
		meshCount = 3
	};

	float torusRotation[torusCount];
	GLuint programs[lightsCount];
	GLuint colorTexture;
	std::pair<GLuint, std::string> depthTextures[lightsCount];
	GLuint depthFBO[lightsCount];

	int cursorPos[Two];
	int rotateDelta[Two];
	int moveDelta[Two];

	Mesh meshes[lightsCount];
	Mesh quadMesh;
	Material materials[meshCount];
	Material quadMaterial;

	Light lights[lightsCount];
	std::string lightNames[lightsCount];
	Camera mainCamera, quadCamera;
	Camera lightCamera[lightsCount];


	bool doRenderQuad;
	bool lightsEnabled[lightsCount];

	Settings()
	{
		colorTexture = 0;
		for(int i = 0; i < lightsCount; i++)
		{
			programs[i] = 0;
			depthFBO[i] = 0;
			depthTextures[i].first = 0;
			lightsEnabled[i] = true;
		}
		for(int i = 0; i < Two; i++)
		{
			cursorPos[i] = 0;
			rotateDelta[i] = 0;
			moveDelta[i] = 0;
			torusRotation[i] = 0.0f;
		}

		lightNames[0] = "directionalLight1";
		lightNames[1] = "directionalLight2";
		lightNames[2] = "pointLight";

		depthTextures[0].second = "depthTextureDirectional1";
		depthTextures[1].second = "depthTextureDirectional2";
		depthTextures[2].second = "depthTexturePoint";
		doRenderQuad = false;
	}
};

inline Settings& settings()
{
	static Settings settings;

	return settings;
}