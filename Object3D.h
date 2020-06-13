#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "GeometricDefinitions.h"
#include "glad/glad.h"
#include "GLErrorCheck.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Object3D {
private:
    static int factorial(int n);

    static float calculateBinomialCoeff(int n, int i);

    static float calculateBernsteinPolynomial(int n, int i, float u);

    Vertex calculateBezierVertices(float u, float v, int index);

public:
    unsigned int VBO;
    unsigned int BezierEBO;
    unsigned int VAO;
    int vertexCount;
    int bezierSurfaceCount;
    glm::mat4 modelMatrix;
    std::vector<Vertex> vertexList;
    std::vector<BezierSurface> bezierList;

    Object3D();

    ~Object3D();

    void CreateObject();

    void SetVertices();
};
