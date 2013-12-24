#include <iterator>
#include <istream>

#include "Interfaces.hpp"

using namespace std;

class ObjSurface : public ISurface {
public:
	ObjSurface(const string& name);
    ~ObjSurface() {}

    int GetVertexCount() const { return m_vertices.size(); }
	int GetLineIndexCount() const { return 0; }
	int GetTriangleIndexCount() const { return m_faces.size()*3; }
    void GenerateVertices(vector<float>& vertices, unsigned char flags = 0) const;
	void GenerateLineIndices(vector<unsigned short>& indices) const {}
	void GenerateTriangleIndices(vector<unsigned short>& indices) const;

private:
	string m_name;

	vector<vec3> m_vertices;
	vector<vec3> m_normals;
	vector<ivec3> m_faces;
};