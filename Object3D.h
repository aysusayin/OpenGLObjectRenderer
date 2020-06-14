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
    static int factorial(int n);

    static float calculateBinomialCoeff(int n, int i);

    static float calculateBernsteinPolynomial(int n, int i, float u);

    Vertex calculateBezierVertices(float u, float v, int index);

    void GenerateBezierVertexList(Vertex *bezierVertexList);

    void GenerateBezierElementArray();

    void SetVertices();

    void SetBezierPatches();

public:
    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;
    int vertexCount;
    int bezierSurfaceCount;
    glm::mat4 modelMatrix;
    std::vector<Vertex> vertexList;
    std::vector<BezierSurface> bezierList;
    // 2 because a patch is 2 triangles and each has 3 vertices
    unsigned int bezierElementArray[PATCH_NUM * (RESU - 1) * (RESV - 1) * 2 * 3];

    Object3D();

    ~Object3D();

    void CreateObject();

    void DrawObject(Shader *shader, glm::mat4 *vp);
};
