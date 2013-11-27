#include "Interfaces.hpp"
#include "ParametricEquations.hpp"
#include <algorithm>

using namespace std;

// # of menu bar contents that select type of shapes to be render.
static const int SurfaceCount = 6;
static const int ButtonCount = SurfaceCount - 1;

class ApplicationEngine : public IApplicationEngine {
public:
    ApplicationEngine(IRenderingEngine* renderingEngine);
    ~ApplicationEngine();
    void Initialize(int width, int height);
    void OnFingerUp(ivec2 location);
    void OnFingerDown(ivec2 location);
    void OnFingerMove(ivec2 oldLocation, ivec2 newLocation);
    void Render() const;
    void UpdateAnimation(float dt);

private:
	void PopulateVisuals(Visual* visuals) const;
	int MapToButton(ivec2 touchpoint) const;
    vec3 MapToSphere(ivec2 touchpoint) const;
    float m_trackballRadius;
    ivec2 m_screenSize;
    ivec2 m_centerPoint;
    ivec2 m_fingerStart;
    bool m_spinning;
    Quaternion m_orientation;
    Quaternion m_previousOrientation;
    IRenderingEngine* m_renderingEngine;
	int m_currentSurface;
	ivec2 m_buttonSize;
	int m_pressedButton;
	int m_buttonSurfaces[ButtonCount];
};

IApplicationEngine* AppEngineInstance()
{
	static ApplicationEngine App(ES2::CreateRenderingEngine());

	return &App;
}

ApplicationEngine::ApplicationEngine(IRenderingEngine* renderingEngine) :
    m_spinning(false),
    m_renderingEngine(renderingEngine),
	m_pressedButton(-1)
{
	m_buttonSurfaces[0] = 0;
	m_buttonSurfaces[1] = 1;
	m_buttonSurfaces[2] = 2;
	m_buttonSurfaces[3] = 4;
	m_buttonSurfaces[4] = 5;
	m_currentSurface = 3;
}

ApplicationEngine::~ApplicationEngine()
{
	delete m_renderingEngine;
}

void ApplicationEngine::Initialize(int width, int height)
{
    m_trackballRadius = width / 3;
	m_buttonSize.y = height / 10;
	m_buttonSize.x = 4 * m_buttonSize.y / 3;
    m_screenSize = ivec2(width, height - m_buttonSize.y);
    m_centerPoint = m_screenSize / 2;

    vector<ISurface*> surfaces(SurfaceCount);
    surfaces[0] = new Cone(3, 1);
    surfaces[1] = new Sphere(1.4f);
    surfaces[2] = new Torus(1.4, 0.3);
    surfaces[3] = new TrefoilKnot(1.8f);
    surfaces[4] = new KleinBottle(0.2f);
    surfaces[5] = new MobiusStrip(1);
    m_renderingEngine->Initialize(surfaces);

    for (int i = 0; i < SurfaceCount; i++)
        delete surfaces[i];
}

void ApplicationEngine::PopulateVisuals(Visual* visuals) const
{
	for (int buttonIndex = 0; buttonIndex < ButtonCount; buttonIndex++) {
		int visualIndex = m_buttonSurfaces[buttonIndex];
		visuals[visualIndex].Color = vec3(0.75f, 0.75f, 0.75f);
		if (m_pressedButton == buttonIndex)
			visuals[visualIndex].Color = vec3(1, 1, 1);

		visuals[visualIndex].ViewportSize = m_buttonSize;
		visuals[visualIndex].LowerLeft.x = buttonIndex * m_buttonSize.x;
		visuals[visualIndex].LowerLeft.y = 0;
		visuals[visualIndex].Orientation = Quaternion();
	}

	visuals[m_currentSurface].Color = m_spinning ? vec3(1,1,1) : vec3(0,1,1);
	visuals[m_currentSurface].LowerLeft = ivec2(0, 48);
	visuals[m_currentSurface].Orientation = m_orientation;
}

void ApplicationEngine::Render() const
{
	vector<Visual> visuals(SurfaceCount);
	PopulateVisuals(&visuals[0]);
    m_renderingEngine->Render(visuals);
}

void ApplicationEngine::UpdateAnimation(float dt)
{
}

void ApplicationEngine::OnFingerUp(ivec2 location)
{
    m_spinning = false;

	if (m_pressedButton != -1 && m_pressedButton == MapToButton(location))
		swap(m_buttonSurfaces[m_pressedButton], m_currentSurface);

	m_pressedButton = -1;
}

void ApplicationEngine::OnFingerDown(ivec2 location)
{
    m_fingerStart = location;
    m_previousOrientation = m_orientation;
	m_pressedButton = MapToButton(location);
	if (m_pressedButton == -1)
		m_spinning = true;
}

void ApplicationEngine::OnFingerMove(ivec2 oldLocation, ivec2 location)
{
    if (m_spinning) {
        vec3 start = MapToSphere(m_fingerStart);
        vec3 end = MapToSphere(location);
        Quaternion delta = Quaternion::CreateFromVectors(start, end);
        m_orientation = delta.Rotated(m_previousOrientation);
    }

	if (m_pressedButton != -1 && m_pressedButton != MapToButton(location))
		m_pressedButton = -1;
}

int ApplicationEngine::MapToButton(ivec2 touchpoint) const
{
	// button section 보다 위에 있는가?
	if (touchpoint.y < m_screenSize.y - m_buttonSize.y)
		return -1;

	int buttonIndex = touchpoint.x / m_buttonSize.x;
	if (buttonIndex >= ButtonCount)
		return -1;

	return buttonIndex;
}

vec3 ApplicationEngine::MapToSphere(ivec2 touchpoint) const
{
    vec2 p = touchpoint - m_centerPoint;

    // 픽셀 좌표는 아래쪽으로 갈수록 값이 증가하므로, y 값의 부호를 바꾼다.
    p.y = -p.y;

    const float radius = m_trackballRadius;
    const float safeRadius = radius - 1;

    if (p.Length() > safeRadius) {
        float theta = atan2(p.y, p.x);
        p.x = safeRadius * cos(theta);
        p.y = safeRadius * sin(theta);
    }

    float z = sqrt(radius * radius - p.LengthSquared());
    vec3 mapped = vec3(p.x, p.y, z);
    return mapped / radius;
}

