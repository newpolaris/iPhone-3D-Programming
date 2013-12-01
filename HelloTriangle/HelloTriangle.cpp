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
#include "Classes/Vector.hpp"
#include "Classes/Interfaces.hpp"

void Draw (ESContext* esContext)
{
	AppEngineInstance()->UpdateAnimation(0.01f);
	AppEngineInstance()->Render();
	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

void touchesBegin (ESContext* esContext, int x, int y)
{
	AppEngineInstance()->OnFingerDown(ivec2(x, y));
}

void touchesEnded (ESContext* esContext, int x, int y)
{
	AppEngineInstance()->OnFingerUp(ivec2(x, y));
}

/// \param: px, py previous touch point.
/// \param: nx, ny next(current) touch point.
void touchesMoved(ESContext* esContext, int px, int py, int nx, int ny)
{
	AppEngineInstance()->OnFingerMove(ivec2(px, py), ivec2(nx, ny));
}

int main ( int argc, char *argv[] )
{
   ESContext esContext;

   esInitContext ( &esContext );

   esCreateWindow ( &esContext, TEXT("Hello Triangle"), 320, 480, ES_WINDOW_RGB );
   
   AppEngineInstance()->Initialize(esContext.width, esContext.height);

   esRegisterDrawFunc ( &esContext, Draw );
   esRegisterLeftButtonDownFunc( &esContext, touchesBegin );
   esRegisterLeftButtonUpFunc( &esContext, touchesEnded );
   esRegisterMouseDragFunc( &esContext, touchesMoved );
   
   esMainLoop ( &esContext );
}

