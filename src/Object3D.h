#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "GeometricDefinitions.h"
#include "../include/glad/glad.h"
#include "GLErrorCheck.h"
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <../include/rply/rply.h>
#include "PlyFileParser.h"

class Object3D {
protected:
    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;
    unsigned int texture;
    bool isTextureSet;
    glm::mat4 modelMatrix;
    int vertexCount;
    int elementCount;
    std::vector<Vertex> vertexList;
    std::vector<Triangle> triangleList;
    unsigned int *elementArray;
    float *vertexArray;

    std::string texturePath;

    void SetTexture();

    virtual void SetVertexArray();

    virtual void SetElementArray();


public:
    void BindBuffers();

    void CreateFromPlyFile(std::string filePath);

    void DrawObject(Shader *shader, glm::mat4 *projectionMatrix,glm::mat4 *viewMatrix, glm::mat4 *animationMatrix);

    void SetTexture(std::string filePath);

    explicit Object3D(glm::mat4 modelMatrix = glm::mat4(1));

    ~Object3D();

};

class UtahTeapot : public Object3D {
protected:
    static int factorial(int n);

    static float CalculateBinomialCoeff(int n, int i);

    static float CalculateBernsteinPolynomial(int n, int i, float u);

    Vertex CalculateBezierVertices(float u, float v, int index);

    void GenerateBezierVertexList(Vertex *bezierVertexList);

    void GenerateBezierElementArray();

    void SetVertices();

    void SetBezierPatches();

    void SetVertexArray() override;

    void SetElementArray() override;

public:
    int bezierSurfaceCount;
    std::vector<Vertex> allVertexList;
    std::vector<BezierSurface> bezierSurfacesList;

    explicit UtahTeapot(std::string texturePath = "res/textures/default.png", glm::mat4 modelMatrix = glm::mat4(1));
};