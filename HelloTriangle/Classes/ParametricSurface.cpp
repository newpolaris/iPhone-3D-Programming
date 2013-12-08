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

void ParametricSurface::GenerateVertices(vector<float>& vertices,
                                         unsigned char flags) const
{
    // 삼각형을 저장하기 위해 3배수 만큼 용량을 늘린다.
    int floatsPerVertex = 3;

    // 만약 Normal vector가 필요하다면 추가로 3 만큼의 공간을 더 늘린다.
    if (flags & VertexFlagsNormals)
        floatsPerVertex += 3;

    vertices.resize(GetVertexCount() * floatsPerVertex);
    float* attribute = (float*)&vertices[0];

    for (int j = 0; j < m_divisions.y; j++) {
		for (int i = 0; i < m_divisions.x; i++) {

            // 위치 계산
			vec2 domain = ComputeDomain(i, j);
			vec3 range = Evaluate(domain);
            attribute = range.Write(attribute);

            // Normal calcurate
            if (flags & VertexFlagsNormals) {
                float s = i, t = j;

                // 법선을 계산할 수 없는 경우, 값을 약간 증가시킨다.
                if (i == 0) s += 0.01f;
                if (i == m_divisions.x - 1) s -= 0.01f;
                if (j == 0) t += 0.01f;
                if (j == m_divisions.y - 1) t -= 0.01f;

                // 접평면과 벡터의 외적을 계산한다.
                vec3 p = Evaluate(ComputeDomain(s, t));
                vec3 u = Evaluate(ComputeDomain(s + 0.01f, t)) - p;
                vec3 v = Evaluate(ComputeDomain(s, t + 0.01f)) - p;

                vec3 normal = u.Cross(v).Normalized();
                if (InvertNormal(domain))
                    normal = -normal;

                attribute = normal.Write(attribute);
            }
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

