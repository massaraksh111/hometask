// текстурный самплер карты глубины
uniform sampler2DShadow depthTextureDirectional1;
uniform sampler2DShadow depthTextureDirectional2;
uniform sampler2DShadow depthTexturePoint;

// параметры точеченого источника освещения
uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} directionalLight1;

uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} directionalLight2;

uniform struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
} pointLight;

// параметры материала
uniform struct Material
{
	sampler2D texture;

	vec4  ambient;
	vec4  diffuse;
	vec4  specular;
	vec4  emission;
	float shininess;
} material;

// параметры полученные из вершинного шейдера
in Vertex
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

layout(location = FRAG_OUTPUT0) out vec4 color;


void main(void)
{
	// нормализуем полученные данные для коррекции интерполяции
	vec3 normal   = normalize(Vert.normal);
	vec3 directionalLight1Dir = normalize(Vert.directionalLight1Dir);
	vec3 directionalLight2Dir = normalize(Vert.directionalLight2Dir);
	vec3 pointLightDir = normalize(Vert.pointLightDir);
	vec3 viewDir  = normalize(Vert.viewDir);

	// коэффициент затенения
	float shadow = textureProj(depthTextureDirectional1, Vert.smcoordDirectionalLight1);
	float shadow2 = textureProj(depthTextureDirectional2, Vert.smcoordDirectionalLight2);
	float shadow3 = textureProj(depthTexturePoint, Vert.smcoordPointLight);

	// добавим собственное свечение материала
	color = material.emission;

	// добавим фоновое освещение
	color += material.ambient * directionalLight1.ambient;
	color += material.ambient * directionalLight2.ambient;
	color += material.ambient * pointLight.ambient;

	// добавим рассеянный свет
	float NdotL;
    NdotL = max(dot(normal, directionalLight1Dir), 0.0);
	color += material.diffuse * directionalLight1.diffuse * NdotL;
    NdotL = max(dot(normal, directionalLight2Dir), 0.0);
	color += material.diffuse * directionalLight2.diffuse * NdotL;
    NdotL = max(dot(normal, pointLightDir), 0.0);
	color += material.diffuse * pointLight.diffuse * NdotL;

	// добавим отраженный свет
	float RdotVpow;
    RdotVpow = max(pow(dot(reflect(-directionalLight1Dir, normal), viewDir), material.shininess), 0.0);
	color += material.specular * directionalLight1.specular * RdotVpow;
    RdotVpow = max(pow(dot(reflect(-directionalLight2Dir, normal), viewDir), material.shininess), 0.0);
	color += material.specular * directionalLight2.specular * RdotVpow;
    RdotVpow = max(pow(dot(reflect(-pointLightDir, normal), viewDir), material.shininess), 0.0);
	color += material.specular * pointLight.specular * RdotVpow;

	// вычислим итоговый цвет пикселя на экране с учетом текстуры
	color *= texture(material.texture, Vert.texcoord) * (shadow + shadow2 + shadow3) / 3;
}
