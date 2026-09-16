//#shader vertex

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    Normal =
        mat3(transpose(inverse(model))) * aNormal;

    gl_Position =
        projection * view * vec4(FragPos, 1.0);
}


//#shader fragment

#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;

uniform vec3 viewPosition;

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);

    // Directional light
    vec3 directionalDir =
        normalize(-directionalLight.direction);

    float directionalStrength =
        max(dot(normal, directionalDir), 0.0);

    vec3 directionalReflectDir =
        reflect(-directionalDir, normal);

    float directionalSpecularStrength =
        pow(
            max(dot(viewDir, directionalReflectDir), 0.0),
            material.shininess
        );

    vec3 directionalAmbient =
        directionalLight.ambient *
        material.ambient;

    vec3 directionalDiffuse =
        directionalLight.diffuse *
        directionalStrength *
        material.diffuse;

    vec3 directionalSpecular =
        directionalLight.specular *
        directionalSpecularStrength *
        material.specular;

    // Point light
    vec3 pointDir =
        normalize(pointLight.position - FragPos);

    float pointStrength =
        max(dot(normal, pointDir), 0.0);

    vec3 pointReflectDir =
        reflect(-pointDir, normal);

    float pointSpecularStrength =
        pow(
            max(dot(viewDir, pointReflectDir), 0.0),
            material.shininess
        );

    float distance =
        length(pointLight.position - FragPos);

    float attenuation =
        1.0 /
        (
            pointLight.constant +
            pointLight.linear * distance +
            pointLight.quadratic * distance * distance
        );

    vec3 pointAmbient =
        pointLight.ambient *
        material.ambient *
        attenuation;

    vec3 pointDiffuse =
        pointLight.diffuse *
        pointStrength *
        material.diffuse *
        attenuation;

    vec3 pointSpecular =
        pointLight.specular *
        pointSpecularStrength *
        material.specular *
        attenuation;

    vec3 result =
        directionalAmbient +
        directionalDiffuse +
        directionalSpecular +
        pointAmbient +
        pointDiffuse +
        pointSpecular;

    FragColor = vec4(result, 1.0);
}