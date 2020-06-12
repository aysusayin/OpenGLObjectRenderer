#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include "GeometricDefinitions.h"
#include "glad/glad.h"

class Object3D {
private:
    static int factorial(int n);

    static float calculateBinomialCoeff(int n, int i);

    static float calculateBernsteinPolynomial(int n, int i, float u);

    Vertex calculateBezierVertices(float u, float v, unsigned int (*controlPoints)[ORDER + 1]);

public:
    GLuint VBO;
    GLuint EBO;
    GLuint BezierEBO;
    GLuint VAO;
    GLuint texture;
    int vertexCount;
    int triangleCount;
    int bezierCount;
    glm::mat4 modelMatrix;
    std::vector<Vertex> vlist;
    std::vector<Triangle> tlist;
    std::vector<BezierSurface> blist;

    Object3D();

    ~Object3D();

    void CreateObject();
};
