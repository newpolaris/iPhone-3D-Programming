#include <fstream>
#include <algorithm>

#include "ObjectSurface.h"

std::istream& operator>>(std::istream& stream, ivec3& out)
{
	return stream >> out.x >> out.y >> out.z;
}

std::istream& operator>>(std::istream& stream, vec3& out)
{
	return stream >> out.x >> out.y >> out.z;
}

ObjSurface::ObjSurface(const string& name) :
	m_name(name)
{
	using std::ifstream;
	using std::string;
	using std::vector;
	using std::istreambuf_iterator;

	ifstream objFile(m_name.c_str());
	objFile.exceptions( ifstream::badbit );
	if (!objFile.is_open())
		throw std::exception("Could not open file");

	std::back_insert_iterator<vector<vec3>> v_it(m_vertices);

	while (objFile) {
		char type = objFile.peek();
		if (type == 'v') {
			vec3 v;
			objFile >> type >> v.x >> v.y >> v.z;
			*v_it++ = v; 
		} else {
			break;
		}
		objFile.ignore(512, '\n');
	}

	std::back_insert_iterator<vector<ivec3>> f_it(m_faces);
	while (objFile) {
		char type = objFile.peek();
		if (type == 'f') {
			ivec3 f;
			objFile >> type >> f.x >> f.y >> f.z;
			*f_it++ = f-ivec3(1, 1, 1);
		} else {
			break;
		}
		objFile.ignore(512, '\n');
	}

	m_normals = vector<vec3>(GetVertexCount(), vec3(0.f, 0.f, 0.f));

	for (auto face = m_faces.begin(); face != m_faces.end(); ++face)
	{
		int vertex0 = face->x;
		int vertex1 = face->y;
		int vertex2 = face->z;

		vec3 a = m_vertices[vertex0];
		vec3 b = m_vertices[vertex1];
		vec3 c = m_vertices[vertex2];

		vec3 faceNormal = (b-a).Cross(c-a);

		m_normals[vertex0] += faceNormal;
		m_normals[vertex1] += faceNormal;
		m_normals[vertex2] += faceNormal;
	}

	for (int i = 0; i < m_normals.size(); i++)
		m_normals[i].Normalize();
}

void ObjSurface::GenerateVertices(vector<float>& vertices, unsigned char flags) const 
{
    // 삼각형을 저장하기 위해 3배수 만큼 용량을 늘린다.
    int floatsPerVertex = 3;

    // 만약 Normal vector가 필요하다면 추가로 3 만큼의 공간을 더 늘린다.
    if (flags & VertexFlagsNormals)
        floatsPerVertex += 3;

	vertices.reserve(GetVertexCount() * floatsPerVertex);

	back_insert_iterator<vector<float>> it(vertices);

	for (int i = 0; i < GetVertexCount(); ++i) {
		*it++ = m_vertices[i].x;
		*it++ = m_vertices[i].y;
		*it++ = m_vertices[i].z;

		*it++ = m_normals[i].x;
		*it++ = m_normals[i].y;
		*it++ = m_normals[i].z;
	}
}

void ObjSurface::GenerateTriangleIndices(vector<unsigned short>& indices) const
{
	for (int i = 0; i < m_faces.size(); i++) {
		int idx = i*3;
		indices[idx++] = m_faces[i].x;
		indices[idx++] = m_faces[i].y;
		indices[idx++] = m_faces[i].z;
	}
}
