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

Vertex Object3D::calculateBezierVertices(float u, float v, int index) {
    Vertex result;
    for (int i = 0; i < ORDER + 1; i++) {
        for (int j = 0; j < ORDER + 1; j++) {
            result = result +
                     calculateBernsteinPolynomial(ORDER + 1, i, u) * calculateBernsteinPolynomial(ORDER + 1, j, v) *
                     vertexList[bezierList[index].controlPoints[i][j]];
        }
    }
    result.r = 1.0 * index / bezierSurfaceCount;
    result.g = 1.0 * index / bezierSurfaceCount;
    result.b = 0.8;
    return result;
}

void Object3D::CreateObject() {
    bezierSurfaceCount = 1;
    SetVertices();

    unsigned int bezierIndices[][ORDER + 1] = {{40, 47, 48, 13},
                                               {72, 79, 80, 49},
                                               {75, 81, 82, 53},
                                               {78, 83, 84, 57}};
    bezierList.resize(bezierSurfaceCount);
    bezierList[0].controlPoints = bezierIndices;
    Vertex bezierVertexList[RESU * RESV];

    for (int i = 0; i < bezierSurfaceCount; i++) {
        for (int ru = 0; ru <= RESU - 1; ru++) {
            float u = 1.0 * ru / (RESU - 1);
            for (int rv = 0; rv <= RESV - 1; rv++) {
                float v = 1.0 * rv / (RESV - 1);
                bezierVertexList[i * RESU * RESV + ru * RESV + rv] = calculateBezierVertices(u, v, i);
            }
        }
    }
    int bezierSize = bezierSurfaceCount * (RESU - 1) * (RESV - 1) * 2 * 3;
    unsigned int bezierElementArray[bezierSize];

    // Elements
    int n = 0;
    for (int i = 0; i < bezierSurfaceCount; i++) {
        for (int ru = 0; ru < RESU - 1; ru++) {
            for (int rv = 0; rv < RESV - 1; rv++) {
                // 1 square ABCD = 2 triangles ABC + CDA
                // ABC
                bezierElementArray[n] = i * RESU * RESV + ru * RESV + rv;
                n++;
                bezierElementArray[n] = i * RESU * RESV + ru * RESV + (rv + 1);
                n++;
                bezierElementArray[n] = i * RESU * RESV + (ru + 1) * RESV + (rv + 1);
                n++;
                // CDA
                bezierElementArray[n] = i * RESU * RESV + (ru + 1) * RESV + (rv + 1);
                n++;
                bezierElementArray[n] = i * RESU * RESV + (ru + 1) * RESV + rv;
                n++;
                bezierElementArray[n] = i * RESU * RESV + ru * RESV + rv;
                n++;
            }
        }
    }
    // Bind the vertex and index buffers
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glGenBuffers(1, &BezierEBO));

    // Bind our Vertex Array Object first, then bind and set our buffers and
    // pointers.
    GLCall(glBindVertexArray(VAO));

    // Convert our vertex list into a continuous array, copy the vertices into the
    // vertex buffer.
    int attributeCount = 6; // x y z r g b

    float *vertexData = new float[RESU * RESV * attributeCount];
    for (int i = 0; i < RESU * RESV; i++)
        bezierVertexList[i].getAsArray(&vertexData[i * attributeCount]);
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, attributeCount * RESU * RESV * sizeof(float), vertexData, GL_STATIC_DRAW));

    // Copy the index data found in the list of triangles into the element array
    // buffer (index array) We are using a triangles, so we need triangleCount * 3
    // indices.
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BezierEBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bezierSize,
                        bezierElementArray,
                        GL_STATIC_DRAW));

    // Position attribute
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) 0));
    GLCall(glEnableVertexAttribArray(0));

    // Color attribute
    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) (3 * sizeof(GLfloat))));
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

    // Unbind VAO
    GLCall(glBindVertexArray(0));

    // Delete temporary buffers
    delete[] vertexData;
}

void Object3D::SetVertices() {
    vertexCount = 253;
    vertexList.resize(vertexCount);
    vertexList[0].set(-0.784, -1.4, 2.4, 1.0, 0.0, 0.0);
    vertexList[1].set(-1.4, -0.784, 2.4, 1.0, 0.0, 0.0);
    vertexList[2].set(-1.4, 0.0, 2.4, 1.0, 0.0, 0.0);
    vertexList[3].set(-0.749, -1.3375, 2.53125, 1.0, 0.0, 0.0);
    vertexList[4].set(-1.3375, -0.749, 2.53125, 1.0, 0.0, 0.0);
    vertexList[5].set(-1.3375, 0.0, 2.53125, 1.0, 0.0, 0.0);
    vertexList[6].set(-0.805, -1.4375, 2.53125, 1.0, 0.0, 0.0);
    vertexList[7].set(-1.4375, -0.805, 2.53125, 1.0, 0.0, 0.0);
    vertexList[8].set(-1.4375, 0.0, 2.53125, 1.0, 0.0, 0.0);
    vertexList[9].set(-0.84, -1.5, 2.4, 1.0, 0.0, 0.0);
    vertexList[10].set(-1.5, -0.84, 2.4, 1.0, 0.0, 0.0);
    vertexList[11].set(-1.5, 0.0, 2.4, 1.0, 0.0, 0.0);
    vertexList[12].set(-1.4, 0.784, 2.4, 1.0, 0.0, 0.0);
    vertexList[13].set(-0.784, 1.4, 2.4, 1.0, 0.0, 0.0);
    vertexList[14].set(0.0, 1.4, 2.4, 1.0, 0.0, 0.0);
    vertexList[15].set(-1.3375, 0.749, 2.53125, 1.0, 0.0, 0.0);
    vertexList[16].set(-0.749, 1.3375, 2.53125, 1.0, 0.0, 0.0);
    vertexList[17].set(0.0, 1.3375, 2.53125, 1.0, 0.0, 0.0);
    vertexList[18].set(-1.4375, 0.805, 2.53125, 1.0, 0.0, 0.0);
    vertexList[19].set(-0.805, 1.4375, 2.53125, 1.0, 0.0, 0.0);
    vertexList[20].set(0.0, 1.4375, 2.53125, 1.0, 0.0, 0.0);
    vertexList[21].set(-1.5, 0.84, 2.4, 1.0, 0.0, 0.0);
    vertexList[22].set(-0.84, 1.5, 2.4, 1.0, 0.0, 0.0);
    vertexList[23].set(0.0, 1.5, 2.4, 1.0, 0.0, 0.0);
    vertexList[24].set(0.784, 1.4, 2.4, 1.0, 0.0, 0.0);
    vertexList[25].set(1.4, 0.784, 2.4, 1.0, 0.0, 0.0);
    vertexList[26].set(0.749, 1.3375, 2.53125, 1.0, 0.0, 0.0);
    vertexList[27].set(1.3375, 0.749, 2.53125, 1.0, 0.0, 0.0);
    vertexList[28].set(0.805, 1.4375, 2.53125, 1.0, 0.0, 0.0);
    vertexList[29].set(1.4375, 0.805, 2.53125, 1.0, 0.0, 0.0);
    vertexList[30].set(0.84, 1.5, 2.4, 1.0, 0.0, 0.0);
    vertexList[31].set(1.5, 0.84, 2.4, 1.0, 0.0, 0.0);
    vertexList[32].set(1.75, 0.0, 1.875, 1.0, 0.0, 0.0);
    vertexList[33].set(1.75, -0.98, 1.875, 1.0, 0.0, 0.0);
    vertexList[34].set(0.98, -1.75, 1.875, 1.0, 0.0, 0.0);
    vertexList[35].set(0.0, -1.75, 1.875, 1.0, 0.0, 0.0);
    vertexList[36].set(2.0, 0.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[37].set(2.0, -1.12, 1.35, 1.0, 0.0, 0.0);
    vertexList[38].set(1.12, -2.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[39].set(0.0, -2.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[40].set(2.0, 0.0, 0.9, 1.0, 0.0, 0.0);
    vertexList[41].set(2.0, -1.12, 0.9, 1.0, 0.0, 0.0);
    vertexList[42].set(1.12, -2.0, 0.9, 1.0, 0.0, 0.0);
    vertexList[43].set(0.0, -2.0, 0.9, 1.0, 0.0, 0.0);
    vertexList[44].set(-0.98, -1.75, 1.875, 1.0, 0.0, 0.0);
    vertexList[45].set(-1.75, -0.98, 1.875, 1.0, 0.0, 0.0);
    vertexList[46].set(-1.75, 0.0, 1.875, 1.0, 0.0, 0.0);
    vertexList[47].set(-1.12, -2.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[48].set(-2.0, -1.12, 1.35, 1.0, 0.0, 0.0);
    vertexList[49].set(-2.0, 0.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[50].set(-1.12, -2.0, 0.9, 1.0, 0.0, 0.0);
    vertexList[51].set(-2.0, -1.12, 0.9, 1.0, 0.0, 0.0);
    vertexList[52].set(-2.0, 0.0, 0.9, 1.0, 0.0, 0.0);
    vertexList[53].set(-1.75, 0.98, 1.875, 1.0, 0.0, 0.0);
    vertexList[54].set(-0.98, 1.75, 1.875, 1.0, 0.0, 0.0);
    vertexList[55].set(0.0, 1.75, 1.875, 1.0, 0.0, 0.0);
    vertexList[56].set(-2.0, 1.12, 1.35, 1.0, 0.0, 0.0);
    vertexList[57].set(-1.12, 2.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[58].set(0.0, 2.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[59].set(-2.0, 1.12, 0.9, 1.0, 0.0, 0.0);
    vertexList[60].set(-1.12, 2.0, 0.9, 1.0, 0.0, 0.0);
    vertexList[61].set(0.0, 2.0, 0.9, 1.0, 0.0, 0.0);
    vertexList[62].set(0.98, 1.75, 1.875, 1.0, 0.0, 0.0);
    vertexList[63].set(1.75, 0.98, 1.875, 1.0, 0.0, 0.0);
    vertexList[64].set(1.12, 2.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[65].set(2.0, 1.12, 1.35, 1.0, 0.0, 0.0);
    vertexList[66].set(1.12, 2.0, 0.9, 1.0, 0.0, 0.0);
    vertexList[67].set(2.0, 1.12, 0.9, 1.0, 0.0, 0.0);
    vertexList[68].set(2.0, 0.0, 0.45, 1.0, 0.0, 0.0);
    vertexList[69].set(2.0, -1.12, 0.45, 1.0, 0.0, 0.0);
    vertexList[70].set(1.12, -2.0, 0.45, 1.0, 0.0, 0.0);
    vertexList[71].set(0.0, -2.0, 0.45, 1.0, 0.0, 0.0);
    vertexList[72].set(1.5, 0.0, 0.225, 1.0, 0.0, 0.0);
    vertexList[73].set(1.5, -0.84, 0.225, 1.0, 0.0, 0.0);
    vertexList[74].set(0.84, -1.5, 0.225, 1.0, 0.0, 0.0);
    vertexList[75].set(0.0, -1.5, 0.225, 1.0, 0.0, 0.0);
    vertexList[76].set(1.5, 0.0, 0.15, 1.0, 0.0, 0.0);
    vertexList[77].set(1.5, -0.84, 0.15, 1.0, 0.0, 0.0);
    vertexList[78].set(0.84, -1.5, 0.15, 1.0, 0.0, 0.0);
    vertexList[79].set(0.0, -1.5, 0.15, 1.0, 0.0, 0.0);
    vertexList[80].set(-1.12, -2.0, 0.45, 1.0, 0.0, 0.0);
    vertexList[81].set(-2.0, -1.12, 0.45, 1.0, 0.0, 0.0);
    vertexList[82].set(-2.0, 0.0, 0.45, 1.0, 0.0, 0.0);
    vertexList[83].set(-0.84, -1.5, 0.225, 1.0, 0.0, 0.0);
    vertexList[84].set(-1.5, -0.84, 0.225, 1.0, 0.0, 0.0);
    vertexList[85].set(-1.5, 0.0, 0.225, 1.0, 0.0, 0.0);
    vertexList[86].set(-0.84, -1.5, 0.15, 1.0, 0.0, 0.0);
    vertexList[87].set(-1.5, -0.84, 0.15, 1.0, 0.0, 0.0);
    vertexList[88].set(-1.5, 0.0, 0.15, 1.0, 0.0, 0.0);
    vertexList[89].set(-2.0, 1.12, 0.45, 1.0, 0.0, 0.0);
    vertexList[90].set(-1.12, 2.0, 0.45, 1.0, 0.0, 0.0);
    vertexList[91].set(0.0, 2.0, 0.45, 1.0, 0.0, 0.0);
    vertexList[92].set(-1.5, 0.84, 0.225, 1.0, 0.0, 0.0);
    vertexList[93].set(-0.84, 1.5, 0.225, 1.0, 0.0, 0.0);
    vertexList[94].set(0.0, 1.5, 0.225, 1.0, 0.0, 0.0);
    vertexList[95].set(-1.5, 0.84, 0.15, 1.0, 0.0, 0.0);
    vertexList[96].set(-0.84, 1.5, 0.15, 1.0, 0.0, 0.0);
    vertexList[97].set(0.0, 1.5, 0.15, 1.0, 0.0, 0.0);
    vertexList[98].set(1.12, 2.0, 0.45, 1.0, 0.0, 0.0);
    vertexList[99].set(2.0, 1.12, 0.45, 1.0, 0.0, 0.0);
    vertexList[100].set(0.84, 1.5, 0.225, 1.0, 0.0, 0.0);
    vertexList[101].set(1.5, 0.84, 0.225, 1.0, 0.0, 0.0);
    vertexList[102].set(0.84, 1.5, 0.15, 1.0, 0.0, 0.0);
    vertexList[103].set(1.5, 0.84, 0.15, 1.0, 0.0, 0.0);
    vertexList[104].set(-1.6, 0.0, 2.025, 1.0, 0.0, 0.0);
    vertexList[105].set(-1.6, -0.3, 2.025, 1.0, 0.0, 0.0);
    vertexList[106].set(-1.5, -0.3, 2.25, 1.0, 0.0, 0.0);
    vertexList[107].set(-1.5, 0.0, 2.25, 1.0, 0.0, 0.0);
    vertexList[108].set(-2.3, 0.0, 2.025, 1.0, 0.0, 0.0);
    vertexList[109].set(-2.3, -0.3, 2.025, 1.0, 0.0, 0.0);
    vertexList[110].set(-2.5, -0.3, 2.25, 1.0, 0.0, 0.0);
    vertexList[111].set(-2.5, 0.0, 2.25, 1.0, 0.0, 0.0);
    vertexList[112].set(-2.7, 0.0, 2.025, 1.0, 0.0, 0.0);
    vertexList[113].set(-2.7, -0.3, 2.025, 1.0, 0.0, 0.0);
    vertexList[114].set(-3.0, -0.3, 2.25, 1.0, 0.0, 0.0);
    vertexList[115].set(-3.0, 0.0, 2.25, 1.0, 0.0, 0.0);
    vertexList[116].set(-2.7, 0.0, 1.8, 1.0, 0.0, 0.0);
    vertexList[117].set(-2.7, -0.3, 1.8, 1.0, 0.0, 0.0);
    vertexList[118].set(-3.0, -0.3, 1.8, 1.0, 0.0, 0.0);
    vertexList[119].set(-3.0, 0.0, 1.8, 1.0, 0.0, 0.0);
    vertexList[120].set(-1.5, 0.3, 2.25, 1.0, 0.0, 0.0);
    vertexList[121].set(-1.6, 0.3, 2.025, 1.0, 0.0, 0.0);
    vertexList[122].set(-2.5, 0.3, 2.25, 1.0, 0.0, 0.0);
    vertexList[123].set(-2.3, 0.3, 2.025, 1.0, 0.0, 0.0);
    vertexList[124].set(-3.0, 0.3, 2.25, 1.0, 0.0, 0.0);
    vertexList[125].set(-2.7, 0.3, 2.025, 1.0, 0.0, 0.0);
    vertexList[126].set(-3.0, 0.3, 1.8, 1.0, 0.0, 0.0);
    vertexList[127].set(-2.7, 0.3, 1.8, 1.0, 0.0, 0.0);
    vertexList[128].set(-2.7, 0.0, 1.575, 1.0, 0.0, 0.0);
    vertexList[129].set(-2.7, -0.3, 1.575, 1.0, 0.0, 0.0);
    vertexList[130].set(-3.0, -0.3, 1.35, 1.0, 0.0, 0.0);
    vertexList[131].set(-3.0, 0.0, 1.35, 1.0, 0.0, 0.0);
    vertexList[132].set(-2.5, 0.0, 1.125, 1.0, 0.0, 0.0);
    vertexList[133].set(-2.5, -0.3, 1.125, 1.0, 0.0, 0.0);
    vertexList[134].set(-2.65, -0.3, 0.9375, 1.0, 0.0, 0.0);
    vertexList[135].set(-2.65, 0.0, 0.9375, 1.0, 0.0, 0.0);
    vertexList[136].set(-2.0, -0.3, 0.9, 1.0, 0.0, 0.0);
    vertexList[137].set(-1.9, -0.3, 0.6, 1.0, 0.0, 0.0);
    vertexList[138].set(-1.9, 0.0, 0.6, 1.0, 0.0, 0.0);
    vertexList[139].set(-3.0, 0.3, 1.35, 1.0, 0.0, 0.0);
    vertexList[140].set(-2.7, 0.3, 1.575, 1.0, 0.0, 0.0);
    vertexList[141].set(-2.65, 0.3, 0.9375, 1.0, 0.0, 0.0);
    vertexList[142].set(-2.5, 0.3, 1.1255, 1.0, 0.0, 0.0);
    vertexList[143].set(-1.9, 0.3, 0.6, 1.0, 0.0, 0.0);
    vertexList[144].set(-2.0, 0.3, 0.9, 1.0, 0.0, 0.0);
    vertexList[145].set(1.7, 0.0, 1.425, 1.0, 0.0, 0.0);
    vertexList[146].set(1.7, -0.66, 1.425, 1.0, 0.0, 0.0);
    vertexList[147].set(1.7, -0.66, 0.6, 1.0, 0.0, 0.0);
    vertexList[148].set(1.7, 0.0, 0.6, 1.0, 0.0, 0.0);
    vertexList[149].set(2.6, 0.0, 1.425, 1.0, 0.0, 0.0);
    vertexList[150].set(2.6, -0.66, 1.425, 1.0, 0.0, 0.0);
    vertexList[151].set(3.1, -0.66, 0.825, 1.0, 0.0, 0.0);
    vertexList[152].set(3.1, 0.0, 0.825, 1.0, 0.0, 0.0);
    vertexList[153].set(2.3, 0.0, 2.1, 1.0, 0.0, 0.0);
    vertexList[154].set(2.3, -0.25, 2.1, 1.0, 0.0, 0.0);
    vertexList[155].set(2.4, -0.25, 2.025, 1.0, 0.0, 0.0);
    vertexList[156].set(2.4, 0.0, 2.025, 1.0, 0.0, 0.0);
    vertexList[157].set(2.7, 0.0, 2.4, 1.0, 0.0, 0.0);
    vertexList[158].set(2.7, -0.25, 2.4, 1.0, 0.0, 0.0);
    vertexList[159].set(3.3, -0.25, 2.4, 1.0, 0.0, 0.0);
    vertexList[160].set(3.3, 0.0, 2.4, 1.0, 0.0, 0.0);
    vertexList[161].set(1.7, 0.66, 0.6, 1.0, 0.0, 0.0);
    vertexList[162].set(1.7, 0.66, 1.425, 1.0, 0.0, 0.0);
    vertexList[163].set(3.1, 0.66, 0.825, 1.0, 0.0, 0.0);
    vertexList[164].set(2.6, 0.66, 1.425, 1.0, 0.0, 0.0);
    vertexList[165].set(2.4, 0.25, 2.025, 1.0, 0.0, 0.0);
    vertexList[166].set(2.3, 0.25, 2.1, 1.0, 0.0, 0.0);
    vertexList[167].set(3.3, 0.25, 2.4, 1.0, 0.0, 0.0);
    vertexList[168].set(2.7, 0.25, 2.4, 1.0, 0.0, 0.0);
    vertexList[169].set(2.8, 0.0, 2.475, 1.0, 0.0, 0.0);
    vertexList[170].set(2.8, -0.25, 2.475, 1.0, 0.0, 0.0);
    vertexList[171].set(3.525, -0.25, 2.49375, 1.0, 0.0, 0.0);
    vertexList[172].set(3.525, 0.0, 2.49375, 1.0, 0.0, 0.0);
    vertexList[173].set(2.9, 0.0, 2.475, 1.0, 0.0, 0.0);
    vertexList[174].set(2.9, -0.15, 2.475, 1.0, 0.0, 0.0);
    vertexList[175].set(3.45, -0.15, 2.5125, 1.0, 0.0, 0.0);
    vertexList[176].set(3.45, 0.0, 2.5125, 1.0, 0.0, 0.0);
    vertexList[177].set(2.8, 0.0, 2.4, 1.0, 0.0, 0.0);
    vertexList[178].set(2.8, -0.15, 2.4, 1.0, 0.0, 0.0);
    vertexList[179].set(3.2, -0.15, 2.4, 1.0, 0.0, 0.0);
    vertexList[180].set(3.2, 0.0, 2.4, 1.0, 0.0, 0.0);
    vertexList[181].set(3.525, 0.25, 2.49375, 1.0, 0.0, 0.0);
    vertexList[182].set(2.8, 0.25, 2.475, 1.0, 0.0, 0.0);
    vertexList[183].set(3.45, 0.15, 2.5125, 1.0, 0.0, 0.0);
    vertexList[184].set(2.9, 0.15, 2.475, 1.0, 0.0, 0.0);
    vertexList[185].set(3.2, 0.15, 2.4, 1.0, 0.0, 0.0);
    vertexList[186].set(2.8, 0.15, 2.4, 1.0, 0.0, 0.0);
    vertexList[187].set(0.0, 0.0, 3.15, 1.0, 0.0, 0.0);
    vertexList[188].set(0.0, -0.002, 3.15, 1.0, 0.0, 0.0);
    vertexList[189].set(0.002, 0.0, 3.15, 1.0, 0.0, 0.0);
    vertexList[190].set(0.8, 0.0, 3.15, 1.0, 0.0, 0.0);
    vertexList[191].set(0.8, -0.45, 3.15, 1.0, 0.0, 0.0);
    vertexList[192].set(0.45, -0.8, 3.15, 1.0, 0.0, 0.0);
    vertexList[193].set(0.0, -0.8, 3.15, 1.0, 0.0, 0.0);
    vertexList[194].set(0.0, 0.0, 2.85, 1.0, 0.0, 0.0);
    vertexList[195].set(0.2, 0.0, 2.7, 1.0, 0.0, 0.0);
    vertexList[196].set(0.2, -0.112, 2.7, 1.0, 0.0, 0.0);
    vertexList[197].set(0.112, -0.2, 2.7, 1.0, 0.0, 0.0);
    vertexList[198].set(0.0, -0.2, 2.7, 1.0, 0.0, 0.0);
    vertexList[199].set(-0.002, 0.0, 3.15, 1.0, 0.0, 0.0);
    vertexList[200].set(-0.45, -0.8, 3.15, 1.0, 0.0, 0.0);
    vertexList[201].set(-0.8, -0.45, 3.15, 1.0, 0.0, 0.0);
    vertexList[202].set(-0.8, 0.0, 3.15, 1.0, 0.0, 0.0);
    vertexList[203].set(-0.112, -0.2, 2.7, 1.0, 0.0, 0.0);
    vertexList[204].set(-0.2, -0.112, 2.7, 1.0, 0.0, 0.0);
    vertexList[205].set(-0.2, 0.0, 2.7, 1.0, 0.0, 0.0);
    vertexList[206].set(0.0, 0.002, 3.15, 1.0, 0.0, 0.0);
    vertexList[207].set(-0.8, 0.45, 3.15, 1.0, 0.0, 0.0);
    vertexList[208].set(-0.45, 0.8, 3.15, 1.0, 0.0, 0.0);
    vertexList[209].set(0.0, 0.8, 3.15, 1.0, 0.0, 0.0);
    vertexList[210].set(-0.2, 0.112, 2.7, 1.0, 0.0, 0.0);
    vertexList[211].set(-0.112, 0.2, 2.7, 1.0, 0.0, 0.0);
    vertexList[212].set(0.0, 0.2, 2.7, 1.0, 0.0, 0.0);
    vertexList[213].set(0.45, 0.8, 3.15, 1.0, 0.0, 0.0);
    vertexList[214].set(0.8, 0.45, 3.15, 1.0, 0.0, 0.0);
    vertexList[215].set(0.112, 0.2, 2.7, 1.0, 0.0, 0.0);
    vertexList[216].set(0.2, 0.112, 2.7, 1.0, 0.0, 0.0);
    vertexList[217].set(0.4, 0.0, 2.55, 1.0, 0.0, 0.0);
    vertexList[218].set(0.4, -0.224, 2.55, 1.0, 0.0, 0.0);
    vertexList[219].set(0.224, -0.4, 2.55, 1.0, 0.0, 0.0);
    vertexList[220].set(0.0, -0.4, 2.55, 1.0, 0.0, 0.0);
    vertexList[221].set(1.3, 0.0, 2.55, 1.0, 0.0, 0.0);
    vertexList[222].set(1.3, -0.728, 2.55, 1.0, 0.0, 0.0);
    vertexList[223].set(0.728, -1.3, 2.55, 1.0, 0.0, 0.0);
    vertexList[224].set(0.0, -1.3, 2.55, 1.0, 0.0, 0.0);
    vertexList[225].set(1.3, 0.0, 2.4, 1.0, 0.0, 0.0);
    vertexList[226].set(1.3, -0.728, 2.4, 1.0, 0.0, 0.0);
    vertexList[227].set(0.728, -1.3, 2.4, 1.0, 0.0, 0.0);
    vertexList[228].set(0.0, -1.3, 2.4, 1.0, 0.0, 0.0);
    vertexList[229].set(-0.224, -0.4, 2.55, 1.0, 0.0, 0.0);
    vertexList[230].set(-0.4, -0.224, 2.55, 1.0, 0.0, 0.0);
    vertexList[231].set(-0.4, 0.0, 2.55, 1.0, 0.0, 0.0);
    vertexList[232].set(-0.728, -1.3, 2.55, 1.0, 0.0, 0.0);
    vertexList[233].set(-1.3, -0.728, 2.55, 1.0, 0.0, 0.0);
    vertexList[234].set(-1.3, 0.0, 2.55, 1.0, 0.0, 0.0);
    vertexList[235].set(-0.728, -1.3, 2.4, 1.0, 0.0, 0.0);
    vertexList[236].set(-1.3, -0.728, 2.4, 1.0, 0.0, 0.0);
    vertexList[237].set(-1.3, 0.0, 2.4, 1.0, 0.0, 0.0);
    vertexList[238].set(-0.4, 0.224, 2.55, 1.0, 0.0, 0.0);
    vertexList[239].set(-0.224, 0.4, 2.55, 1.0, 0.0, 0.0);
    vertexList[240].set(0.0, 0.4, 2.55, 1.0, 0.0, 0.0);
    vertexList[241].set(-1.3, 0.728, 2.55, 1.0, 0.0, 0.0);
    vertexList[242].set(-0.728, 1.3, 2.55, 1.0, 0.0, 0.0);
    vertexList[243].set(0.0, 1.3, 2.55, 1.0, 0.0, 0.0);
    vertexList[244].set(-1.3, 0.728, 2.4, 1.0, 0.0, 0.0);
    vertexList[245].set(-0.728, 1.3, 2.4, 1.0, 0.0, 0.0);
    vertexList[246].set(0.0, 1.3, 2.4, 1.0, 0.0, 0.0);
    vertexList[247].set(0.224, 0.4, 2.55, 1.0, 0.0, 0.0);
    vertexList[248].set(0.4, 0.224, 2.55, 1.0, 0.0, 0.0);
    vertexList[249].set(0.728, 1.3, 2.55, 1.0, 0.0, 0.0);
    vertexList[250].set(1.3, 0.728, 2.55, 1.0, 0.0, 0.0);
    vertexList[251].set(0.728, 1.3, 2.4, 1.0, 0.0, 0.0);
    vertexList[252].set(1.3, 0.728, 2.4, 1.0, 0.0, 0.0);
}