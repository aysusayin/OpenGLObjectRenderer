#include "Object3D.h"

#define STB_IMAGE_IMPLEMENTATION

#include "vendor/stb.h"

Object3D::Object3D(glm::mat4 modelMatrix) {
    this->modelMatrix = modelMatrix;
    this->texturePath = "res/textures/default.png";
    this->isTextureSet = true;
    SetTexture(this->texturePath);
}

Object3D::~Object3D() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);

    if (vertexArray != nullptr) {
        delete[] vertexArray;
        vertexArray = nullptr;
    }
    if (elementArray != nullptr) {
        delete[] elementArray;
        elementArray = nullptr;
    }
}

void Object3D::BindBuffers() {
    // Bind the vertex and index buffers
    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glGenBuffers(1, &EBO));

    // Bind our Vertex Array Object first, then bind and set our buffers and
    // pointers.
    GLCall(glBindVertexArray(VAO));

    // Convert our vertex list into a continuous array, copy the vertices into the
    // vertex buffer.
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, vertexCount * Vertex::attributeCount * sizeof(GLfloat), vertexArray,
                        GL_STATIC_DRAW));

    // Bind buffer element array.
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * 3 * sizeof(GLuint), elementArray, GL_STATIC_DRAW));

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::attributeCount * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::attributeCount * sizeof(GLfloat),
                          (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Texture Position Attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex::attributeCount * sizeof(GLfloat),
                          (GLvoid *) (6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind VAO
    glBindVertexArray(0);
}

void Object3D::CreateFromPlyFile(std::string filePath) {
    PlyFileParser parser(filePath);
    vertexCount = parser.getVertexNum();
    elementCount = parser.getTriangleNum();
    vertexList.resize(vertexCount);
    triangleList.resize(elementCount);
    parser.ParseFile(&vertexList, &triangleList);
    SetElementArray();
    SetVertexArray();
    BindBuffers();
}

void
Object3D::DrawObject(Shader *shader, glm::mat4 *projectionMatrix, glm::mat4 *viewMatrix, glm::mat4 *animationMatrix) {
    GLCall(glBindVertexArray(VAO));
    glActiveTexture(GL_TEXTURE0);
    GLCall(glBindTexture(GL_TEXTURE_2D, texture));

    shader->SetUniform1i("u_texture", 0);
    shader->SetUniformMat4fv("projection", *projectionMatrix);
    shader->SetUniformMat4fv("view", *viewMatrix);
    shader->SetUniformMat4fv("model", modelMatrix);
    shader->SetUniformMat4fv("animation", *animationMatrix);

    GLCall(glDrawElements(GL_TRIANGLES, elementCount * 3, GL_UNSIGNED_INT, 0));
    GLCall(glBindVertexArray(0));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

}

void Object3D::SetTexture(std::string filePath) {
    texturePath = filePath;
    SetTexture();
}

void Object3D::SetTexture() {
    int w, h, BPP;
    unsigned char *textureBuffer;

    stbi_set_flip_vertically_on_load(1);
    textureBuffer = stbi_load(texturePath.c_str(), &w, &h, &BPP, 4);

    GLCall(glGenTextures(1, &texture));
    GLCall(glBindTexture(GL_TEXTURE_2D, texture));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    if (textureBuffer) {
        stbi_image_free(textureBuffer);
    }
}

void Object3D::SetElementArray() {
    elementArray = (unsigned int *) malloc(3 * elementCount * sizeof(unsigned int));
    for (int i = 0; i < elementCount; i++)
        memcpy(&elementArray[i * 3], triangleList[i].vertexIndexList, sizeof(int) * 3);
};

void Object3D::SetVertexArray() {
    vertexArray = (float *) malloc(vertexCount * Vertex::attributeCount * sizeof(float));
    for (int i = 0; i < vertexCount; i++)
        vertexList[i].getAsArray(&vertexArray[i * Vertex::attributeCount]);
};

UtahTeapot::UtahTeapot(std::string texturePath, glm::mat4 modelMatrix) {
    this->modelMatrix = modelMatrix;
    this->texturePath = texturePath;
    this->isTextureSet = !texturePath.compare("res/textures/default.png") == 0;

    SetVertices();
    SetBezierPatches();
    SetTexture(this->texturePath);
    SetVertexArray();
    SetElementArray();
    BindBuffers();
}

void UtahTeapot::GenerateBezierVertexList(Vertex *bezierVertexList) {
    for (int p = 0; p < bezierSurfaceCount; p++) {
        for (int ru = 0; ru < RESU; ru++) {
            float u = 1.0 * ru / (RESU - 1);
            for (int rv = 0; rv < RESV; rv++) {
                float v = 1.0 * rv / (RESV - 1);
                bezierVertexList[p * RESU * RESV + ru * RESV + rv] = CalculateBezierVertices(u, v, p);
            }
        }
    }
}

void UtahTeapot::GenerateBezierElementArray() {
    int n = 0;
    for (int i = 0; i < bezierSurfaceCount; i++) {
        for (int ru = 0; ru < RESU - 1; ru++) {
            for (int rv = 0; rv < RESV - 1; rv++) {
                // 1 square ABCD = 2 triangles ABC + CDA
                // ABC
                elementArray[n] = i * RESU * RESV + ru * RESV + rv;
                n++;
                elementArray[n] = i * RESU * RESV + ru * RESV + (rv + 1);
                n++;
                elementArray[n] = i * RESU * RESV + (ru + 1) * RESV + (rv + 1);
                n++;
                // CDA
                elementArray[n] = i * RESU * RESV + (ru + 1) * RESV + (rv + 1);
                n++;
                elementArray[n] = i * RESU * RESV + (ru + 1) * RESV + rv;
                n++;
                elementArray[n] = i * RESU * RESV + ru * RESV + rv;
                n++;
            }
        }
    }
}

Vertex UtahTeapot::CalculateBezierVertices(float u, float v, int index) {
    Vertex *result = new Vertex();
    for (int i = 0; i < ORDER + 1; i++) {
        for (int j = 0; j < ORDER + 1; j++) {
            *result = *result +
                      (CalculateBernsteinPolynomial(ORDER, i, u) * CalculateBernsteinPolynomial(ORDER, j, v) *
                       allVertexList[bezierSurfacesList[index].controlPoints[i][j]]);
        }
    }
    if (isTextureSet) {
        result->r = 1.0f;
        result->g = 1.0f;
        result->b = 1.0f;
    }

    result->texX = (atan2(result->z, result->x)) + M_PI;
    result->texY = (result->y);
    return *result;
}

int UtahTeapot::factorial(int n) {
    return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

float UtahTeapot::CalculateBinomialCoeff(int n, int i) {
    return factorial(n) * 1.0f / (factorial(i) * factorial(n - i));
}

float UtahTeapot::CalculateBernsteinPolynomial(int n, int i, float u) {
    return CalculateBinomialCoeff(n, i) * (float) pow(u, i) * (float) pow(1 - u, n - i);
}

void UtahTeapot::SetVertexArray() {
    Vertex *bezierVertexList = (Vertex *) malloc(RESU * RESV * bezierSurfaceCount * sizeof(Vertex));
    GenerateBezierVertexList(bezierVertexList);

    vertexCount = bezierSurfaceCount * RESU * RESV;
    vertexArray = (float *) malloc(vertexCount * Vertex::attributeCount * sizeof(float));

    for (int i = 0; i < vertexCount; i++)
        bezierVertexList[i].getAsArray(&vertexArray[i * Vertex::attributeCount]);
}

void UtahTeapot::SetElementArray() {
    elementCount = bezierSurfaceCount * (RESU - 1) * (RESV - 1) * 2; // 2 because a patch is 2 triangles
    elementArray = (unsigned int *) malloc(3 * elementCount * sizeof(unsigned int)); // each triangle has 3 vertices
    GenerateBezierElementArray();
}

void UtahTeapot::SetBezierPatches() {
    unsigned int teapotPatches[][ORDER + 1][ORDER + 1] = {
            // rim
            {{1,   2,   3,   4},   {5,   6,   7,   8},   {9,   10,  11,  12},  {13,  14,  15,  16}},
            {{4,   17,  18,  19},  {8,   20,  21,  22},  {12,  23,  24,  25},  {16,  26,  27,  28}},
            {{19,  29,  30,  31},  {22,  32,  33,  34},  {25,  35,  36,  37},  {28,  38,  39,  40}},
            {{31,  41,  42,  1},   {34,  43,  44,  5},   {37,  45,  46,  9},   {40,  47,  48,  13}},
            // body
            {{13,  14,  15,  16},  {49,  50,  51,  52},  {53,  54,  55,  56},  {57,  58,  59,  60}},
            {{16,  26,  27,  28},  {52,  61,  62,  63},  {56,  64,  65,  66},  {60,  67,  68,  69}},
            {{28,  38,  39,  40},  {63,  70,  71,  72},  {66,  73,  74,  75},  {69,  76,  77,  78}},
            {{40,  47,  48,  13},  {72,  79,  80,  49},  {75,  81,  82,  53},  {78,  83,  84,  57}},
            {{57,  58,  59,  60},  {85,  86,  87,  88},  {89,  90,  91,  92},  {93,  94,  95,  96}},
            {{60,  67,  68,  69},  {88,  97,  98,  99},  {92,  100, 101, 102}, {96,  103, 104, 105}},
            {{69,  76,  77,  78},  {99,  106, 107, 108}, {102, 109, 110, 111}, {105, 112, 113, 114}},
            {{78,  83,  84,  57},  {108, 115, 116, 85},  {111, 117, 118, 89},  {114, 119, 120, 93}},
            // handle
            {{121, 122, 123, 124}, {125, 126, 127, 128}, {129, 130, 131, 132}, {133, 134, 135, 136}},
            {{124, 137, 138, 121}, {128, 139, 140, 125}, {132, 141, 142, 129}, {136, 143, 144, 133}},
            {{133, 134, 135, 136}, {145, 146, 147, 148}, {149, 150, 151, 152}, {69,  153, 154, 155}},
            {{136, 143, 144, 133}, {148, 156, 157, 145}, {152, 158, 159, 149}, {155, 160, 161, 69}},
            // spout
            {{162, 163, 164, 165}, {166, 167, 168, 169}, {170, 171, 172, 173}, {174, 175, 176, 177}},
            {{165, 178, 179, 162}, {169, 180, 181, 166}, {173, 182, 183, 170}, {177, 184, 185, 174}},
            {{174, 175, 176, 177}, {186, 187, 188, 189}, {190, 191, 192, 193}, {194, 195, 196, 197}},
            {{177, 184, 185, 174}, {189, 198, 199, 186}, {193, 200, 201, 190}, {197, 202, 203, 194}},
            // lid
            {{204, 204, 204, 204}, {207, 208, 209, 210}, {211, 211, 211, 211}, {212, 213, 214, 215}},
            {{204, 204, 204, 204}, {210, 217, 218, 219}, {211, 211, 211, 211}, {215, 220, 221, 222}},
            {{204, 204, 204, 204}, {219, 224, 225, 226}, {211, 211, 211, 211}, {222, 227, 228, 229}},
            {{204, 204, 204, 204}, {226, 230, 231, 207}, {211, 211, 211, 211}, {229, 232, 233, 212}},
            {{212, 213, 214, 215}, {234, 235, 236, 237}, {238, 239, 240, 241}, {242, 243, 244, 245}},
            {{215, 220, 221, 222}, {237, 246, 247, 248}, {241, 249, 250, 251}, {245, 252, 253, 254}},
            {{222, 227, 228, 229}, {248, 255, 256, 257}, {251, 258, 259, 260}, {254, 261, 262, 263}},
            {{229, 232, 233, 212}, {257, 264, 265, 234}, {260, 266, 267, 238}, {263, 268, 269, 242}}
            // no bottom!
    };
    bezierSurfaceCount = sizeof(teapotPatches) / sizeof(teapotPatches[0]);
    bezierSurfacesList.resize(bezierSurfaceCount);
    for (int p = 0; p < bezierSurfaceCount; p++) {
        for (int i = 0; i < ORDER + 1; i++) {
            for (int j = 0; j < ORDER + 1; j++) {
                bezierSurfacesList[p].controlPoints[i][j] = teapotPatches[p][i][j] - 1;
            }
        }
    }
}

void UtahTeapot::SetVertices() {
    float teapotCPVertices[][3] = {
            // 1
            {1.4,     0.0,     2.4},
            {1.4,     -0.784,  2.4},
            {0.784,   -1.4,    2.4},
            {0.0,     -1.4,    2.4},
            {1.3375,  0.0,     2.53125},
            {1.3375,  -0.749,  2.53125},
            {0.749,   -1.3375, 2.53125},
            {0.0,     -1.3375, 2.53125},
            {1.4375,  0.0,     2.53125},
            {1.4375,  -0.805,  2.53125},
            // 11
            {0.805,   -1.4375, 2.53125},
            {0.0,     -1.4375, 2.53125},
            {1.5,     0.0,     2.4},
            {1.5,     -0.84,   2.4},
            {0.84,    -1.5,    2.4},
            {0.0,     -1.5,    2.4},
            {-0.784,  -1.4,    2.4},
            {-1.4,    -0.784,  2.4},
            {-1.4,    0.0,     2.4},
            {-0.749,  -1.3375, 2.53125},
            // 21
            {-1.3375, -0.749,  2.53125},
            {-1.3375, 0.0,     2.53125},
            {-0.805,  -1.4375, 2.53125},
            {-1.4375, -0.805,  2.53125},
            {-1.4375, 0.0,     2.53125},
            {-0.84,   -1.5,    2.4},
            {-1.5,    -0.84,   2.4},
            {-1.5,    0.0,     2.4},
            {-1.4,    0.784,   2.4},
            {-0.784,  1.4,     2.4},
            // 31
            {0.0,     1.4,     2.4},
            {-1.3375, 0.749,   2.53125},
            {-0.749,  1.3375,  2.53125},
            {0.0,     1.3375,  2.53125},
            {-1.4375, 0.805,   2.53125},
            {-0.805,  1.4375,  2.53125},
            {0.0,     1.4375,  2.53125},
            {-1.5,    0.84,    2.4},
            {-0.84,   1.5,     2.4},
            {0.0,     1.5,     2.4},
            // 41
            {0.784,   1.4,     2.4},
            {1.4,     0.784,   2.4},
            {0.749,   1.3375,  2.53125},
            {1.3375,  0.749,   2.53125},
            {0.805,   1.4375,  2.53125},
            {1.4375,  0.805,   2.53125},
            {0.84,    1.5,     2.4},
            {1.5,     0.84,    2.4},
            {1.75,    0.0,     1.875},
            {1.75,    -0.98,   1.875},
            // 51
            {0.98,    -1.75,   1.875},
            {0.0,     -1.75,   1.875},
            {2.0,     0.0,     1.35},
            {2.0,     -1.12,   1.35},
            {1.12,    -2.0,    1.35},
            {0.0,     -2.0,    1.35},
            {2.0,     0.0,     0.9},
            {2.0,     -1.12,   0.9},
            {1.12,    -2.0,    0.9},
            {0.0,     -2.0,    0.9},
            // 61
            {-0.98,   -1.75,   1.875},
            {-1.75,   -0.98,   1.875},
            {-1.75,   0.0,     1.875},
            {-1.12,   -2.0,    1.35},
            {-2.0,    -1.12,   1.35},
            {-2.0,    0.0,     1.35},
            {-1.12,   -2.0,    0.9},
            {-2.0,    -1.12,   0.9},
            {-2.0,    0.0,     0.9},
            {-1.75,   0.98,    1.875},
            // 71
            {-0.98,   1.75,    1.875},
            {0.0,     1.75,    1.875},
            {-2.0,    1.12,    1.35},
            {-1.12,   2.0,     1.35},
            {0.0,     2.0,     1.35},
            {-2.0,    1.12,    0.9},
            {-1.12,   2.0,     0.9},
            {0.0,     2.0,     0.9},
            {0.98,    1.75,    1.875},
            {1.75,    0.98,    1.875},
            // 81
            {1.12,    2.0,     1.35},
            {2.0,     1.12,    1.35},
            {1.12,    2.0,     0.9},
            {2.0,     1.12,    0.9},
            {2.0,     0.0,     0.45},
            {2.0,     -1.12,   0.45},
            {1.12,    -2.0,    0.45},
            {0.0,     -2.0,    0.45},
            {1.5,     0.0,     0.225},
            {1.5,     -0.84,   0.225},
            // 91
            {0.84,    -1.5,    0.225},
            {0.0,     -1.5,    0.225},
            {1.5,     0.0,     0.15},
            {1.5,     -0.84,   0.15},
            {0.84,    -1.5,    0.15},
            {0.0,     -1.5,    0.15},
            {-1.12,   -2.0,    0.45},
            {-2.0,    -1.12,   0.45},
            {-2.0,    0.0,     0.45},
            {-0.84,   -1.5,    0.225},
            // 101
            {-1.5,    -0.84,   0.225},
            {-1.5,    0.0,     0.225},
            {-0.84,   -1.5,    0.15},
            {-1.5,    -0.84,   0.15},
            {-1.5,    0.0,     0.15},
            {-2.0,    1.12,    0.45},
            {-1.12,   2.0,     0.45},
            {0.0,     2.0,     0.45},
            {-1.5,    0.84,    0.225},
            {-0.84,   1.5,     0.225},
            // 111
            {0.0,     1.5,     0.225},
            {-1.5,    0.84,    0.15},
            {-0.84,   1.5,     0.15},
            {0.0,     1.5,     0.15},
            {1.12,    2.0,     0.45},
            {2.0,     1.12,    0.45},
            {0.84,    1.5,     0.225},
            {1.5,     0.84,    0.225},
            {0.84,    1.5,     0.15},
            {1.5,     0.84,    0.15},
            // 121
            {-1.6,    0.0,     2.025},
            {-1.6,    -0.3,    2.025},
            {-1.5,    -0.3,    2.25},
            {-1.5,    0.0,     2.25},
            {-2.3,    0.0,     2.025},
            {-2.3,    -0.3,    2.025},
            {-2.5,    -0.3,    2.25},
            {-2.5,    0.0,     2.25},
            {-2.7,    0.0,     2.025},
            {-2.7,    -0.3,    2.025},
            // 131
            {-3.0,    -0.3,    2.25},
            {-3.0,    0.0,     2.25},
            {-2.7,    0.0,     1.8},
            {-2.7,    -0.3,    1.8},
            {-3.0,    -0.3,    1.8},
            {-3.0,    0.0,     1.8},
            {-1.5,    0.3,     2.25},
            {-1.6,    0.3,     2.025},
            {-2.5,    0.3,     2.25},
            {-2.3,    0.3,     2.025},
            // 141
            {-3.0,    0.3,     2.25},
            {-2.7,    0.3,     2.025},
            {-3.0,    0.3,     1.8},
            {-2.7,    0.3,     1.8},
            {-2.7,    0.0,     1.575},
            {-2.7,    -0.3,    1.575},
            {-3.0,    -0.3,    1.35},
            {-3.0,    0.0,     1.35},
            {-2.5,    0.0,     1.125},
            {-2.5,    -0.3,    1.125},
            // 151
            {-2.65,   -0.3,    0.9375},
            {-2.65,   0.0,     0.9375},
            {-2.0,    -0.3,    0.9},
            {-1.9,    -0.3,    0.6},
            {-1.9,    0.0,     0.6},
            {-3.0,    0.3,     1.35},
            {-2.7,    0.3,     1.575},
            {-2.65,   0.3,     0.9375},
            {-2.5,    0.3,     1.1255},
            {-1.9,    0.3,     0.6},
            // 161
            {-2.0,    0.3,     0.9},
            {1.7,     0.0,     1.425},
            {1.7,     -0.66,   1.425},
            {1.7,     -0.66,   0.6},
            {1.7,     0.0,     0.6},
            {2.6,     0.0,     1.425},
            {2.6,     -0.66,   1.425},
            {3.1,     -0.66,   0.825},
            {3.1,     0.0,     0.825},
            {2.3,     0.0,     2.1},
            // 171
            {2.3,     -0.25,   2.1},
            {2.4,     -0.25,   2.025},
            {2.4,     0.0,     2.025},
            {2.7,     0.0,     2.4},
            {2.7,     -0.25,   2.4},
            {3.3,     -0.25,   2.4},
            {3.3,     0.0,     2.4},
            {1.7,     0.66,    0.6},
            {1.7,     0.66,    1.425},
            {3.1,     0.66,    0.825},
            // 181
            {2.6,     0.66,    1.425},
            {2.4,     0.25,    2.025},
            {2.3,     0.25,    2.1},
            {3.3,     0.25,    2.4},
            {2.7,     0.25,    2.4},
            {2.8,     0.0,     2.475},
            {2.8,     -0.25,   2.475},
            {3.525,   -0.25,   2.49375},
            {3.525,   0.0,     2.49375},
            {2.9,     0.0,     2.475},
            // 191
            {2.9,     -0.15,   2.475},
            {3.45,    -0.15,   2.5125},
            {3.45,    0.0,     2.5125},
            {2.8,     0.0,     2.4},
            {2.8,     -0.15,   2.4},
            {3.2,     -0.15,   2.4},
            {3.2,     0.0,     2.4},
            {3.525,   0.25,    2.49375},
            {2.8,     0.25,    2.475},
            {3.45,    0.15,    2.5125},
            // 201
            {2.9,     0.15,    2.475},
            {3.2,     0.15,    2.4},
            {2.8,     0.15,    2.4},
            {0.0,     0.0,     3.15},
            {0.0,     -0.002,  3.15},
            {0.002,   0.0,     3.15},
            {0.8,     0.0,     3.15},
            {0.8,     -0.45,   3.15},
            {0.45,    -0.8,    3.15},
            {0.0,     -0.8,    3.15},
            // 211
            {0.0,     0.0,     2.85},
            {0.2,     0.0,     2.7},
            {0.2,     -0.112,  2.7},
            {0.112,   -0.2,    2.7},
            {0.0,     -0.2,    2.7},
            {-0.002,  0.0,     3.15},
            {-0.45,   -0.8,    3.15},
            {-0.8,    -0.45,   3.15},
            {-0.8,    0.0,     3.15},
            {-0.112,  -0.2,    2.7},
            // 221
            {-0.2,    -0.112,  2.7},
            {-0.2,    0.0,     2.7},
            {0.0,     0.002,   3.15},
            {-0.8,    0.45,    3.15},
            {-0.45,   0.8,     3.15},
            {0.0,     0.8,     3.15},
            {-0.2,    0.112,   2.7},
            {-0.112,  0.2,     2.7},
            {0.0,     0.2,     2.7},
            {0.45,    0.8,     3.15},
            // 231
            {0.8,     0.45,    3.15},
            {0.112,   0.2,     2.7},
            {0.2,     0.112,   2.7},
            {0.4,     0.0,     2.55},
            {0.4,     -0.224,  2.55},
            {0.224,   -0.4,    2.55},
            {0.0,     -0.4,    2.55},
            {1.3,     0.0,     2.55},
            {1.3,     -0.728,  2.55},
            {0.728,   -1.3,    2.55},
            // 241
            {0.0,     -1.3,    2.55},
            {1.3,     0.0,     2.4},
            {1.3,     -0.728,  2.4},
            {0.728,   -1.3,    2.4},
            {0.0,     -1.3,    2.4},
            {-0.224,  -0.4,    2.55},
            {-0.4,    -0.224,  2.55},
            {-0.4,    0.0,     2.55},
            {-0.728,  -1.3,    2.55},
            {-1.3,    -0.728,  2.55},
            // 251
            {-1.3,    0.0,     2.55},
            {-0.728,  -1.3,    2.4},
            {-1.3,    -0.728,  2.4},
            {-1.3,    0.0,     2.4},
            {-0.4,    0.224,   2.55},
            {-0.224,  0.4,     2.55},
            {0.0,     0.4,     2.55},
            {-1.3,    0.728,   2.55},
            {-0.728,  1.3,     2.55},
            {0.0,     1.3,     2.55},
            // 261
            {-1.3,    0.728,   2.4},
            {-0.728,  1.3,     2.4},
            {0.0,     1.3,     2.4},
            {0.224,   0.4,     2.55},
            {0.4,     0.224,   2.55},
            {0.728,   1.3,     2.55},
            {1.3,     0.728,   2.55},
            {0.728,   1.3,     2.4},
            {1.3,     0.728,   2.4},
    };
    int numVertices = sizeof(teapotCPVertices) / sizeof(teapotCPVertices[0]);
    for (int j = 0; j < numVertices; j++) {
        allVertexList.emplace_back(teapotCPVertices[j][0], teapotCPVertices[j][1], teapotCPVertices[j][2],
                                   0.8f, 0.8f, 0.0f);
    }
}

