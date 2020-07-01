#include "PlyFileParser.h"

int PlyFileParser::vertexId = 0, PlyFileParser::triangleId = 0, PlyFileParser::triangleVertexId = 0;

enum VertexProperty {
    x, y, z, r, g, b
};

PlyFileParser::PlyFileParser(std::string filePath) {
    p_ply ply;
    this->filePath = filePath;

    ply = ply_open((char *) filePath.c_str(), NULL, 0, NULL);
    ply_read_header(ply);

    p_ply_element element = NULL;
    long numInstances = 0;
    const char *element_name;

    while ((element = ply_get_next_element(ply, element))) {
        ply_get_element_info(element, &element_name, &numInstances);
        if (strcmp(element_name, "vertex") == 0)
            vertexNum = numInstances;
        else if (strcmp(element_name, "triangle") == 0)
            triangleNum = numInstances;
    };

    ply_close(ply);

}

PlyFileParser::~PlyFileParser() {}

void PlyFileParser::ParseFile(std::vector<Vertex> *vertexList, std::vector<Triangle> *triangleList) {
    p_ply ply;
    vertexId = 0;
    triangleId = 0;
    triangleVertexId = 0;
    // Read the .ply file. We will have "vertex" and "triangle" elements in the .ply file.
    ply = ply_open((char *) filePath.c_str(), NULL, 0, NULL);
    // read the header/file format
    ply_read_header(ply);

    ply_set_read_cb(ply, "vertex", "x", VertexCallback, vertexList, VertexProperty::x);
    ply_set_read_cb(ply, "vertex", "y", VertexCallback, vertexList, VertexProperty::y);
    ply_set_read_cb(ply, "vertex", "z", VertexCallback, vertexList, VertexProperty::z);
    ply_set_read_cb(ply, "vertex", "r", VertexCallback, vertexList, VertexProperty::r);
    ply_set_read_cb(ply, "vertex", "g", VertexCallback, vertexList, VertexProperty::g);
    ply_set_read_cb(ply, "vertex", "b", VertexCallback, vertexList, VertexProperty::b);
    ply_set_read_cb(ply, "triangle", "vertex_indices", TriangleCallback, triangleList, 0);

    ply_read(ply);
    ply_close(ply);
}

int PlyFileParser::VertexCallback(p_ply_argument argument) {
    long propVal;
    void *ptr;
    float val = ply_get_argument_value(argument);
    ply_get_argument_user_data(argument, &ptr, &propVal);
    auto *vertexVector = reinterpret_cast<std::vector<Vertex> *>(ptr);

    switch (propVal) {
        case VertexProperty::x:
            (*vertexVector)[vertexId].x = val;
            break;
        case VertexProperty::y:
            (*vertexVector)[vertexId].y = val;
            break;
        case VertexProperty::z:
            (*vertexVector)[vertexId].z = val;
            break;
        case VertexProperty::r:
            (*vertexVector)[vertexId].r = val;
            break;
        case VertexProperty::g:
            (*vertexVector)[vertexId].g = val;
            break;
        case VertexProperty::b:
            (*vertexVector)[vertexId++].b = val;
            break;
    }
    return 1;
}

int PlyFileParser::TriangleCallback(p_ply_argument argument) {
    long length, value_index, propVal;
    void *ptr;
    int val = ply_get_argument_value(argument);
    ply_get_argument_user_data(argument, &ptr, &propVal);
    ply_get_argument_property(argument, NULL, &length, &value_index);
    auto *triangleVector = reinterpret_cast<std::vector<Triangle> *>(ptr);

    if (value_index == -1) {
        triangleVertexId = 0;
        (*triangleVector)[triangleId].numberOfVertices = val;
        (*triangleVector)[triangleId].vertexIndexList = (int *) malloc(val * sizeof(int));
    } else if (value_index == (*triangleVector)[triangleId].numberOfVertices - 1) {
        (*triangleVector)[triangleId++].vertexIndexList[triangleVertexId] = val;
    } else {
        (*triangleVector)[triangleId].vertexIndexList[triangleVertexId++] = val;
    }

    return 1;
}

int PlyFileParser::getVertexNum() const {
    return vertexNum;
}

int PlyFileParser::getTriangleNum() const {
    return triangleNum;
}
