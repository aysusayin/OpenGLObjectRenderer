#include "Object3D.h"

Object3D::Object3D() { this->modelMatrix = glm::mat4(1.0f); }

Object3D::~Object3D() {}

int Object3D::factorial(int n) {
    return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

float Object3D::calculateBinomialCoeff(int n, int i) {
    return factorial(n) * 1.0f / (factorial(i) * factorial(n - i));
}

float Object3D::calculateBernsteinPolynomial(int n, int i, float u) {
    return calculateBinomialCoeff(n, i) * (float) pow(u, i) * (float) pow(1 - u, n - i);
}

Vertex Object3D::calculateBezierVertices(float u, float v, unsigned int (*controlPoints)[ORDER + 1]) {
    Vertex result;
    for (int i = 0; i < ORDER + 1; i++) {
        for (int j = 0; j < ORDER + 1; j++) {
            result = result +
                     calculateBernsteinPolynomial(ORDER + 1, i, u) * calculateBernsteinPolynomial(ORDER + 1, j, v) *
                     vlist[controlPoints[i][j]];
        }
    }
    return result;
}

void Object3D::CreateObject() {
    bezierCount = 1;
    triangleCount = 2;
    vertexCount = 20;
    vlist.resize(vertexCount);
    tlist.resize(triangleCount);
    vlist[0].set(-0.5, -0.5, 0.0, 1.0, 0.0, 0.0);
    vlist[1].set(-0.5, 0.5, 0.0, 0.0, 1.0, 0.0);
    vlist[2].set(0.5, 0.5, 0.0, 0.0, 0.0, 1.0);
    vlist[3].set(0.5, -0.5, 0.0, 1.0, 0.0, 1.0);

    vlist[4].set(1.4, 0.0, 2.4, 1.0, 0.0, 0.0);
    vlist[5].set(1.4, -0.784, 2.4, 0.0, 1.0, 0.0);
    vlist[6].set(0.784, -1.4, 2.4, 0.0, 1.0, 0.0);
    vlist[7].set(0.0, -1.4, 2.4, 0.0, 1.0, 0.0);

    vlist[8].set(1.3375, 0.0, 2.53125, 1.0, 0.0, 0.0);
    vlist[9].set(1.3375, -0.749, 2.53125, 0.0, 1.0, 0.0);
    vlist[10].set(0.749, -1.3375, 2.53125, 0.0, 0.0, 1.0);
    vlist[11].set(0.0, -1.3375, 2.53125, 1.0, 0.0, 1.0);

    vlist[12].set(1.4375, 0.0, 2.53125, 1.0, 0.0, 0.0);
    vlist[13].set(1.4375, -0.805, 2.53125, 0.0, 1.0, 0.0);
    vlist[14].set(0.805, -1.4375, 2.53125, 0.0, 0.0, 1.0);
    vlist[15].set(0.0, -1.4375, 2.53125, 1.0, 0.0, 1.0);

    vlist[16].set(1.5, 0.0, 2.4, 1.0, 0.0, 0.0);
    vlist[17].set(1.5, -0.84, 2.4, 0.0, 1.0, 0.0);
    vlist[18].set(0.84, -1.5, 2.4, 0.0, 0.0, 1.0);
    vlist[19].set(0.0, -1.5, 2.4, 1.0, 0.0, 1.0);

    unsigned int indices[] = {
            0, 1, 2
    };
    tlist[0].nverts = 3;
    tlist[0].verts = indices;
    unsigned int indices2[] = {
            0, 2, 3
    };
    tlist[1].nverts = 3;
    tlist[1].verts = indices2;

    unsigned int bezierIndices[][ORDER + 1] = {{4,  5,  6,  7},
                                               {8,  9,  10, 11},
                                               {12, 13, 14, 15},
                                               {16, 17, 18, 19}};
    blist.resize(bezierCount);
    blist[0].controlPoints = bezierIndices;
    Vertex bezier_vertices[RESU * RESV];

    for (int ru = 0; ru <= RESU - 1; ru++) {
        float u = 1.0 * ru / (RESU - 1);
        for (int rv = 0; rv <= RESV - 1; rv++) {
            float v = 1.0 * rv / (RESV - 1);
            bezier_vertices[ru * RESV + rv] = calculateBezierVertices(u, v, blist[0].controlPoints);
        }
    }

    int bezierSize = (RESU - 1) * (RESV - 1) * 2 * 3;
    unsigned int bezier_elements[bezierSize];

    // Elements
    int n = 0;
    for (int ru = 0; ru < RESU - 1; ru++)
        for (int rv = 0; rv < RESV - 1; rv++) {
            // 1 square ABCD = 2 triangles ABC + CDA
            // ABC
            bezier_elements[n] = ru * RESV + rv;
            n++;
            bezier_elements[n] = ru * RESV + (rv + 1);
            n++;
            bezier_elements[n] = (ru + 1) * RESV + (rv + 1);
            n++;
            // CDA
            bezier_elements[n] = (ru + 1) * RESV + (rv + 1);
            n++;
            bezier_elements[n] = (ru + 1) * RESV + rv;
            n++;
            bezier_elements[n] = ru * RESV + rv;
            n++;
        }


    // Bind the vertex and index buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &BezierEBO);
    // Bind our Vertex Array Object first, then bind and set our buffers and
    // pointers.
    glBindVertexArray(VAO);


    // Convert our vertex list into a continuous array, copy the vertices into the
    // vertex buffer.
    int attributeCount = 6; // x y z r g b
    /* float *vertexData = new float[vertexCount * attributeCount];
    for (int i = 0; i < vertexCount; i++)
        vlist[i].getAsArray(&vertexData[i * attributeCount]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, attributeCount * vertexCount * sizeof(float), vertexData, GL_STATIC_DRAW);
*/
   float *vertexData = new float[RESU * RESV * attributeCount];
    for (int i = 0; i < RESU * RESV; i++)
        bezier_vertices[i].getAsArray(&vertexData[i * attributeCount]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, attributeCount * RESU * RESV * sizeof(float), vertexData, GL_STATIC_DRAW);

    // Copy the index data found in the list of triangles into the element array
    // buffer (index array) We are using a triangles, so we need triangleCount * 3
    // indices.

    /*unsigned int* indexData = new unsigned int[triangleCount * 3];
    for (int i = 0; i < triangleCount; i++)
        memcpy(&indexData[i * 3], tlist[i].verts, sizeof(unsigned int) * 3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * triangleCount, indexData, GL_STATIC_DRAW);
    */
    /*
    unsigned int *indexData = new unsigned int[bezierCount * 16];
    for (int i = 0; i < bezierCount; i++)
        memcpy(&indexData[i * 16], blist[i].controlPoints, sizeof(unsigned int) * 16);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BezierEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 16 * bezierCount, indexData, GL_STATIC_DRAW);
     */

    //unsigned int *indexData = new unsigned int[bezierCount * bezierSize];
    //for (int i = 0; i < bezierCount; i++)
        //memcpy(&indexData[i * bezierSize], bezier_elements, sizeof(unsigned int) * bezierSize);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BezierEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bezierSize * bezierCount, bezier_elements, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Unbind VAO
    glBindVertexArray(0);

    // Delete temporary buffers
    delete[] vertexData;
    //delete[] indexData;
}
