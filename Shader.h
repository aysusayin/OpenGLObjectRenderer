#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/glad.h"

class Shader {
private:
    std::string vertexCode;
    std::string fragmentCode;

    static unsigned int CompileShader(unsigned int type, const std::string &source) {
        unsigned int id = glCreateShader(type);
        const char *src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        // Print compile errors if any
        int success;
        char infoLog[512];
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                      << "::COMPILATION_FAILED\n" << infoLog << std::endl;
            glDeleteShader(id);
            return 0;
        }
        return id;
    }

    void ParseShaderFile(const std::string& shaderPath){
        std::ifstream fileStream;
        short type = -1; // None, Vertex, Fragment
        // ensures ifstream objects can throw exceptions:
        fileStream.exceptions(std::ifstream::badbit);

        try {
            // Open files
            fileStream.open(shaderPath);
            std::stringstream vShaderStream, fShaderStream;
            std::string line;
            // Read file's buffer contents into streams
            while (getline(fileStream, line)) {
                if (line.find("#shader") != std::string::npos) {
                    if (line.find("vertex") != std::string::npos) {
                        type = 0;
                    } else if (line.find("fragment") != std::string::npos) {
                        type = 1;
                    }
                } else {
                    if (type == 0) {
                        vShaderStream << line << "\n";
                    } else if (type == 1) {
                        fShaderStream << line << "\n";
                    }
                }
            }
            // close file handlers
            fileStream.close();
            // Convert stream into string
            this->vertexCode = vShaderStream.str();
            this->fragmentCode = fShaderStream.str();

        } catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
    }

public:
    unsigned int Program;

    // Constructor generates the shader on the fly
    Shader(const std::string& shaderPath) {
        // 1. Retrieve the vertex/fragment source code from filePath
        ParseShaderFile(shaderPath);

        // 2. Compile shaders
        unsigned int vertex, fragment;
        vertex = CompileShader(GL_VERTEX_SHADER, vertexCode);
        fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

        // Shader Program
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);

        // Print linking errors if any
        int success;
        char infoLog[512];
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (success == GL_FALSE) {
            glGetProgramInfoLog(this->Program, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Uses the current shader
    void Use() {
        glUseProgram(this->Program);
    }

    void Delete() {
        glDeleteProgram(this->Program);
    }

};

#endif
