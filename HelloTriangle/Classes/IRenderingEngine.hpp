// Physical orientation of a handheld device, equivalent to UIDeviceOrientation
enum DeviceOrientation {
    DeviceOrientationUnknown,
    DeviceOrientationPortrait,
    DeviceOrientationPortraitUpsideDown,
    DeviceOrientationLandscapeLeft,
    DeviceOrientationLandscapeRight,
    DeviceOrientationFaceUp,
    DeviceOrientationFaceDown,
};

// Create renderer object, inistalize OpenGL Status
struct IRenderingEngine* CreateRenderer1();
struct IRenderingEngine* CreateRenderer2();
struct IRenderingEngine* RenderingEngine();

// OpenGL ES Renderer interface. Used by GLView class
struct IRenderingEngine {
	virtual void Initialize(int width, int height) = 0;
	virtual void Render() const = 0;
	virtual void UpdateAnimation(float timeStep) = 0;
	virtual void OnRotate(DeviceOrientation newOrientation) = 0;
	virtual ~IRenderingEngine() {}
};
