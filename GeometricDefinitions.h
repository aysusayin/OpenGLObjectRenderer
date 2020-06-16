#pragma once

#define ORDER 3
#define RESU 10
#define RESV 10
#include <cmath>

struct Vertex {
    static const int attributeCount = 8;
    float x, y, z;  /* the usual 3-space position of a vertex */
    float r, g, b;  // Color
    float texX{0.5}, texY{0.5};

    void getAsArray(float *attributeArray) {
        // Keep the declaration order
        attributeArray[0] = x;
        attributeArray[1] = y;
        attributeArray[2] = z;
        attributeArray[3] = r;
        attributeArray[4] = g;
        attributeArray[5] = b;
        attributeArray[6] = texX;
        attributeArray[7] = texY;
    }

    Vertex(){
        x = 0;
        y = 0;
        z = 0;
        r = 0;
        g = 0;
        b = 0;
    };

    Vertex(float v_x, float v_y, float v_z, float v_r, float v_g, float v_b) {
        x = v_x;
        y = v_y;
        z = v_z;
        r = v_r;
        g = v_g;
        b = v_b;
        texX = x;
        texY = y;
    }

};

inline Vertex operator*(float a, const Vertex &v) {
    Vertex vertex;
    vertex.x = a * v.x;
    vertex.y = a * v.y;
    vertex.z = a * v.z;
    vertex.r = v.r;
    vertex.g = v.g;
    vertex.b = v.b;
    return vertex;
}

inline Vertex operator*(const Vertex &v, float a) {
    Vertex vertex;
    vertex.x = a * v.x;
    vertex.y = a * v.y;
    vertex.z = a * v.z;
    vertex.r = v.r;
    vertex.g = v.g;
    vertex.b = v.b;
    return vertex;
}

inline Vertex operator+(const Vertex &v1, const Vertex &v2) {
    Vertex vertex;
    vertex.x = v1.x + v2.x;
    vertex.y = v1.y + v2.y;
    vertex.z = v1.z + v2.z;
    vertex.r = (v1.r + v2.r) / 2;
    vertex.g = (v1.g + v2.g) / 2;
    vertex.b = (v1.b + v2.b) / 2;
    return vertex;
}

struct BezierSurface {
    unsigned int controlPoints[ORDER + 1][ORDER + 1];
};
