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

RenderingEngine2::RenderingEngine2() {}

int RenderingEngine2::Initialize(int width, int height) 
{
   const GLbyte vShaderStr[] =  
      "attribute vec4 vPosition;    \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = vPosition;  \n"
      "}                            \n";
   
   const GLbyte fShaderStr[] =  
      "precision mediump float;\n"\
      "void main()                                  \n"
      "{                                            \n"
      "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
      "}                                            \n";

   mWidth = width;
   mHeight = height;

   // Load the vertex/fragment shaders
   m_simpleProgram = BuildProgram((const char*)vShaderStr, (const char*)fShaderStr);

   // Bind vPosition to attribute 0   
   // glBindAttribLocation ( programObject, 0, "vPosition" );

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

   return TRUE;
}

void RenderingEngine2::Render() const
{
   GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f, 
                           -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f };
      
   // Set the viewport
   glViewport ( 0, 0, mWidth, mHeight );
   
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( m_simpleProgram );

   // Load the vertex data
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
   glEnableVertexAttribArray ( 0 );

   glDrawArrays ( GL_TRIANGLES, 0, 3 );
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

