/*
 *  RenderingEngine2.cpp
 *  HelloArrow
 *
 *  Created by Kristof Vannotten on 2/10/10.
 *  Copyright 2010 Kristof Projects. All rights reserved.
 *
 */

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include "IRenderingEngine.hpp"

#define STRINGIFY(A) #A

#include "../Shaders/Simple.vert"
#include "../Shaders/Simple.frag"


static const float RevolutionPerSecond = 1;

class RenderingEngine2: public IRenderingEngine {
public:
    RenderingEngine2();
    int Initialize(int width, int height);
    void Render() const;

private:
	GLuint BuildProgram(const char* vertexShaderSource, const char* fragmentShaderSource) const;
	GLuint BuildShader(const char* source, GLenum shaderType) const;

    GLuint m_simpleProgram;
	int mWidth;
	int mHeight;
};

IRenderingEngine* RenderingEngine() {
    static RenderingEngine2 engine;
    return &engine;
}

struct Vertex {
    float Position[2];
    float Color[4];
};

const Vertex Vertices[] = {
    {{-0.5, -0.866},        {1, 1, 0.5f, 1}},
    {{0.5, -0.866},         {1, 1, 0.5f, 1}},
    {{0, 1},                {1, 1, 0.5f, 1}},
    {{-0.5, -0.866},        {0.5f, 0.5f, 0.5f}},
    {{0.5, -0.866},         {0.5f, 0.5f, 0.5f}},
    {{0, -0.4f},            {0.5f, 0.5f, 0.5f}}
};

RenderingEngine2::RenderingEngine2() 
{
    // glGenRenderbuffers(1, &m_renderbuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer);
}

int RenderingEngine2::Initialize(int width, int height) 
{
   mWidth = width;
   mHeight = height;

    //create framebuffer object and attach the colour buffer
    /*
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderbuffer);
    */

   // Load the vertex/fragment shaders
   m_simpleProgram = BuildProgram(SimpleVertexShader, SimpleFragmentShader );

   // Bind vPosition to attribute 0   
   // glBindAttribLocation ( programObject, 0, "vPosition" );

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

   return TRUE;
}

void RenderingEngine2::Render() const
{
   GLfloat vVertices[] = {  0.0f,  0.5f, 
                           -0.5f, -0.5f,
                            0.5f, -0.5f };
      
   // Set the viewport
   glViewport ( 0, 0, mWidth, mHeight );
   
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( m_simpleProgram );

   GLuint positionSlot = glGetAttribLocation(m_simpleProgram, "vPosition");

   glEnableVertexAttribArray(positionSlot);

   GLsizei stride = sizeof(Vertex);
   const GLvoid* pCoords = &Vertices[0].Position[0];

   // Load the vertex data
   glVertexAttribPointer (positionSlot, 2, GL_FLOAT, GL_FALSE, stride, pCoords);

   GLsizei vertexCount = sizeof(Vertices) / sizeof(Vertex);

   glDrawArrays ( GL_TRIANGLES, 0, vertexCount);

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

