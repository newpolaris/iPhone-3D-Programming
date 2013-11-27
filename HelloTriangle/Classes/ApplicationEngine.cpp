#include "Interfaces.hpp"
#include "ParametricEquations.hpp"

using namespace std;

// # of menu bar contents that select type of shapes to be render.
static const int SurfaceCount = 6;

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
    vec3 MapToSphere(ivec2 touchpoint) const;
    float m_trackballRadius;
    ivec2 m_screenSize;
    ivec2 m_centerPoint;
    ivec2 m_fingerStart;
    bool m_spinning;
    Quaternion m_orientation;
    Quaternion m_previousOrientation;
    IRenderingEngine* m_renderingEngine;
};

IApplicationEngine* CreateapplicationEngine(IRenderingEngine* renderingEngine)
{
    return new ApplicationEngine(renderingEngine);
}

ApplicationEngine::ApplicationEngine(IRenderingEngine* renderingEngine) :
    m_spinning(false),
    m_renderingEngine(renderingEngine)
{
}

void ApplicationEngine::Initialize(int width, int height)
{
    m_trackballRadius = width / 3;
    m_screenSize = ivec2(width, height);
    m_centerPoint = m_screenSize / 2;

    vector<ISurface*> surfaces(SurfaceCount);
    surfaces[0] = new Cone(3, 1);
    surfaces[1] = new Sphere(1.4f);
    surfaces[2] = new Torus(1.4, 0.3);
    surfaces[3] = new TrefoilKnot(1.8f);
    surfaces[4] = new KleinBottle(0.2f);
    surfaces[5] = new MobiusStrip(1);
    m_renderingEngine->Initialize(surfaces);

    for (int i = 0; i < SourfaceCount; i++)
        delete surfaces[i];
}

void ApplicationEngine::Render() const
{
    Visual visual;
    visual.Color = m_spinning ? vec3(1, 1, 1) : vec3(0, 1, 1);
    visual.LowerLeft = ivec2(0, 48);
    visual.ViewportSize = ivec2(320, 432);
    visual.Orientation = m_orientation;
    m_renderingEngine->Render(visual);
}

void ApplicationEngine::UpdateAnimation(float dt)
{
}

void ApplicationEngine::OnFingerUp(ivec2 location)
{
    m_spinning = false;
}

void ApplicationEngine::OnFingerDown(ivec2 location)
{
    m_fingerStart = location;
    m_previousOrientation = m_orientation;
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

