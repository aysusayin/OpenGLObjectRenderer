#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoords;

out vec3 ourColor;
out vec2 ourTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 animation;

void main()
{
	gl_Position = projection * view * model * animation * vec4(position, 1.0f);
	ourColor = color + position[0]*vec3(0.2f,0.2f,0.2f);
	ourTexCoords = texCoords;
}

#shader fragment
#version 330 core

in vec3 ourColor;
in vec2 ourTexCoords;
out vec4 color;

uniform sampler2D u_texture;

void main()
{
    vec4 textureOut = texture(u_texture, ourTexCoords);
    color = textureOut * vec4(ourColor, 1.0f);
}