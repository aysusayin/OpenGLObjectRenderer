#include "GLErrorCheck.h"

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLCheckError(const char *functionName, const char *file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL ERROR] (" << error << "): " << functionName << " in file " << file << " on line " << line
                  << std::endl;
        return false;
    }
    return true;
}

