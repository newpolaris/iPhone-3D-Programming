#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

} UserData;

// Physical orientation of a handheld device; equivalent to UIDeviceOrientation
enum DeviceOrientation {
    DeviceOrientationUnknown,
    DeviceOrientationPortrait,
    DeviceOrientationPortraitUpsideDown,
    DeviceOrientationLandscapeLeft,
    DeviceOrientationLandscapeRight,
    DeviceOrientationFaceUp,
    DeviceOrientationFaceDown,
};
// OpenGL ES Renderer interface. Used by GLView class
struct IRenderingEngine {
	virtual int Initialize(int width, int height) = 0;
	virtual void Render() const = 0;
	virtual ~IRenderingEngine() {}
};

// Create renderer object, inistalize OpenGL Status
struct IRenderingEngine* RenderingEngine();

