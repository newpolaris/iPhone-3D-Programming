#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "Interfaces.hpp"
#include "Matrix.hpp"
#include <iostream>
#include <stdlib.h>

namespace ES2 {

#define STRINGIFY(A)  #A
#include "../Shaders/SimpleLighting.vert"
#include "../Shaders/Simple.frag"

struct AttributeHandle
{
    GLuint Position;
    GLuint Normal;
    GLuint DiffuseMaterial;
};

struct UniformHandle
{
    GLint Projection;
    GLint Modelview;
    GLint NormalMatrix;
    GLint LightPosition;
    GLint AmbientMaterial;
    GLint SpecularMaterial;
    GLint Shininess;
};

struct Drawable {
    GLuint VertexBuffer;
    GLuint IndexBuffer;
    int IndexCount;
};

class RenderingEngine : public IRenderingEngine {
public:
    RenderingEngine();
    void Initialize(const vector<ISurface*>& surfaces);
    void Render(const vector<Visual>& visuals) const;
private:
    GLuint BuildShader(const char* source, GLenum shaderType) const;
    GLuint BuildProgram(const char* vShader, const char* fShader) const;
    vector<Drawable> m_drawables;
    // GLuint m_colorRenderbuffer;
    
    UniformHandle m_uniform;
    AttributeHandle m_attribute;

	GLuint m_depthRenderbuffer;
    mat4 m_translation;
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
        
        // Create a new VBO for the indices if needed.
        int indexCount = (*surface)->GetTriangleIndexCount();
        GLuint indexBuffer;
        if (!m_drawables.empty() && indexCount == m_drawables[0].IndexCount) {
            indexBuffer = m_drawables[0].IndexBuffer;
        } else {
            vector<GLushort> indices(indexCount);
            (*surface)->GenerateTriangleIndices(indices);
            glGenBuffers(1, &indexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indexCount * sizeof(GLushort),
                         &indices[0],
                         GL_STATIC_DRAW);
        }
        
        Drawable drawable = { vertexBuffer, indexBuffer, indexCount};
        m_drawables.push_back(drawable);
    }
    
	// 색상 버퍼에서 넓이와 높이를 추출한다.
	int width, height;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

	// 색상 버퍼와 같은 크기의 깊이 버퍼를 생성한다.
	glGenRenderbuffers(1, &m_depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

	glEnable(GL_DEPTH_TEST);
    // Create the framebuffer object.
    // GLuint framebuffer;
    // glGenFramebuffers(1, &framebuffer);
    // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    //                           GL_RENDERBUFFER, m_colorRenderbuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
    
    // Create the GLSL program.
    GLuint program = BuildProgram(SimpleLightingVertexShader, SimpleFragmentShader);
    glUseProgram(program);
    m_attribute.Position = glGetAttribLocation(program, "Position");
    m_attribute.Normal = glGetAttribLocation(program, "Normal");
    m_attribute.DiffuseMaterial = glGetAttribLocation(program, "DiffuseMaterial");

    glEnableVertexAttribArray(m_attribute.Position);
    glEnableVertexAttribArray(m_attribute.Normal);
    // glEnableVertexAttribArray(m_attribute.DiffuseMaterial);

    // Set up some matrices.
    m_uniform.Projection = glGetUniformLocation(program, "Projection");
    m_uniform.Modelview = glGetUniformLocation(program, "Modelview");
    m_uniform.NormalMatrix = glGetUniformLocation(program, "NormalMatrix");
    m_uniform.LightPosition = glGetUniformLocation(program, "LightPosition");
    m_uniform.AmbientMaterial = glGetUniformLocation(program, "AmbientMaterial");
    m_uniform.SpecularMaterial = glGetUniformLocation(program, "SpecularMaterial");
    m_uniform.Shininess = glGetUniformLocation(program, "Shininess");

    // Set Light settings.
    vec3 lightPosition(0.25, 0.25, 1);
    glUniform3fv(m_uniform.LightPosition, 1, lightPosition.Pointer()); 
    glUniform3f(m_uniform.AmbientMaterial, 0, 0, 0.5); // light blue 
    glUniform3f(m_uniform.SpecularMaterial, 1, 1, 0); // orange
    glUniform1f(m_uniform.Shininess, 1.5);

    // set translation.
    m_translation = mat4::Translate(0, 0, -7);
}

void RenderingEngine::Render(const vector<Visual>& visuals) const
{
    glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    vector<Visual>::const_iterator visual = visuals.begin();
    for (int visualIndex = 0; visual != visuals.end(); ++visual, ++visualIndex) {

        // Set the viewport transform.
        ivec2 size = visual->ViewportSize;
        ivec2 lowerLeft = visual->LowerLeft;
        glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);

        // Set the model-view transform.
        mat4 rotation = visual->Orientation.ToMatrix();
        mat4 modelview = rotation * m_translation;
        glUniformMatrix4fv(m_uniform.Modelview, 1, 0, modelview.Pointer());

        // Set the projection transform.
        float h = 4.0f * size.y / size.x;
        mat4 projectionMatrix = mat4::Frustum(-2, 2, -h / 2, h / 2, 5, 10);
        glUniformMatrix4fv(m_uniform.Projection, 1, 0, projectionMatrix.Pointer());
        
        // Set the normal matrix
		// It's orthogoal, so Its Inverse-Transpose matrix is itself!
		mat3 normalMatrix = modelview.ToMat3();
        glUniformMatrix3fv(m_uniform.NormalMatrix, 1, 0, normalMatrix.Pointer());

        // Set the color.
        vec3 color = visual->Color;
        glVertexAttrib3f(m_attribute.DiffuseMaterial,
                         color.x, color.y, color.z);
        
        // Draw the wireframe.
        int stride = 2*sizeof(vec3);
        const GLvoid* offset = (const GLvoid*)sizeof(vec3);
        const Drawable& drawable = m_drawables[visualIndex];
        glBindBuffer(GL_ARRAY_BUFFER, drawable.VertexBuffer);
        glVertexAttribPointer(m_attribute.Position, 3, GL_FLOAT, 
                              GL_FALSE, stride, 0);
        glVertexAttribPointer(m_attribute.Normal, 3, GL_FLOAT,
                              GL_FALSE, stride, offset);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.IndexBuffer);
		glDrawElements(GL_TRIANGLES, drawable.IndexCount, GL_UNSIGNED_SHORT, 0);
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
