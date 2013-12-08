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
    void Initialize(const vector<ISurface*>& surfaces);
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

void RenderingEngine::Initialize(const vector<ISurface*>& surfaces)
{
    vector<ISurface*>::const_iterator surface;
    for (surface = surfaces.begin();
         surface != surfaces.end(); ++surface) {
        // Create the VBO for the vertices.
        vector<float> vertices;
        (*surface)->GenerateVertices(vertices, VertexFlagsNormals);
        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(vertices[0]),
                     &vertices[0],
                     GL_STATIC_DRAW);

        // Create a new VBO for the indices if needed.
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

    // 여러 GL상태를 설정한다.
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    // 물체 속성을 설정한다.
    vec4 specular(0.5f, 0.5f, 0.5f, 1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.Pointer());
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);

    m_translation = mat4::Translate(0, 0, -7);
}
    
void RenderingEngine::Render(const vector<Visual>& visuals) const
{
    glClearColor(0.5f, 0.5f, 0.5f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    vector<Visual>::const_iterator visual = visuals.begin();
    for (int visualIndex = 0;
         visual != visuals.end();
         ++visual, ++visualIndex)
    {
        // 뷰포트 변환을 설정한다.
        ivec2 size = visual->ViewportSize;
        ivec2 lowerLeft = visual->LowerLeft;
        glViewport(lowerLeft.x, lowerLeft.y, size.x, size.y);

        // 조명 위치를 설정한다.
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        vec4 lightPosition(0.25, 0.25, 1, 0);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition.Pointer());

        // 모델-뷰 변환을 설정한다.
        mat4 rotation = visual->Orientation.ToMatrix();
        mat4 modelview = rotation * m_traslation;
        glLoadMatrixf(modelview.Pointer());

        // 투상 행렬을 설정한다.
        float h = 4.0f * size.y / size.x;
        mat4 projection = mat4::Frustum(-2, 2, -h/2, h/2, 5, 10);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(projection.Pointer());

        // 확산 색상을 설정한다.
        vec3 color = visual->Color * 0.75f;
        vec4 diffuse(color.x, color.y, color.z, 1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.Pointer());

        // 표면을 그린다.
        int stride = 2 * sizeof(vec3);
        const Drawable& drawalbe = m_drawables[visualIndex];
        glBindBuffer(GL_ARRAY_BUFFER, drawable.VertexBuffer);
        glVertexPointer(3, GL_FLOAT, stride, 0);
        const GLvoid* normalOffset = (const GLvoid*) sizeof(vec3);
        glNormalPointer(GL_FLOAT, stride, normalOffset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.IndexBuffer);
        glDrawElements(GL_LINES, drawable.IndexCount, GL_UNSIGNED_SHORT, 0);
    }
}

} // namespace ES1
