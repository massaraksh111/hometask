layout(location = VERT_POSITION) in vec3 position;
layout(location = VERT_TEXCOORD) in vec2 texcoord;
layout(location = VERT_NORMAL)   in vec3 normal;

// параметры преобразований
uniform struct Transform
{
	mat4 model;
	mat4 viewProjection;
	mat4 directionalLight1;
	mat4 directionalLight2;
	mat4 pointLight;
	mat3 normal;
	vec3 viewPosition;
} transform;

// параметры точеченого источника освещения
uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} directionalLight1;

// параметры точеченого источника освещения
uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} directionalLight2;

// параметры точеченого источника освещения
uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} pointLight;

// параметры для фрагментного шейдера
out Vertex
{
	vec2 texcoord;
	vec4 smcoordDirectionalLight1;
	vec4 smcoordDirectionalLight2;
	vec4 smcoordPointLight;
	vec3 normal;
	vec3 directionalLight1Dir;
	vec3 directionalLight2Dir;
	vec3 pointLightDir;
	vec3 viewDir;
} Vert;

void main(void)
{
	// переведем координаты вершины в мировую систему координат
	vec4 vertex   = transform.model * vec4(position, 1.0);

	// передадим в фрагментный шейдер некоторые параметры
	// вычисляем текстурные координаты вершины на карте глубины
	Vert.smcoordDirectionalLight1 = transform.directionalLight1 * vertex;
	Vert.smcoordDirectionalLight2 = transform.directionalLight2 * vertex;
	Vert.smcoordPointLight = transform.pointLight * vertex;
	// передаем текстурные координаты
	Vert.texcoord = texcoord;
	// передаем нормаль в мировой системе координат
	Vert.normal   = transform.normal * normal;
	// передаем направление на источник освещения
	Vert.directionalLight1Dir = vec3(directionalLight1.position);
	Vert.directionalLight2Dir = vec3(directionalLight2.position);
	Vert.pointLightDir = vec3(pointLight.position);
	// передаем направление от вершины к наблюдателю в мировой системе координат
	Vert.viewDir  = transform.viewPosition - vec3(vertex);

	// переводим координаты вершины в однородные
	gl_Position = transform.viewProjection * vertex;
}
