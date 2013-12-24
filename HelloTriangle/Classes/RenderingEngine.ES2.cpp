#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Interfaces.hpp"
#include "Matrix.hpp"
#include <iostream>
#include <stdlib.h>

namespace ES2 {

#define STRINGIFY(A)  #A

#include "../Shaders/PixelLighting.vert"
#include "../Shaders/PixelLighting.frag"
#include "../Shaders/Simple.vert"
#include "../Shaders/Simple.frag"

struct AttributeHandle
{
    GLuint Position;
    GLuint Normal;
    GLuint DiffuseMaterial;
    GLuint AmbientMaterial;
};

struct AttributeLineHandle
{
    GLuint Position;
    GLuint Color;
};

struct UniformHandle
{
    GLint Projection;
    GLint Modelview;
    GLint NormalMatrix;
    GLint LightPosition;
    GLint SpecularMaterial;
    GLint Shininess;
};

struct UniformLineHandle
{
    GLint Projection;
    GLint Modelview;
};

struct Drawable {
    GLuint VertexBuffer;
    GLuint TriangleIndexBuffer;
    int TriangleIndexCount;
	GLuint LineIndexBuffer;
	int LineIndexCount;
};

class RenderingEngine : public IRenderingEngine {
public:
    RenderingEngine();
    void Initialize(const vector<ISurface*>& surfaces);
    void Render(const vector<Visual>& visuals) const;
private:
    GLuint BuildShader(const char* source, GLenum shaderType) const;
    GLuint BuildProgram(const char* vShader, const char* fShader) const;
	void RenderTriangles(mat4& modelview, mat4& projectionMatrix, const vec3& color, const Drawable& drawable) const;
	void RenderLines(mat4& modelview, mat4& projectionMatrix, const Drawable& drawable) const;

    vector<Drawable> m_drawables;
    // GLuint m_colorRenderbuffer;

    UniformHandle m_uniform;
    AttributeHandle m_attribute;

    UniformLineHandle m_uniformLine;
    AttributeLineHandle m_attributeLine;

	GLuint m_depthRenderbuffer;
    mat4 m_translation;

	GLuint m_triangle_program; 
	GLuint m_line_program;
};

IRenderingEngine* CreateRenderingEngine()
{
    return new RenderingEngine();
}

RenderingEngine::RenderingEngine()
{
    // glGenRenderbuffers(1, &m_colorRenderbuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
}

void RenderingEngine::Initialize(const vector<ISurface*>& surfaces)
{
    vector<ISurface*>::const_iterator surface;
    for (surface = surfaces.begin(); 
         surface != surfaces.end(); ++surface) {

        // Create the VBO for the vertices.
        vector<float> vertices;
        (*surface)->GenerateVertices(vertices, VertexFlagsNormals);
        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertices[0]),
                     &vertices[0],
                     GL_STATIC_DRAW);

		GLenum error = glGetError();
		if (error == GL_INVALID_ENUM) {
			int i = 0;
		}
        
        // Create a new VBO for the trinagle indices if needed.
        int TriangleIndexCount = (*surface)->GetTriangleIndexCount();
        GLuint TriangleIndexBuffer;
        if (!m_drawables.empty() 
		 && TriangleIndexCount == m_drawables[0].TriangleIndexCount) {
            TriangleIndexBuffer = m_drawables[0].TriangleIndexBuffer;
        } else {
            vector<GLushort> indices(TriangleIndexCount);
            (*surface)->GenerateTriangleIndices(indices);
            glGenBuffers(1, &TriangleIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TriangleIndexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         TriangleIndexCount * sizeof(GLushort),
                         &indices[0],
                         GL_STATIC_DRAW);
        }
        
        // Create a new VBO for the trinagle indices if needed.
        int LineIndexCount = (*surface)->GetLineIndexCount();
		GLuint LineIndexBuffer = 0;
		if (LineIndexCount != 0) {
			if (!m_drawables.empty() 
				&& LineIndexCount == m_drawables[0].LineIndexCount) {
					LineIndexBuffer = m_drawables[0].LineIndexBuffer;
			} else {
				vector<GLushort> indices(LineIndexCount);
				(*surface)->GenerateLineIndices(indices);
				glGenBuffers(1, &LineIndexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LineIndexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					LineIndexCount * sizeof(GLushort),
					&indices[0],
					GL_STATIC_DRAW);
			}
        }

        Drawable drawable = { vertexBuffer,
							  TriangleIndexBuffer,
							  TriangleIndexCount,
							  LineIndexBuffer,
							  LineIndexCount };

        m_drawables.push_back(drawable);
    }
    
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(4, 8);

	// 색상 버퍼에서 넓이와 높이를 추출한다.
	int width, height;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

	// 색상 버퍼와 같은 크기의 깊이 버퍼를 생성한다.
	glGenRenderbuffers(1, &m_depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    // Create the framebuffer object.
    // GLuint framebuffer;
    // glGenFramebuffers(1, &framebuffer);
    // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    //                           GL_RENDERBUFFER, m_colorRenderbuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
    
    // Create the GLSL program.
    GLuint program = 0;
    program = BuildProgram(PixelLightingVertexShader, PixelLightingFragmentShader);
    m_attribute.Position = glGetAttribLocation(program, "Position");
    m_attribute.Normal = glGetAttribLocation(program, "Normal");
    m_attribute.DiffuseMaterial = glGetAttribLocation(program, "DiffuseMaterial");
    m_attribute.AmbientMaterial = glGetUniformLocation(program, "AmbientMaterial");

    // Set up some matrices.
    m_uniform.Projection = glGetUniformLocation(program, "Projection");
    m_uniform.Modelview = glGetUniformLocation(program, "Modelview");
    m_uniform.NormalMatrix = glGetUniformLocation(program, "NormalMatrix");
    m_uniform.LightPosition = glGetUniformLocation(program, "LightPosition");
    m_uniform.SpecularMaterial = glGetUniformLocation(program, "SpecularMaterial");
    m_uniform.Shininess = glGetUniformLocation(program, "Shininess");
	m_triangle_program = program;

	glUseProgram(m_triangle_program);

    // Set Light settings.
    glVertexAttrib3f(m_attribute.AmbientMaterial, 0.04f, 0.04f, 0.04f); 
    glUniform3f(m_uniform.LightPosition, 0.25, 0.25, 0.25);
    glUniform3f(m_uniform.SpecularMaterial, 0.5, 0.5, 0.5);
    glUniform1f(m_uniform.Shininess, 50);

    program = BuildProgram(SimpleVertexShader, SimpleFragmentShader);

    // Set up some matries.
    m_attributeLine.Position = glGetAttribLocation(program, "Position");
    m_attributeLine.Color = glGetAttribLocation(program, "SourceColor");

    m_uniformLine.Projection = glGetUniformLocation(program, "Projection");
    m_uniformLine.Modelview = glGetUniformLocation(program, "Modelview");

	glUseProgram(program);

    // glEnableVertexAttribArray(m_attributeLine.Position);

    m_line_program = program;

    // set translation.
    m_translation = mat4::Translate(0, 0, -7);
}

void RenderingEngine::RenderTriangles(mat4& modelview,
									  mat4& projectionMatrix,
									  const vec3& Color,
									  const Drawable& drawable) const
{
	glEnable(GL_POLYGON_OFFSET_FILL);

	int stride = 2*sizeof(vec3);
	const GLvoid* offset = (const GLvoid*)sizeof(vec3);

	glUseProgram(m_triangle_program);
	glUniformMatrix4fv(m_uniform.Modelview, 1, 0, modelview.Pointer());
	glUniformMatrix4fv(m_uniform.Projection, 1, 0, projectionMatrix.Pointer());

	// Set the normal matrix
	// It's orthogoal, so Its Inverse-Transpose matrix is itself!
	mat3 normalMatrix = modelview.ToMat3();
	glUniformMatrix3fv(m_uniform.NormalMatrix, 1, 0, normalMatrix.Pointer());

	// Set the color.
	vec3 color = Color * 0.75f;
	glVertexAttrib3f(m_attribute.DiffuseMaterial,
		color.x, color.y, color.z);

	glEnableVertexAttribArray(m_attribute.Position);
	glEnableVertexAttribArray(m_attribute.Normal);

	glBindBuffer(GL_ARRAY_BUFFER, drawable.VertexBuffer);
	glVertexAttribPointer(m_attribute.Position, 3, GL_FLOAT, 
		GL_FALSE, stride, 0);
	glVertexAttribPointer(m_attribute.Normal, 3, GL_FLOAT,
		GL_FALSE, stride, offset);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.TriangleIndexBuffer);
	glDrawElements(GL_TRIANGLES, drawable.TriangleIndexCount, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(m_attribute.Position);
	glDisableVertexAttribArray(m_attribute.Normal);
	glDisable(GL_POLYGON_OFFSET_FILL);
}

void RenderingEngine::RenderLines(mat4& modelview, 
								  mat4& projectionMatrix, 
								  const Drawable& drawable) const
{
	glUseProgram(m_line_program);

	int stride = 2*sizeof(vec3);

	glUniformMatrix4fv(m_uniformLine.Modelview, 1, 0, modelview.Pointer());
	glUniformMatrix4fv(m_uniformLine.Projection, 1, 0, projectionMatrix.Pointer());
	glVertexAttrib4f(m_attributeLine.Color, 1.f, 1.f, 1.f, 1.f);

	glEnableVertexAttribArray(m_attributeLine.Position);
	glBindBuffer(GL_ARRAY_BUFFER, drawable.VertexBuffer);
	glVertexAttribPointer(m_attributeLine.Position, 3, GL_FLOAT,
		GL_FALSE, stride, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.LineIndexBuffer);
	glDrawElements(GL_LINES, drawable.LineIndexCount, GL_UNSIGNED_SHORT, 0);
	glDisableVertexAttribArray(m_attributeLine.Position);
}

void RenderingEngine::Render(const vector<Visual>& visuals) const
{
    glClearColor(0.0, 0.125f, 0.25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    vector<Visual>::const_iterator visual = visuals.begin();
    for (int visualIndex = 0; visual != visuals.end(); ++visual, ++visualIndex) {
        // Set the viewport transform.
        ivec2 size = visual->ViewportSize;
        ivec2 lowerLeft = visual->LowerLeft;
        glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);

		// Draw the wireframe.
		const Drawable& drawable = m_drawables[visualIndex];

		// Set the model-view transform.
		mat4 rotation = visual->Orientation.ToMatrix();
		mat4 modelview = rotation * m_translation;

		// Set the projection transform.
		float h = 4.0f * size.y / size.x;
		mat4 projectionMatrix = mat4::Frustum(-2, 2, -h / 2, h / 2, 5, 10);

		RenderTriangles(modelview, projectionMatrix, visual->Color, drawable);
		if (drawable.LineIndexCount == 0)
			RenderLines(modelview, projectionMatrix, drawable);
    }
}

GLuint RenderingEngine::BuildShader(const char* source, GLenum shaderType) const
{
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &source, 0);
    glCompileShader(shaderHandle);
    
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages;
        exit(1);
    }
    
    return shaderHandle;
}

GLuint RenderingEngine::BuildProgram(const char* vertexShaderSource,
                                     const char* fragmentShaderSource) const
{
    GLuint vertexShader = BuildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = BuildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages;
        exit(1);
    }
    
    return programHandle;
}
    
}
