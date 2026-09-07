// #shader vertex

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Normal = aNormal;
}


// #shader fragment

#version 330 core

in vec3 Normal;

out vec4 FragColor;

void main()
{
    vec3 n = normalize(Normal);

    // Pod - srednje siv
    if (n.y > 0.5)
    {
        FragColor = vec4(0.55, 0.55, 0.55, 1.0);
    }
    // Zadnji zid - svetao
    else if (n.z > 0.5)
    {
        FragColor = vec4(0.9, 0.9, 0.9, 1.0);
    }
    // Levi i desni zid - tamno sivi
    else
    {
        FragColor = vec4(0.3, 0.3, 0.3, 1.0);
    }
}