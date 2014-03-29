#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "math/math3d.h"
#include "math/mathgl.h"
#include "OpenGL.h"
#include <string>

struct Camera
{
	vec3 position;
	vec3 rotation;
	mat4 projection;

	Camera(GLuint program, const mat4 &model);
	void lookAt(const vec3 &position, const vec3 &center, const vec3 &up);
	void perspective(float fov, float aspect, float zNear, float zFar);
	void ortho(float left, float right, 
			   float bottom, float top,
			   float zNear, float zFar);
	void rotate(float x, float y, float z);
	void move(float x, float y, float z);
	void setup(GLuint program, const mat4 &model);
	void setupLight(GLuint program, std::string prefix);
};

void CameraLookAt(Camera &camera, const vec3 &position, const vec3 &center, const vec3 &up);
void CameraPerspective(Camera &camera, float fov, float aspect, float zNear, float zFar);
void CameraOrtho(Camera &camera, float left, float right,
	float bottom, float top, float zNear, float zFar);
void CameraRotate(Camera &camera, float x, float y, float z);
void CameraMove(Camera &camera, float x, float y, float z);
void CameraSetup(GLuint program, const Camera &camera, const mat4 &model);
void CameraSetupLight(GLuint program, const Camera &camera, std::string prefix);

#endif /* CAMERA_H */
