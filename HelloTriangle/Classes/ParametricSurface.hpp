#include "Interfaces.hpp"

struct ParametricInterval {
    ivec2 Divisions;
    vec2 UpperBound;
};

class ParametricSurface : public ISurface {
public:
    int GetVertexCount() const;
    int GetLineIndexCount() const;
	int GetTriangleIndexCount() const;
    void GenerateVertices(vector<float>& vertices) const;
    void GenerateLineIndices(vector<unsigned short>& indices) const;
	void GenerateTriangleIndices(vector<unsigned short>& indices) const;

protected:
    void SetInterval(const ParametricInterval& interval);
    virtual vec3 Evaluate(const vec2& domain) const = 0;

private:
    vec2 ComputeDomain(float i, float j) const;
    vec2 m_upperBound;
    ivec2 m_slices;
    ivec2 m_divisions;
};

