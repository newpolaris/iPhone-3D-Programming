#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

} UserData;

// OpenGL ES Renderer interface. Used by GLView class
struct IRenderingEngine {
	virtual int Initialize(int width, int height) = 0;
	virtual void Render() const = 0;
	virtual ~IRenderingEngine() {}
};

// Create renderer object, inistalize OpenGL Status
struct IRenderingEngine* RenderingEngine();

