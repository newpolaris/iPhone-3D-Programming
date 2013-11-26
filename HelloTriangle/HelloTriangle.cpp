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
    static float step = 0.0;
    step+=0.0002;

    RenderingEngine()->UpdateAnimation(step);
    RenderingEngine()->Render();
	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

void touchesBegin (ESContext* esContext, int x, int y)
{
    RenderingEngine()->OnFingerDown(ivec2(x, y));
    esLogMessage("B %d, %d\n", x, y);
}

void touchesEnded (ESContext* esContext, int x, int y)
{
    RenderingEngine()->OnFingerUp(ivec2(x, y));
    esLogMessage("E %d, %d\n", x, y);
}

/// \param: px, py previous touch point.
/// \param: nx, ny next(current) touch point.
void touchesMoved(ESContext* esContext, int px, int py, int nx, int ny)
{
    RenderingEngine()->OnFingerMove(ivec2(px, py), ivec2(nx, ny));
    esLogMessage("M %d, %d\n", nx, ny);
}

int main ( int argc, char *argv[] )
{
   ESContext esContext;
   UserData  userData;

   esInitContext ( &esContext );
   esContext.userData = &userData;

   esCreateWindow ( &esContext, TEXT("Hello Triangle"), 640, 480, ES_WINDOW_RGB );
   
   if (!RenderingEngine()->Initialize(esContext.width, esContext.height))
      return 0;

   esRegisterDrawFunc ( &esContext, Draw );
   esRegisterLeftButtonDownFunc( &esContext, touchesBegin );
   esRegisterLeftButtonUpFunc( &esContext, touchesEnded );
   esRegisterMouseDragFunc( &esContext, touchesMoved );
   
   RenderingEngine()->OnRotate(DeviceOrientationFaceUp);

   esMainLoop ( &esContext );
}

