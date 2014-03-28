#pragma once

#include "GLWindow.h"

void InitLights();
void InitGround();
void InitFigures();
bool LoadShaders();
void InitCamera(const GLWindow &window);
void InitGL(const GLWindow &window);
bool CreateFBO();
void CreateDepth(const GLWindow &window);
void InitTextures();
