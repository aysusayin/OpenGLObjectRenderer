#pragma once

#include <GLFW/glfw3.h>
#include <iostream>

#define ASSERT(x) if (!(x)) exit(0)
#define GLCall(x) GLClearError();x;ASSERT(GLCheckError(#x, __FILE__, __LINE__));

void GLClearError();

bool GLCheckError(const char *functionName, const char *file, int line);