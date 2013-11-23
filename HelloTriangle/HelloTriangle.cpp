//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// Hello_Triangle.c
//
//    This is a simple example that draws a single triangle with
//    a minimal vertex/fragment shader.  The purpose of this 
//    example is to demonstrate the basic concepts of 
//    OpenGL ES 2.0 rendering.
#include <stdlib.h>
#include "esUtil.h"
#include "Classes/IRenderingEngine.hpp"

void Draw (ESContext* esContext)
{
    RenderingEngine()->Render();
	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

int main ( int argc, char *argv[] )
{
   ESContext esContext;
   UserData  userData;

   esInitContext ( &esContext );
   esContext.userData = &userData;

   esCreateWindow ( &esContext, TEXT("Hello Triangle"), 320, 240, ES_WINDOW_RGB );
   
   if (!RenderingEngine()->Initialize(esContext.width, esContext.height))
      return 0;

   esRegisterDrawFunc ( &esContext, Draw );
   
   esMainLoop ( &esContext );
}

