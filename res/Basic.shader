#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 ourColor;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(position, 1.0f);
	ourColor = color;
}

#shader fragment
#version 330 core
in vec3 ourColor;
out vec4 color;

void main()
{
    color = vec4(ourColor, 1.0f);
}