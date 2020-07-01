#include <string>
#include <cstring>
#include <vector>
#include <../include/rply/rply.h>
#include "GeometricDefinitions.h"

class PlyFileParser {
private:
    static int vertexId;
    static int triangleId;
    static int triangleVertexId;
    std::string filePath;

    int vertexNum;
    int triangleNum;

    static int VertexCallback(p_ply_argument argument);

    static int TriangleCallback(p_ply_argument argument);

public:
    explicit PlyFileParser(std::string filePath);

    virtual ~PlyFileParser();

    void ParseFile(std::vector<Vertex> *vertexList, std::vector<Triangle> *triangleList);

    int getVertexNum() const;

    int getTriangleNum() const;
};