//#shader vertex

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    Normal =
        mat3(transpose(inverse(model))) * aNormal;

    TexCoords = aTexCoords;

    gl_Position =
        projection * view * vec4(FragPos, 1.0);
}


//#shader fragment

#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 objectColor;

uniform sampler2D texture_diffuse1;
uniform bool hasDiffuseTexture;

uniform vec3 directionalDirection;
uniform vec3 directionalColor;

uniform vec3 pointPosition;
uniform vec3 pointColor;

void main()
{
    vec3 normal = normalize(Normal);

    vec3 baseColor;

    if (hasDiffuseTexture)
    {
        baseColor = texture(texture_diffuse1, TexCoords).rgb;
    }
    else
    {
        baseColor = objectColor;
    }

    // Ambient
    vec3 ambient =
        0.25 * baseColor;

    // Directional light
    vec3 directionalDir =
        normalize(-directionalDirection);

    float directionalStrength =
        max(dot(normal, directionalDir), 0.0);

    vec3 directional =
        directionalStrength *
        directionalColor *
        baseColor;

    // Point light
    vec3 pointDir =
        normalize(pointPosition - FragPos);

    float pointStrength =
        max(dot(normal, pointDir), 0.0);

    float distance =
        length(pointPosition - FragPos);

    float attenuation =
        1.0 /
        (
            1.0 +
            0.09 * distance +
            0.032 * distance * distance
        );

    vec3 point =
        pointStrength *
        pointColor *
        baseColor *
        attenuation;

    vec3 result =
        ambient +
        directional +
        point;

    FragColor =
        vec4(result, 1.0);
}