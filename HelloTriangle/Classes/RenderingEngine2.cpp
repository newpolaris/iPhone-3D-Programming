#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "IRenderingEngine.hpp"
#include "Quaternion.hpp"
#include "Vector.hpp"

#define STRINGIFY(A) #A

#include "../Shaders/Simple.vert"
#include "../Shaders/Simple.frag"

using namespace std;

static const float AnimationDuration = 0.25f;

struct Vertex {
    vec3 Position;
    vec4 Color;
};

struct Animation {
    Quaternion Start;
    Quaternion End;
    Quaternion Current;
    float Elapsed;
    float Duration;
};

class RenderingEngine2: public IRenderingEngine {
public:
    RenderingEngine2();
    int Initialize(int width, int height);
    void Render() const;
    void UpdateAnimation(float timeStep) {}
    void OnRotate(DeviceOrientation newOrientation) {}
    void OnFingerUp(ivec2 location);
    void OnFingerDown(ivec2 location);
    void OnFingerMove(ivec2 oldLocation, ivec2 newLocation);

private:
	GLuint BuildProgram(const char* vertexShaderSource, const char* fragmentShaderSource) const;
	GLuint BuildShader(const char* source, GLenum shaderType) const;

	// GL의 상태 변화는 알지 못하네?
	void ApplyRotation(float degrees) const;
	void ApplyOrtho(float maxX, float maxY) const;

    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLuint m_bodyIndexCount;
    GLuint m_diskIndexCount;
    GLfloat m_rotationAngle;
    GLfloat m_scale;
    ivec2 m_pivotPoint;

    GLuint m_simpleProgram;
    // GLuint m_framebuffer;
    // GLuint m_colorRenderbuffer;
    // GLuint m_depthRenderbuffer;

	int mWidth;
	int mHeight;
};

IRenderingEngine* RenderingEngine() {
    static RenderingEngine2 engine;
    return &engine;
}

RenderingEngine2::RenderingEngine2() 
    : m_rotationAngle(0), m_scale(1)
{
	// glGenRenderbuffers(1, &m_colorRenderbuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, m_colorRenderbuffer);
}

int RenderingEngine2::Initialize(int width, int height) 
{
	mWidth = width;
	mHeight = height;

    m_pivotPoint = ivec2(width / 2, height / 2);

    const float coneRadius = 0.5f;
    const float coneHeight = 1.866f;
    const int coneSlices = 80;
    const float dtheta = TwoPi / coneSlices;

    // 꼭대기 점점을 각각 다른 색으로 지정하기 위해
    // Apex vertices: n (for each set different color)
    // bottom disk boundary: n
    // center of disk: 1
    const int vertexCount = coneSlices * 2 + 1;

    vector<Vertex> coneVertices(vertexCount);
    vector<Vertex>::iterator vertex = coneVertices.begin();

    for (float theta = 0; vertex != coneVertices.end() - 1 ; theta += dtheta) {
        // grayscale gradient
        float brightness = abs(sin(theta));

        vec4 color(brightness, brightness, brightness, 1);

        // Apex vertex
        vertex->Position = vec3(0, 1, 0);
        vertex->Color = color;
        vertex++;

        vertex->Position.x = coneRadius * cos(theta);
        vertex->Position.y = 1 - coneHeight;
        vertex->Position.z = coneRadius * sin(theta);
        vertex->Color = color;
        vertex++;
    }

    // center of disk
    vertex->Position = vec3(0, 1 - coneHeight, 0);
    vertex->Color = vec4(1, 1, 1, 1);

    // 정점을 저장할 VBO 생성.
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 coneVertices.size() * sizeof(coneVertices[0]),
                 &coneVertices[0],
                 GL_STATIC_DRAW);
    
    m_bodyIndexCount = coneSlices * 3;
	m_diskIndexCount = coneSlices * 3;

    vector<GLubyte> coneIndices(m_bodyIndexCount + m_diskIndexCount);
    vector<GLubyte>::iterator index = coneIndices.begin();

    // Body Triangles.
    for (int i = 0; i < coneSlices * 2; i+= 2) {
        *index++ = i;
        *index++ = (i + 1) % (2 * coneSlices); 
        *index++ = (i + 3) % (2 * coneSlices);
    }

	// Bottom triangles.
	const int diskCenterIndex = vertexCount - 1;

	for (int i = 1; i < coneSlices * 2 + 1; i += 2) {
		*index++ = diskCenterIndex;
		*index++ = i;
		*index++ = (i + 2) % (2 * coneSlices);
	}

    // Create VBO to store indices.
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 coneIndices.size() * sizeof(coneIndices[0]),
                 &coneIndices[0],
                 GL_STATIC_DRAW);

	// Create framebuffer object; attach the depth and colour buffers.
	// glGenFramebuffers(1, &m_framebuffer);
	// glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	// glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
    //                          GL_COLOR_ATTACHMENT0,
    //                           GL_RENDERBUFFER,
    //                           m_colorRenderbuffer);
	// glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
    //                           GL_DEPTH_ATTACHMENT,
    //                           GL_RENDERBUFFER,
    //                           m_depthRenderbuffer);

    // Set up some GL state.
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

	// Build the GLSL program.
	m_simpleProgram = BuildProgram( SimpleVertexShader, SimpleFragmentShader );
	glUseProgram(m_simpleProgram);

    // Set the projection matrix.
    GLint projectionUniform = glGetUniformLocation(m_simpleProgram, "Projection");
    mat4 projectionMatrix = mat4::Frustum(-1.6f, 1.6f, -2.4, 2.4, 5, 10);
    glUniformMatrix4fv(projectionUniform, 1, 0, projectionMatrix.Pointer());

	return TRUE;
}

void RenderingEngine2::OnFingerUp(ivec2 location)
{
    m_scale = 1.0f;
}

void RenderingEngine2::OnFingerDown(ivec2 location)
{
    m_scale = 1.5f;
    OnFingerMove(location, location);
}

void RenderingEngine2::OnFingerMove(ivec2 previou, ivec2 location)
{
    vec2 direction = vec2(location - m_pivotPoint).Normalized();

    // 필셀 좌표는 아래로 갈수록 커지므로, y축 값의 부호를 바꾼다.
    direction.y = -direction.y;

    m_rotationAngle = std::acos(direction.y) * 180.0f / 3.14159f;
    if (direction.x > 0)
        m_rotationAngle = -m_rotationAngle;
}

void RenderingEngine2::ApplyOrtho(float maxX, float maxY) const
{
	float a = 1.0f / maxX;
	float b = 1.0f / maxY;

	float ortho[16] = {
		a, 0, 0, 0,
		0, b, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
	};

	GLint projectionUniform = glGetUniformLocation(m_simpleProgram, 
												   "Projection");

	glUniformMatrix4fv(projectionUniform, 1, 0, &ortho[0]);
}

void RenderingEngine2::ApplyRotation(float degrees) const
{
	float radians = degrees * 3.14159f / 180.0f;
	float s =std::sin(radians);
	float c = std::cos(radians);

	float zRotation[16] = {
		c, s, 0, 0,
		-s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	GLint modelviewUniform = glGetUniformLocation(m_simpleProgram,
												 "Modelview");

	glUniformMatrix4fv(modelviewUniform, 1, 0, &zRotation[0]);
}

void RenderingEngine2::Render() const
{
	GLuint positionSlot = glGetAttribLocation(m_simpleProgram, "vPosition");
	GLuint colorSlot = glGetAttribLocation(m_simpleProgram, "SourceColor");

    mat4 rotation = mat4::Rotate(m_rotationAngle);
    mat4 scale = mat4::Scale(m_scale);
    mat4 translation = mat4::Translate(0, 0, -7);

    // Set the model-view matrix.
    GLint modelviewUniform = glGetUniformLocation(m_simpleProgram, "Modelview");

    mat4 modelviewMatrix = scale * rotation * translation;

	GLsizei stride = sizeof(Vertex);
	const GLvoid* colorOffset = (GLvoid*) sizeof(vec3);

	// Clear the color buffer
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUniformMatrix4fv(modelviewUniform, 1, 0, modelviewMatrix.Pointer());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

	glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, stride, colorOffset);

	glEnableVertexAttribArray(positionSlot);
	glEnableVertexAttribArray(colorSlot);

	const GLvoid* bodyOffset = 0;
	const GLvoid* diskOffset = (GLvoid*) m_bodyIndexCount;

	glDrawElements(GL_TRIANGLES, m_bodyIndexCount, GL_UNSIGNED_BYTE, bodyOffset);

	glDisableVertexAttribArray(colorSlot);
	glVertexAttrib4f(colorSlot, 1, 1, 1, 1);
	glDrawElements(GL_TRIANGLES, m_diskIndexCount, GL_UNSIGNED_BYTE, diskOffset);

	glDisableVertexAttribArray(positionSlot);
}


GLuint RenderingEngine2::BuildProgram(const char* vertexShaderSource, const char* fragmentShaderSource) const {
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

GLuint RenderingEngine2::BuildShader(const char* source, GLenum shaderType) const {
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &source, 0);
    glCompileShader(shaderHandle);

    GLint  compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);

    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        std::cout << messages;
        exit(1);
    }

    return shaderHandle;
}

