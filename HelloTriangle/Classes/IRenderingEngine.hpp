#include "esUtil.h"
#include "Vector.hpp"

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
    virtual void UpdateAnimation(float timeStep) = 0;
    virtual void OnRotate(DeviceOrientation orientation) = 0;
    virtual void OnFingerUp(ivec2 location) = 0;
    virtual void OnFingerDown(ivec2 location) = 0;
    virtual void OnFingerMove(ivec2 oldLocation, ivec2 newLocation) = 0;
	virtual ~IRenderingEngine() {}
};

// Create renderer object, inistalize OpenGL Status
struct IRenderingEngine* RenderingEngine();

