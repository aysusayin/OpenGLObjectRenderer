#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "GeometricDefinitions.h"
#include "glad/glad.h"
#include "Helper/GLErrorCheck.h"
#include "Shader.h"

class Object3D {
protected:
    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;
    glm::mat4 modelMatrix;
    int vertexCount;
    int elementCount;
    unsigned int *elementArray;
    float *vertexArray;

    void CreateObject();

    virtual void SetVertexList() = 0;

    virtual void SetElementList() = 0;

public:
    void DrawObject(Shader *shader, glm::mat4 *transformationMatrix, glm::mat4 *animationMatrix = nullptr);

    ~Object3D();

};

class UtahTeapot : public Object3D {
protected:
    static int factorial(int n);

    static float calculateBinomialCoeff(int n, int i);

    static float calculateBernsteinPolynomial(int n, int i, float u);

    Vertex calculateBezierVertices(float u, float v, int index);

    void GenerateBezierVertexList(Vertex *bezierVertexList);

    void GenerateBezierElementArray();

    void SetVertices();

    void SetBezierPatches();

    void SetVertexList() override;

    void SetElementList() override;

public:
    int bezierSurfaceCount;
    std::vector<Vertex> allVertexList;
    std::vector<BezierSurface> bezierSurfacesList;

    UtahTeapot(glm::mat4 modelMatrix = glm::mat4(1));
};