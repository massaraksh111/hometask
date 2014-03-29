#include "Light.h"
using std::string;

void LightDefault(Light &light, LightType type)
{
	light.type = type;

	light.ambient.set(0.0f, 0.0f, 0.0f, 1.0f);
	light.diffuse.set(1.0f, 1.0f, 10.0f, 1.0f);
	light.specular.set(1.0f, 1.0f, 1.0f, 1.0f);
	light.position.set(0.0f, 0.0f, 1.0f, 0.0f);
	light.attenuation.set(1.0f, 0.0f, 0.0f);
	light.spotDirection.set(0.0f, 0.0f, -1.0f);
	light.spotExponent = 0.0f;
	light.spotCosCutoff = -1.0f;
}

void LightSetup(GLuint program, const Light &light, const string& prefixLight)
{
	glUniform4fv(glGetUniformLocation(program, (prefixLight + ".ambient").c_str()), 1, light.ambient.v);
	glUniform4fv(glGetUniformLocation(program, (prefixLight + ".diffuse").c_str()), 1, light.diffuse.v);
	glUniform4fv(glGetUniformLocation(program, (prefixLight + ".specular").c_str()), 1, light.specular.v);
	glUniform4fv(glGetUniformLocation(program, (prefixLight + ".position").c_str()), 1, light.position.v);

	glUniform3fv(glGetUniformLocation(program, (prefixLight + ".attenuation").c_str()), 1, light.attenuation.v);
	glUniform3fv(glGetUniformLocation(program, (prefixLight + ".spotDirection").c_str()), 1, light.spotDirection.v);

	glUniform1fv(glGetUniformLocation(program, (prefixLight + ".spotExponent").c_str()), 1, &light.spotExponent);
	glUniform1fv(glGetUniformLocation(program, (prefixLight + ".spotCosCutoff").c_str()), 1, &light.spotCosCutoff);
}
