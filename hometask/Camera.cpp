#include "Camera.h"
using std::string;
// матрица сдвига текстурных координат
static const mat4 bias(
	0.5f, 0.0f, 0.0f, 0.5f,
	0.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 0.0f, 0.5f, 0.5f,
	0.0f, 0.0f, 0.0f, 1.0f
);

void Camera::create(float x, float y, float z)
{
	// зададим позицию камеры и еденичную матрицу проекции
	position   = vec3(x, y, z);
	rotation   = vec3_zero;
	projection = mat4_identity;
}

void Camera::lookAt(const vec3 &position, const vec3 &center, const vec3 &up)
{
	rotation = GLToEuler(GLLookAt(position, center, up));
	position = position;
}

void Camera::perspective(float fov, float aspect, float zNear, float zFar)
{
	// расчет перспективной матрицы проекции
	projection = GLPerspective(fov, aspect, zNear, zFar);
}

void Camera::ortho(float left, float right,
	float bottom, float top, float zNear, float zFar)
{
	// расчет ортографической матрицы проекции
	projection = GLOrthographic(left, right, bottom, top, zNear, zFar);
}

void Camera::rotate(float x, float y, float z)
{
	// увеличение углов вращения камеры
	rotation += vec3(x, y, z);
}

void Camera::move(float x, float y, float z)
{
	// сначала нам надо перевести вектор направления в локальные координаты камеры
	// для этого нам надо инвертировать матрицу вращения камеры и умножить ее на вектор
	// однако для матрицы вращения транспонирование дает тот же результат что инвертирование
	vec3 move = transpose(mat3(GLFromEuler(rotation))) * vec3(x, y, z);

	position += move;
}

void Camera::setup(GLuint program, const mat4 &model)
{
	// расчитаем необходимые матрицы
	mat4 view           = GLFromEuler(rotation) * GLTranslation(-position);
	mat4 viewProjection = projection * view;
	mat3 normal         = transpose(mat3(inverse(model)));

	mat4 modelViewProjection = viewProjection * model;

	// передаем матрицы в шейдерную программу
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.model"),          1, GL_TRUE, model.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "transform.viewProjection"), 1, GL_TRUE, viewProjection.m);
	glUniformMatrix3fv(glGetUniformLocation(program, "transform.normal"),         1, GL_TRUE, normal.m);

	glUniformMatrix4fv(glGetUniformLocation(program, "transform.modelViewProjection"),
		1, GL_TRUE, modelViewProjection.m);

	// передаем позицию наблюдателя (камеры) в шейдерную программу
	glUniform3fv(glGetUniformLocation(program, "transform.viewPosition"), 1, position.v);
}

void Camera::setupLight(GLuint program, const std::string prefix)
{
	// расчитаем необходимые матрицы
	mat4 view           = GLFromEuler(rotation) * GLTranslation(-position);
	mat4 viewProjection = bias * Camera::projection * view;
	
	// передадим матрицу источника освещения в шейдерную программу
	glUniformMatrix4fv(glGetUniformLocation(program,  prefix.insert(0, "transform.").c_str()), 1, GL_TRUE, viewProjection.m);
}
