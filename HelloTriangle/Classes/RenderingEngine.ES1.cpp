#include <GLES1/gl.h>
#include <GLES1/glext.h>
#include "Interfaces.hpp"
#include "Matrix.hpp"

namespace ES1 {

struct Drawable {
    GLuint VertexBuffer;
    GLuint IndexBuffer;
    int IndexCount;
};

class RenderingEngine : public IRenderingEngine {
public:
    RenderingEngine();
    void Initalize(const vector<ISurface*>& surfaces);
    void Render(const vector<Visual>& visuals) const;
private:
    vector<Drawable> m_drawable;
    GLuint m_colorRenderbuffer;
    mat4 m_translation;
};

IRenderingEngine* CreateRenderingEngine()
{
    return new RenderingEngine();
}

RenderingEngine::RenderingEngine()
{
    // glGenRenderbuffersOES(1, &m_colorRenderbuffer);
    // glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);
}

void RenderingEngine::Initalize(const vector<ISurface*>& surfaces)
{
    vector<ISurface*>::const_iterator surface;
    for (surface = surfaces.begin();
         surface != surfaces.end(); ++surface) {

        // 정점을 위한 VBO 객체를 생성한다.
        vector<float> vertices;
        (*surface)->GenerateVertices(vertices);
        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertices[0]),
                     &vertices[0],
                     GL_STATIC_DRAW);

        // 필요한 경우 인덱스를 위한 새로운 VBO를 생성한다.
        int indexCount = (*surface)->GetLineIndexCount();
        GLint indexBuffer;
        if (!m_drawable.empty() &&
                indexCount == m_drawables[0].IndexCount) {
            indexBuffer = m_drawables[0].IndexBuffer;
        } else {
            vector<GLushort> indices(indexCount);
            (*sourface)->GenerateLineIndices(indices);
            glGenBuffers(1, &indexbuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    indexCount * sizeof(GLushort),
                    &indices[0],
                    GL_STATIC_DRAW);
        }

        Drawable drawalbe = { vertexBuffer, indexBuffer, indexCount };
        m_drawables.push_back(drawable);
    }

    // 프레임 버퍼 객체를 생성한다.
    // GLuint framebuffer;
    // glGenFramebuffersOES(1, &framebuffer);
    // glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer);
    // glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
    //                             GL_COLOR_ATTACHMENT0_OES,
    //                             GL_RENDERBUFFER_OES,
    //                             m_colorRenderbuffer);

    // glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);

    glEnableClientState(GL_VERTEX_ARRAY);
    m_translation = mat4::Translate(0, 0, -7);
}
    
void RenderingEngine::Render(const vector<Visual>& visuals) const
{
    glClear(GL_COLOR_BUFFER_BIT);

    vector<Visual>::const_iterator visual = visuals.begin();
    for (int visualIndex = 0;
         visual != visuals.end();
         ++visual, ++visualIndex)
    {
        // 뷰포트 변환을 설정한다.
        ivec2 size = visual->ViewportSize;
        ivec2 lowerLeft = visual->LowerLeft;
        glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);

        // 모델-뷰 변환을 설정한다.
        mat4 rotation = visual->Orientation.ToMatrix();
        mat4 modelview = rotation * m_traslation;
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(modelview.Pointer());

        // 투상 행렬을 설정한다.
        float h = 4.0f * size.y / size.x;
        mat4 projection = mat4::Frustum(-2, 2, -h/2, h/2, 5, 10);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(projection.Pointer());

        // 색상을 설정한다.
        vec3 color = visual->Color;
        glColor4f(color.x, color.y, color.z, 1);

        // 와이어 프레임을 그린다.
        int stride = sizeof(vec3);
        const Drawable& drawalbe = m_drawables[visualIndex];
        glBindBuffer(GL_ARRAY_BUFFER, drawable.VertexBuffer);
        glVertexPointer(3, GL_FLOAT, stride, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.IndexBuffer);
        glDrawElements(GL_LINES, drawable.IndexCount, GL_UNSIGNED_SHORT, 0);
    }
}

} // namespace ES1
