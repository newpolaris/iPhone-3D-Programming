#include "Interfaces.hpp"
#include "ParametricSurface.hpp"

void ParametricSurface::SetInterval(const ParametricInterval& interval)
{
    m_upperBound = interval.UpperBound;
    m_divisions = interval.Divisions;
    m_slices = m_divisions - ivec2(1, 1);
}

int ParametricSurface::GetVertexCount() const
{
    return m_divisions.x * m_divisions.y;
}

int ParametricSurface::GetLineIndexCount() const
{
    return 4 * m_slices.x * m_slices.y;
}

int ParametricSurface::GetTriangleIndexCount() const
{
	return 6 * m_slices.x * m_slices.y;
}

vec2 ParametricSurface::ComputeDomain(float x, float y) const
{
    return vec2(x * m_upperBound.x / m_slices.x,
                y * m_upperBound.y / m_slices.y);
}

void ParametricSurface::GenerateVertices(vector<float>& vertices) const
{
    vertices.resize(GetVertexCount() * 3);
    vec3* position = (vec3*)&vertices[0];

    for (int j = 0; j < m_divisions.y; j++) {
		for (int i = 0; i < m_divisions.x; i++) {
			vec2 domain = ComputeDomain(i, j);
			vec3 range = Evaluate(domain);
			*position++ = range;
		}
    }
}

void ParametricSurface::GenerateLineIndices(vector<unsigned short>& indices) const
{
    indices.resize(GetLineIndexCount());
    vector<unsigned short>::iterator index = indices.begin();
    for (int j = 0, vertex = 0; j < m_slices.y; j++) {
        for (int i = 0; i < m_slices.x; i++) {
            *index++ = vertex + i;
            *index++ = vertex + i+1;
            *index++ = vertex + i;
            *index++ = vertex + i + m_divisions.x;
        }
        vertex += m_divisions.x;
    }
}

void ParametricSurface::GenerateTriangleIndices(vector<unsigned short>& indices)
																		const
{
	indices.resize(GetTriangleIndexCount());
	vector<unsigned short>::iterator index = indices.begin();
	for (int j = 0, vertex = 0; j < m_slices.y; j++) {
		for (int i = 0; i < m_slices.x; i++) {
			int next = (i + 1) % m_divisions.x;
            *index++ = vertex + i;
            *index++ = vertex + next;
            *index++ = vertex + i + m_divisions.x;
            *index++ = vertex + next;
            *index++ = vertex + next + m_divisions.x;
            *index++ = vertex + i + m_divisions.x;
		}
		vertex += m_divisions.x;
	}
}

