#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "givr.h"

//------------------------------------------------------------------------------
// Start givr.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// END givr.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start renderer.cpp
//------------------------------------------------------------------------------

namespace givr {
    void allocateBuffers(RenderContext &ctx) {
        ctx.vao = std::make_unique<VertexArray>();
        ctx.vao->alloc();

        // Map - but don't upload indices data
        std::unique_ptr<Buffer> indices = std::make_unique<Buffer>();
        indices->alloc();
        ctx.arrayBuffers.push_back(std::move(indices));

        auto allocateBuffer = [&ctx]() {
            std::unique_ptr<Buffer> vbo = std::make_unique<Buffer>();
            vbo->alloc();
            ctx.arrayBuffers.push_back(std::move(vbo));
        };

        // Upload / bind / map model data
        allocateBuffer();//data.vertices);
        allocateBuffer();//data.normals);
        allocateBuffer();//data.uvs);
        allocateBuffer();//data.colours);
    }

    void uploadBuffers(
        RenderContext &ctx,
        BufferData const &data
    ) {
        // Start by setting the appropriate context variables for rendering.
        ctx.numberOfIndices = data.indices.size();
        ctx.startIndex = 0;
        ctx.endIndex =  data.vertices.size() / data.dimensions;

        std::uint16_t vaIndex = 4;
        ctx.vao->bind();

        std::unique_ptr<Buffer> &indices = ctx.arrayBuffers[0];
        indices->bind(GL_ELEMENT_ARRAY_BUFFER);
        indices->data(
                GL_ELEMENT_ARRAY_BUFFER,
                data.indices,
                getBufferUsageType(data.indicesType));

        std::uint16_t bufferIndex = 1;
        auto applyBuffer = [&ctx, &vaIndex, &bufferIndex](
            GLenum type,
            GLuint size,
            GLenum bufferType,
            std::string name,
            std::vector<float> const &data
        ) {
            // if this data piece is empty disable this one.
            std::unique_ptr<Buffer> &vbo = ctx.arrayBuffers[bufferIndex];
            vbo->bind(type);
            if (data.size() == 0) {
                glDisableVertexAttribArray(vaIndex);
            } else {
                glBindAttribLocation(*ctx.shaderProgram.get(), vaIndex, name.c_str());
                vbo->data(type, data, bufferType);
                glVertexAttribPointer(vaIndex, size, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                glEnableVertexAttribArray(vaIndex);
            }
            ++vaIndex;
            ++bufferIndex;
        };

        // Upload / bind / map model data
        applyBuffer(GL_ARRAY_BUFFER, data.dimensions, getBufferUsageType(data.verticesType), "position", data.vertices);
        applyBuffer(GL_ARRAY_BUFFER, data.dimensions, getBufferUsageType(data.normalsType), "normals", data.normals);
        applyBuffer(GL_ARRAY_BUFFER, 2, getBufferUsageType(data.uvsType), "uvs", data.uvs);
        applyBuffer(GL_ARRAY_BUFFER, 3, getBufferUsageType(data.coloursType), "colour", data.colours);

        ctx.vao->unbind();

        ctx.arrayBuffers[0]->unbind(GL_ELEMENT_ARRAY_BUFFER);
        if (ctx.arrayBuffers.size() > 1) {
            ctx.arrayBuffers[1]->unbind(GL_ARRAY_BUFFER);
        }
    }
};// end namespace givr
//------------------------------------------------------------------------------
// END renderer.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start cube.cpp
//------------------------------------------------------------------------------

std::vector<float> givr::generate_vertices(cube const &) {
    return std::vector<float>{
        -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,
        1.0,  1.0,  1.0,
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        -1.0,  1.0, -1.0,
        1.0,  1.0, -1.0
    };
}

std::vector<std::uint32_t> givr::generate_indices(cube const &) {
    return std::vector<std::uint32_t>{
        0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
    };
}
//------------------------------------------------------------------------------
// END cube.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start buffer_data.cpp
//------------------------------------------------------------------------------

using BufferData = givr::BufferData;
using vec3f = givr::vec3f;

void BufferData::addIndices(std::vector<GLuint> const &newIndices) {
    indices.insert(indices.end(), newIndices.begin(), newIndices.end());
}
void BufferData::addVertices(std::vector<float> const &newVertices) {
    vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
}
void BufferData::addVertices(std::vector<vec3f> const &newVertices) {
    vertices.resize(newVertices.size()*3);
    for (size_t i = 0; i < newVertices.size(); ++i) {
        vertices[(i*3)] = newVertices[i][0];
        vertices[(i*3)+1] = newVertices[i][1];
        vertices[(i*3)+2] = newVertices[i][2];
    }
}
void BufferData::addNormals(std::vector<float> const &newNormals) {
    normals.insert(normals.end(), newNormals.begin(), newNormals.end());
}
void BufferData::addNormals(std::vector<vec3f> const &newNormals) {
    normals.resize(newNormals.size()*3);
    for (size_t i = 0; i < newNormals.size(); ++i) {
        normals[(i*3)] = newNormals[i][0];
        normals[(i*3)+1] = newNormals[i][1];
        normals[(i*3)+2] = newNormals[i][2];
    }
}
void BufferData::addUvs(std::vector<float> const &newUvs) {
    uvs.insert(uvs.end(), newUvs.begin(), newUvs.end());
}
/*TODO: void BufferData::addUvs(std::vector<vec3f> const &newUvs) {
    uvs.reserve(newUvs.size()*2);
    for (size_t i = 0; i < uvs.size(); ++i) {
        uvs[(i*3)] = newUvs[i][0];
        uvs[(i*3)+1] = newUvs[i][1];
    }
}*/
void BufferData::addColours(std::vector<float> const &newColours) {
    colours.insert(colours.end(), newColours.begin(), newColours.end());
}
void BufferData::addColours(std::vector<vec3f> const &newColours) {
    colours.resize(newColours.size()*3);
    for (size_t i = 0; i < newColours.size(); ++i) {
        colours[(i*3)] = newColours[i][0];
        colours[(i*3)+1] = newColours[i][1];
        colours[(i*3)+2] = newColours[i][2];
    }
}
//------------------------------------------------------------------------------
// END buffer_data.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start draw.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// END draw.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start types.cpp
//------------------------------------------------------------------------------

using PrimitiveType = givr::PrimitiveType;
using BufferUsageType = givr::BufferUsageType;

GLenum givr::getMode(PrimitiveType const &t) {
    switch(t) {
        case PrimitiveType::POINTS:
            return GL_POINTS;
        case PrimitiveType::LINES:
            return GL_LINES;
        case PrimitiveType::LINE_LOOP:
            return GL_LINE_LOOP;
        case PrimitiveType::LINE_STRIP:
            return GL_LINE_STRIP;
        case PrimitiveType::TRIANGLES:
            return GL_TRIANGLES;
        case PrimitiveType::TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case PrimitiveType::TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
        case PrimitiveType::LINES_ADJACENCY:
            return GL_LINES_ADJACENCY;
        case PrimitiveType::LINE_STRIP_ADJACENCY:
            return GL_LINE_STRIP_ADJACENCY;
        case PrimitiveType::TRIANGLES_ADJACENCY:
            return GL_TRIANGLES_ADJACENCY;
        case PrimitiveType::TRIANGLE_STRIP_ADJACENCY:
            return GL_TRIANGLE_STRIP_ADJACENCY;
        default:
            return GL_TRIANGLES;
    }
}

GLenum givr::getBufferUsageType(BufferUsageType const &d) {
    switch(d) {
        case BufferUsageType::STATIC_DRAW:
            return GL_STATIC_DRAW;
        case BufferUsageType::DYNAMIC_DRAW:
            return GL_DYNAMIC_DRAW;
        default:
            return GL_STATIC_DRAW;
    }

}
//------------------------------------------------------------------------------
// END types.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start instanced_renderer.cpp
//------------------------------------------------------------------------------

namespace givr {
    void allocateBuffers(InstancedRenderContext &ctx) {
        ctx.vao = std::make_unique<VertexArray>();
        ctx.vao->alloc();

        // Map - but don't upload framing data.
        ctx.modelTransformsBuffer = std::make_unique<Buffer>();
        ctx.modelTransformsBuffer->alloc();

        // Map - but don't upload indices data
        std::unique_ptr<Buffer> indices = std::make_unique<Buffer>();
        indices->alloc();
        ctx.arrayBuffers.push_back(std::move(indices));

        auto allocateBuffer = [&ctx]() {
            std::unique_ptr<Buffer> vbo = std::make_unique<Buffer>();
            vbo->alloc();
            ctx.arrayBuffers.push_back(std::move(vbo));
        };

        // bind / map model data
        allocateBuffer();//data.vertices);
        allocateBuffer();//data.normals);
        allocateBuffer();//data.uvs);
        allocateBuffer();//data.colours);
    }

    void uploadBuffers(
        InstancedRenderContext &ctx,
        BufferData const &data
    ) {
        // Start by setting the appropriate context variables for rendering.
        ctx.numberOfIndices = data.indices.size();
        ctx.startIndex = 0;
        ctx.endIndex =  data.vertices.size() / data.dimensions;

        std::uint16_t vaIndex = 0;
        ctx.vao->bind();

        // Upload framing data.
        ctx.modelTransformsBuffer->bind(GL_ARRAY_BUFFER);
        auto vec4Size = sizeof(mat4f)/4;
        for (std::uint16_t i = 0; i < 4; ++i) {
            glVertexAttribPointer(vaIndex, 4, GL_FLOAT, GL_FALSE, sizeof(mat4f), (GLvoid*)(i*vec4Size));
            glEnableVertexAttribArray(vaIndex);
            glVertexAttribDivisor(vaIndex, 1);
            ++vaIndex;
        }

        std::unique_ptr<Buffer> &indices = ctx.arrayBuffers[0];
        indices->bind(GL_ELEMENT_ARRAY_BUFFER);
        indices->data(
                GL_ELEMENT_ARRAY_BUFFER,
                data.indices,
                getBufferUsageType(data.indicesType));

        std::uint16_t bufferIndex = 1;
        auto applyBuffer = [&ctx, &vaIndex, &bufferIndex](
            GLenum type,
            GLuint size,
            GLenum bufferType,
            std::string name,
            std::vector<float> const &data
        ) {
            std::unique_ptr<Buffer> &vbo = ctx.arrayBuffers[bufferIndex];
            vbo->bind(type);
            if (data.size() == 0) {
                glDisableVertexAttribArray(vaIndex);
            } else {
                vbo->data(type, data, bufferType);
                glBindAttribLocation(*ctx.shaderProgram.get(), vaIndex, name.c_str());
                glVertexAttribPointer(vaIndex, size, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                glEnableVertexAttribArray(vaIndex);
            }
            ++vaIndex;
            ++bufferIndex;
        };

        // Upload / bind / map model data
        applyBuffer(GL_ARRAY_BUFFER, data.dimensions, getBufferUsageType(data.verticesType), "position", data.vertices);
        applyBuffer(GL_ARRAY_BUFFER, data.dimensions, getBufferUsageType(data.normalsType), "normals", data.normals);
        applyBuffer(GL_ARRAY_BUFFER, 2, getBufferUsageType(data.uvsType), "uvs", data.uvs);
        applyBuffer(GL_ARRAY_BUFFER, 3, getBufferUsageType(data.coloursType), "colour", data.colours);

        ctx.vao->unbind();

        ctx.arrayBuffers[0]->unbind(GL_ELEMENT_ARRAY_BUFFER);
        if (ctx.arrayBuffers.size() > 1) {
            ctx.arrayBuffers[1]->unbind(GL_ARRAY_BUFFER);
        }

    }
};// end namespace givr
//------------------------------------------------------------------------------
// END instanced_renderer.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start turntable.cpp
//------------------------------------------------------------------------------

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <iostream>

using mat4f = givr::mat4f;
using vec3f = givr::vec3f;
using TurnTableCamera = givr::camera::TurnTableCamera;

float angle_to_x(float r, float theta, float phi) {
    return r * sin(theta) * sin(phi);
}
float angle_to_y(float r, float phi) {
    return r * cos(phi);
}
float angle_to_z(float r, float theta, float phi) {
    return r * cos(theta) * sin(phi);
}


mat4f TurnTableCamera::viewMatrix() const {
    vec3f camera_position{
        angle_to_x(zoom(), longitude(), latitude()),
        angle_to_y(zoom(), latitude()),
        angle_to_z(zoom(), longitude(), latitude())
    };
    camera_position += translation();
    vec3f up{ 0., 1., 0. };
    vec3f binormal = cross(camera_position, up);
    up = glm::normalize(cross(binormal, camera_position));

    return glm::lookAt(camera_position, translation(), up);
}

vec3f TurnTableCamera::viewPosition() const {
    return vec3f{
        angle_to_x(zoom(), longitude(), latitude()),
        angle_to_y(zoom(), latitude()),
        angle_to_z(zoom(), longitude(), latitude()) };
}

void TurnTableCamera::rotateAroundXPercent(float perc) {
    rotateAroundX(perc * LONGITUDE_MAX);
}
void TurnTableCamera::rotateAroundYPercent(float perc) {
    rotateAroundY(perc * LATITUDE_MAX);
}

void TurnTableCamera::rotateAroundX(float angle) {
    longitude() += angle;
}
void TurnTableCamera::rotateAroundY(float angle) {
    latitude() = std::fmin(std::fmax(latitude() + angle, 0.001f), M_PI - 0.001f);
}
void TurnTableCamera::zoom(float amount) {
    zoom() += amount;
    zoom() = std::fmax(0, zoom());
}
void TurnTableCamera::translate(vec3f amount) {
    translation() += amount;
}

//------------------------------------------------------------------------------
// END turntable.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start perspective.cpp
//------------------------------------------------------------------------------

#include <glm/gtc/matrix_transform.hpp>

using PerspectiveProjection = givr::camera::PerspectiveProjection;

givr::mat4f PerspectiveProjection::projectionMatrix() const {
    return glm::perspective(
        glm::radians(fieldOfViewY()),
        aspectRatio(),
        nearDistance(),
        farDistance()
    );
}

void PerspectiveProjection::updateAspectRatio(int width, int height) {
    float w = static_cast<float>(width);
    float h = static_cast<float>(height);

    set(AspectRatio(w / h));
}
//------------------------------------------------------------------------------
// END perspective.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start orthographic.cpp
//------------------------------------------------------------------------------

#include <glm/gtc/matrix_transform.hpp>

using OrthographicProjection = givr::camera::OrthographicProjection;
using mat4f = givr::mat4f;

mat4f OrthographicProjection::projectionMatrix() const {
    // TODO: This assumes a view centred on the origin
    auto ar = aspectRatio();
    if (ar < 1) {
        // taller than we are wide
        return glm::ortho(
            left(),
            right(),
            bottom() / ar,
            top() / ar,
            nearDistance(),
            farDistance()
        );
    } else {
        // Wider than we are tall
        return glm::ortho(
            left() * ar,
            right() * ar,
            bottom(),
            top(),
            nearDistance(),
            farDistance()
        );
    }
}
void OrthographicProjection::updateAspectRatio(int width, int height) {
    float w = static_cast<float>(width);
    float h = static_cast<float>(height);

    set(AspectRatio(w / h));
}
//------------------------------------------------------------------------------
// END orthographic.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start phong.cpp
//------------------------------------------------------------------------------

template<typename ColorSrc>
using prc = givr::style::T_PhongRenderContext<ColorSrc>;
template<typename ColorSrc>
using pirc = givr::style::T_PhongInstancedRenderContext<ColorSrc>;
using namespace givr::style;

std::string givr::style::phongVertexSource(std::string modelSource, bool usingTexture, bool hasNormals, bool hasColours) {
    return
        "#version 330 core\n" +
        std::string(usingTexture ? "#define USING_TEXTURE\n" : "") +
        std::string(hasNormals ? "#define HAS_NORMALS\n" : "") +
        std::string(hasColours ? "#define HAS_COLOURS\n" : "") +
        modelSource +
        std::string(R"shader( mat4 model;
        layout(location=4) in vec3 position;
        #ifdef HAS_NORMALS
            layout(location=5) in vec3 normal;
        #endif
        #ifdef USING_TEXTURE
            layout(location=6) in vec2 uvs;
        #endif
        #ifdef HAS_COLOURS
            layout(location=7) in vec3 colour;
        #endif

        uniform mat4 view;
        uniform mat4 projection;

        #ifdef HAS_NORMALS
            out vec3 geomNormal;
        #endif
        out vec3 geomOriginalPosition;
        #ifdef USING_TEXTURE
            out vec2 geomUv;
        #endif
        #ifdef HAS_COLOURS
            out vec3 geomColour;
        #endif

        void main(){
            mat4 mv = view * model;
            mat4 mvp = projection * mv;
            gl_Position = mvp * vec4(position, 1.0);
            geomOriginalPosition = vec3(model * vec4(position, 1.0));
            #ifdef HAS_NORMALS
                geomNormal = vec3(model*vec4(normal, 0));
            #endif
            #ifdef HAS_COLOURS
                geomColour = colour;
            #endif
            #ifdef USING_TEXTURE
                geomUv = uvs;
            #endif
        }

        )shader"
    );
}
std::string givr::style::phongGeometrySource(bool usingTexture, bool hasNormals, bool hasColours) {
    return
        "#version 330 core\n" +
        std::string(usingTexture ? "#define USING_TEXTURE\n" : "") +
        std::string(hasNormals ? "#define HAS_NORMALS\n" : "") +
        std::string(hasColours ? "#define HAS_COLOURS\n" : "") +
        std::string(R"shader(
        layout (triangles) in;
        layout (triangle_strip, max_vertices = 3) out;

        uniform bool generateNormals;

        #ifdef HAS_NORMALS
            in vec3 geomNormal[];
        #endif
        in vec3 geomOriginalPosition[];
        #ifdef USING_TEXTURE
            in vec2 geomUv[];
        #endif
        #ifdef HAS_COLOURS
            in vec3 geomColour[];
        #endif

        out vec3 fragNormal;
        out vec3 originalPosition;
        out vec2 fragUv;
        #ifdef HAS_COLOURS
            out vec3 fragColour;
        #endif
        out vec3 fragBarycentricCoords;

        void main() {
            vec3 normal;
            if (generateNormals) {
                normal =
                    cross(
                        vec3(gl_in[1].gl_Position - gl_in[0].gl_Position),
                        vec3(gl_in[2].gl_Position - gl_in[0].gl_Position)
                    );
            }

            for(int i = 0; i < 3; i++) {
                gl_Position = gl_in[i].gl_Position;
                if (!generateNormals) {
                    #ifdef HAS_NORMALS
                        fragNormal = geomNormal[i];
                    #endif
                } else {
                    fragNormal = normal;
                }
                originalPosition = geomOriginalPosition[i];
                #ifdef USING_TEXTURE
                    fragUv = geomUv[i];
                #endif
                #ifdef HAS_COLOURS
                    fragColour = geomColour[i];
                #endif
                fragBarycentricCoords = vec3(0.0, 0.0, 0.0);
                fragBarycentricCoords[i] = 1.0;
                EmitVertex();
            }

            EndPrimitive();
        }

        )shader"
        );
}

// Using wireframe technique from:
// http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/
std::string givr::style::phongFragmentSource(bool usingTexture, bool hasColours) {
    return
        "#version 330 core\n" +
        std::string(usingTexture ? "#define USING_TEXTURE\n" : "") +
        std::string(hasColours ? "#define HAS_COLOURS\n" : "") +
        std::string(R"shader(
        #define M_PI 3.1415926535897932384626433832795

        uniform vec3 colour;
        uniform bool perVertexColour;
        uniform vec3 lightPosition;
        uniform float ambientFactor;
        uniform float specularFactor;
        uniform float phongExponent;
        uniform vec3 viewPosition;
        uniform bool showWireFrame;
        uniform vec3 wireFrameColour;
        uniform float wireFrameWidth;


        float edgeFactor(vec3 vBC){
            vec3 d = fwidth(vBC);
            vec3 a3 = smoothstep(vec3(0.0), d*wireFrameWidth, vBC);
            return min(min(a3.x, a3.y), a3.z);
        }

        #ifdef USING_TEXTURE
            uniform sampler2D colorTexture;
            in vec2 fragUv;

            vec3 getColor(){ return texture(colorTexture, fragUv).rgb; }
        #else
            vec3 getColor(){ return colour; }
        #endif

        in vec3 fragNormal;
        in vec3 originalPosition;
        #ifdef HAS_COLOURS
            in vec3 fragColour;
        #endif
        in vec3 fragBarycentricCoords;

        out vec4 outColour;

        void main()
        {
            vec3 finalColour = getColor();
            #ifdef HAS_COLOURS
                if (perVertexColour) {
                    finalColour = fragColour;
                }
            #endif

            // ambient
            vec3 ambient = ambientFactor * finalColour;

            // diffuse
            vec3 lightDirection = normalize(lightPosition - originalPosition);
            vec3 normal = normalize(fragNormal);
            if (!gl_FrontFacing) normal = -normal;
            float diff = max(dot(lightDirection, normal), 0.0);
            vec3 diffuse = diff * finalColour;

            // specular
            vec3 viewDirection = normalize(viewPosition - originalPosition);
            vec3 reflectDirection = normalize(2.0*dot(lightDirection, normal)*normal - lightDirection);
            float normalization = (phongExponent+2.0)/(2.0*M_PI);
            float spec = normalization*diff*pow(max(dot(viewDirection, reflectDirection), 0.0), phongExponent);
            vec3 specular = vec3(specularFactor) * spec; // assuming bright white light colour

            vec3 shadedColour = ambient + diffuse + specular;
            if(showWireFrame) {
                shadedColour = mix(wireFrameColour, shadedColour, edgeFactor(fragBarycentricCoords));
            }
            outColour = vec4(shadedColour, 1.0);
        }


        )shader"
        );
}
//------------------------------------------------------------------------------
// END phong.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start lines.cpp
//------------------------------------------------------------------------------

using lrc = givr::style::LineRenderContext;
using lirc = givr::style::LineInstancedRenderContext;
using Colour = givr::style::Colour;

template <typename RenderContextT>
void setLineUniforms(RenderContextT const &ctx, std::unique_ptr<givr::Program> const &p) {
    p->setVec3("colour", ctx.template value<Colour>());
}

void lrc::setUniforms(std::unique_ptr<givr::Program> const &p) const {
    setLineUniforms(*this, p);
}
void lirc::setUniforms(std::unique_ptr<givr::Program> const &p) const {
    setLineUniforms(*this, p);
}

std::string linesVertexSource(std::string modelSource) {
    return "#version 330 core\n" + modelSource + std::string(R"shader( mat4 model;
        layout(location=4) in vec3 position;

        uniform mat4 view;
        uniform mat4 projection;

        out vec3 fragPosition;

        void main(){
            mat4 modelview = model * view;
            mat4 mvp = projection * modelview;
            gl_Position = mvp * vec4(position, 1.0);
            vec4 modelVert = modelview * vec4(position, 1.0);
            fragPosition = vec3(modelVert);
        }

        )shader"
    );
}

std::string linesFragmentSource() {
    return std::string(R"shader(#version 330 core
        uniform vec3 colour;
        uniform vec3 lightPosition;
        uniform vec3 viewPosition;

        in vec3 fragPosition;

        out vec4 outColour;

        void main()
        {
            outColour = vec4(colour, 1.);
        }


        )shader"
    );
}

std::string lrc::getVertexShaderSource() const {
    return linesVertexSource("uniform");
}

std::string lrc::getFragmentShaderSource() const {
    return linesFragmentSource();
}

std::string lirc::getVertexShaderSource() const {
    return linesVertexSource("layout(location=0) in");
}

std::string lirc::getFragmentShaderSource() const {
    return linesFragmentSource();
}

//------------------------------------------------------------------------------
// END lines.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start noshading.cpp
//------------------------------------------------------------------------------

using nsrc = givr::style::NoShadingRenderContext;
using nsirc = givr::style::NoShadingInstancedRenderContext;

template <typename RenderContextT>
void setNoShadingUniforms(RenderContextT const &ctx, std::unique_ptr<givr::Program> const &p) {
    p->setVec3("colour", ctx.template value<givr::style::Colour>());
}

void nsrc::setUniforms(std::unique_ptr<givr::Program> const &p) const {
    setNoShadingUniforms(*this, p);
}
void nsirc::setUniforms(std::unique_ptr<givr::Program> const &p) const {
    setNoShadingUniforms(*this, p);
}

std::string noShadingVertexSource(std::string modelSource) {
    return "#version 330 core\n" + modelSource + std::string(R"shader( mat4 model;
        layout(location=4) in vec3 position;

        uniform mat4 view;
        uniform mat4 projection;
        uniform vec3 colour;

        void main()
        {
            mat4 mvp = projection * view * model;
            gl_Position = mvp * vec4(position, 1.0);
        }
        )shader"
    );
}

std::string noShadingFragmentSource() {
    return std::string(R"shader(#version 330 core
        uniform vec3 colour;

        out vec4 outColour;

        void main()
        {
            outColour = vec4(colour, 1.0);
        }

        )shader"
    );
}

std::string nsrc::getVertexShaderSource() const {
    return noShadingVertexSource("uniform");
}
std::string nsrc::getFragmentShaderSource() const {
    return noShadingFragmentSource();
}
std::string nsirc::getVertexShaderSource() const {
    return noShadingVertexSource("layout(location=0) in");
}
std::string nsirc::getFragmentShaderSource() const {
    return noShadingFragmentSource();
}
//------------------------------------------------------------------------------
// END noshading.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start cylinder.cpp
//------------------------------------------------------------------------------

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>

using CylinderGeometry = givr::geometry::CylinderGeometry;

CylinderGeometry::Data givr::geometry::generateGeometry(CylinderGeometry const &c) {
    CylinderGeometry::Data data;

    auto add = [&](std::vector<float> &v, vec3f const &p) {
        v.push_back(p[0]);
        v.push_back(p[1]);
        v.push_back(p[2]);
    };

    // Calculate the axis around which we will rotate and a
    // vector which is perpindicular to that axis.
    vec3f axis = c.p2() - c.p1();
    // Find the x,y,z axis along which the axis vector is the shortest
    std::vector<std::size_t> indices{0, 1, 2};
    std::size_t min_i = *std::min_element(
        indices.begin(),
        indices.end(),
        [&](std::size_t const &i_1, std::size_t const &i_2) {
            return std::abs(axis[i_1]) < std::abs(axis[i_2]);
        }
    );
    // Use that axis to define a pseudo-orthogonal vector.
    vec3f radius_vec = vec3f{0.0, 0.0, 0.0};
    radius_vec[min_i] = 1.0f;
    // Use cross product, normalize and then scale by radius to get our vector
    radius_vec = c.radius() * glm::normalize(glm::cross(axis, radius_vec));

    // Next, rotate the vector around that axis by c.azimuthPoints() times.
    data.vertices.reserve(c.azimuthPoints()*6);
    data.normals.reserve(c.azimuthPoints()*6);
    float step = 360.0f/static_cast<float>(c.azimuthPoints());
    for (float x = 0.f; x < 360.0f; x+= step) {
        float angle = x * 0.01745329252f; // convert to radians
        vec3f normal = glm::rotate(radius_vec, angle, axis);
        add(data.vertices, c.p2() + normal);
        add(data.vertices, c.p1() + normal);
        add(data.normals, normal);
        add(data.normals, normal);
    }
    // Create the triangles 2 at a time.
    for (std::size_t i = 2; i < data.vertices.size()/3; i+=2) {
        data.indices.push_back(i);
        data.indices.push_back(i-2);
        data.indices.push_back(i-1);

        data.indices.push_back(i+1);
        data.indices.push_back(i);
        data.indices.push_back(i-1);
    }

    // Create the last set
    auto size = data.vertices.size()/3;
    data.indices.push_back(0);
    data.indices.push_back(size-2);
    data.indices.push_back(size-1);

    data.indices.push_back(1);
    data.indices.push_back(0);
    data.indices.push_back(size-1);

    return data;
}
//------------------------------------------------------------------------------
// END cylinder.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start sphere.cpp
//------------------------------------------------------------------------------

#include <cassert>
#include <cmath>

using SphereGeometry = givr::geometry::SphereGeometry;

SphereGeometry::Data givr::geometry::generateGeometry(SphereGeometry const &s) {
    SphereGeometry::Data data;

    // TODO: should make this reserve the space necessary for the sphere.

    struct point { float v[3]; };
    //Azimuth: 0 to 2pi, Altitude: 0 to pi
    auto sphereFunc = [](float azi, float alt) {
        return point{ std::cos(azi)*std::sin(alt), std::cos(alt), std::sin(azi)*std::sin(alt) };
    };
    auto azimuthPoints = s.value<AzimuthPoints>().value();
    auto altitudePoints = s.value<AltitudePoints>().value();
    auto centroid = s.value<Centroid>().value();
    auto radius = s.value<Radius>().value();

    for (size_t azi = 0; azi < azimuthPoints; azi++) {
        for (size_t alt = 0; alt < altitudePoints; alt++) {
            if (azi < azimuthPoints - 1 && alt < altitudePoints - 1) {
                data.indices.push_back((alt + 1) + (azi + 1)* altitudePoints);
                data.indices.push_back((alt + 1) + azi * altitudePoints);
                data.indices.push_back(alt + azi * altitudePoints);

                data.indices.push_back(alt + (azi + 1)*altitudePoints);
                data.indices.push_back((alt + 1) + (azi + 1)*altitudePoints);
                data.indices.push_back(alt + azi * altitudePoints);
            }
            else if(alt < altitudePoints - 1){
                data.indices.push_back((alt + 1));
                data.indices.push_back((alt + 1) + azi * altitudePoints);
                data.indices.push_back(alt + azi * altitudePoints);

                data.indices.push_back(alt);
                data.indices.push_back((alt + 1));
                data.indices.push_back(alt + azi * altitudePoints);
            }
            float u = float(azi) / float(azimuthPoints - 1);
            float v = float(alt) / float(altitudePoints - 1);
            //Make uniform distribution
            v = acos(1 - 2.f*v) / M_PI;     //DELETE THIS IF SPHERES LOOK BAD

            point spherePoint = sphereFunc(2.f*M_PI*u, M_PI*v);
            data.vertices.push_back((radius * spherePoint.v[0]) + centroid[0]);
            data.vertices.push_back((radius * spherePoint.v[1]) + centroid[1]);
            data.vertices.push_back((radius * spherePoint.v[2]) + centroid[2]);

            data.normals.push_back(spherePoint.v[0]);
            data.normals.push_back(spherePoint.v[1]);
            data.normals.push_back(spherePoint.v[2]);

            data.uvs.push_back(u);
            data.uvs.push_back(v);
        }
    }

    assert(data.vertices.size() == data.normals.size());

    return data;
}
//------------------------------------------------------------------------------
// END sphere.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start multiline.cpp
//------------------------------------------------------------------------------

using MultiLineGeometry = givr::geometry::MultiLineGeometry;
using LineGeometry = givr::geometry::LineGeometry;

void MultiLineGeometry::push_back(LineGeometry l) {
    m_segments.push_back(l);
}

MultiLineGeometry::Data givr::geometry::generateGeometry(MultiLineGeometry const &l) {
    MultiLineGeometry::Data data;
    auto const &segments = l.segments();
    data.vertices.reserve(6 * segments.size());
    auto push_vertex = [&](vec3f const &p) {
        for(std::size_t i = 0; i < 3; ++i) {
            data.vertices.push_back(p[i]);
        }
    };
    for(const auto &segment : segments) {
        push_vertex(segment.p1());
        push_vertex(segment.p2());
    }
    return data;
}
//------------------------------------------------------------------------------
// END multiline.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start triangle.cpp
//------------------------------------------------------------------------------

using TriangleGeometry = givr::geometry::TriangleGeometry;

TriangleGeometry::Data givr::geometry::generateGeometry(TriangleGeometry const &t) {
    TriangleGeometry::Data data;
    vec3f normal = glm::normalize(glm::cross(t.p2()-t.p1(), t.p3()-t.p1()));
    data.normals.reserve(9);
    data.vertices.reserve(9);
    auto push_vertex = [&](vec3f const &p) {
        for(std::size_t i = 0; i < 3; ++i) {
            data.vertices.push_back(p[i]);
            data.normals.push_back(normal[i]);
        }
    };
    push_vertex(t.p1());
    push_vertex(t.p2());
    push_vertex(t.p3());
    return data;
}
//------------------------------------------------------------------------------
// END triangle.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start mesh.cpp
//------------------------------------------------------------------------------
#include <iostream>
#include <unordered_map>
#include <tuple>

struct index_pair {
    unsigned int a, b;

    bool operator==(const index_pair& rh) const {
        return a == rh.a && b == rh.b;
    }

    bool operator<(index_pair right_hand) {
        return (a < right_hand.a) ? true : b < right_hand.b;
    }
};

namespace std {
    template<>
    struct hash<index_pair> {
        size_t operator()(const index_pair &key) const {
            return size_t(key.a << 16) | size_t(key.b);
        }
    };
};

namespace givr {
namespace geometry {

    template<typename V1, typename V2>
    std::tuple<std::vector<unsigned int>, V1, V2> two_index_unifier(
        const std::vector<unsigned int> &indices_A, const V1 &data_A, size_t components_A,
        const std::vector<unsigned int> &indices_B, const V2 &data_B, size_t components_B)
    {

        std::vector<unsigned int> unified_indices;
        V1 unified_dataA;
        V2 unified_dataB;
        std::unordered_map<index_pair, unsigned int> index_map;

        for (std::size_t i = 0; i < indices_A.size(); i++) {
            if (index_map.find({ indices_A[i], indices_B[i] }) == index_map.end()) {
                index_map[{indices_A[i], indices_B[i]}] = unified_dataA.size()/components_A;
                unified_indices.push_back(unified_dataA.size()/components_A);

                for (std::size_t j = 0; j < components_A; j++)
                    unified_dataA.push_back(data_A[indices_A[i]*components_A + j]);

                for (std::size_t j = 0; j < components_B; j++)
                    unified_dataB.push_back(data_B[indices_B[i]*components_B + j]);
            }
            else {
                unified_indices.push_back(index_map[{indices_A[i], indices_B[i]}]);
            }
        }

        return { unified_indices, unified_dataA, unified_dataB };
    }


    template<typename V1, typename V2, typename V3>
    std::tuple<std::vector<unsigned int>, V1, V2, V3> three_index_unifier(
        const std::vector<unsigned int> &indices_A, const V1 &data_A, size_t components_A,
        const std::vector<unsigned int> &indices_B, const V2 &data_B, size_t components_B,
        const std::vector<unsigned int> &indices_C, const V3 &data_C, size_t components_C)
    {

        std::vector<unsigned int> partially_unified_indices;
        V1 partially_unified_dataA;
        V2 partially_unified_dataB;
        V3 partially_unified_dataC;
        std::unordered_map<index_pair, unsigned int> partially_unified_index_map;

        for (std::size_t i = 0; i < indices_A.size(); i++) {
            if (partially_unified_index_map.find({ indices_A[i], indices_B[i] }) == partially_unified_index_map.end()) {
                partially_unified_index_map[ {indices_A[i], indices_B[i]} ] = partially_unified_dataA.size()/components_A;
                partially_unified_indices.push_back(partially_unified_dataA.size()/components_A);

                for(std::size_t j=0; j<components_A; j++)
                    partially_unified_dataA.push_back(data_A[indices_A[i]*components_A + j]);
                for (std::size_t j = 0; j<components_B; j++)
                    partially_unified_dataB.push_back(data_B[indices_B[i]*components_B + j]);
            }
            else {
                partially_unified_indices.push_back(partially_unified_index_map[{indices_A[i], indices_B[i]}]);
            }
        }

        std::vector<unsigned int> unified_indices;
        V1 unified_dataA;
        V2 unified_dataB;
        V3 unified_dataC;
        std::unordered_map<index_pair, unsigned int> unified_index_map;

        for (std::size_t i = 0; i < partially_unified_indices.size(); i++) {
            if (unified_index_map.find({ partially_unified_indices[i], indices_C[i] }) == unified_index_map.end()) {
                unified_index_map[{partially_unified_indices[i], indices_C[i] }] = unified_dataA.size()/components_A;
                unified_indices.push_back(unified_dataA.size()/components_A);

                for (std::size_t j = 0; j<components_A; j++)
                    unified_dataA.push_back(partially_unified_dataA[partially_unified_indices[i] * components_A + j]);
                for (std::size_t j = 0; j<components_B; j++)
                    unified_dataB.push_back(partially_unified_dataB[partially_unified_indices[i] * components_B + j]);
                for (std::size_t j = 0; j<components_C; j++)
                    unified_dataC.push_back(data_C[indices_C[i] * components_C + j]);
            }
            else {
                unified_indices.push_back(unified_index_map[{ partially_unified_indices[i], indices_C[i] }]);
            }
        }

        return { unified_indices, unified_dataA, unified_dataB, unified_dataC };
    }

    MeshGeometry::Data loadMeshFile(const char *file_name) {

        //Tiny obj loading
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string errors;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &errors, file_name)) {
            std::cerr << errors << std::endl;
            return MeshGeometry::Data{};
        }

        std::vector<float> multi_index_vertex_data = attrib.vertices;
        std::vector<float> multi_index_uv_data = attrib.texcoords;
        std::vector<float> multi_index_normal_data = attrib.normals;
        std::vector<unsigned int> vertex_indices;
        std::vector<unsigned int> uv_indices;
        std::vector<unsigned int> normal_indices;
        for (auto index : shapes[0].mesh.indices) {
            vertex_indices.push_back(index.vertex_index);
            uv_indices.push_back(index.texcoord_index);
            normal_indices.push_back(index.normal_index);
        }

        MeshGeometry::Data unifiedIndexMesh;

        if (multi_index_uv_data.size() == 0 && multi_index_normal_data.size() == 0) {
            unifiedIndexMesh.indices = vertex_indices;
            unifiedIndexMesh.vertices = multi_index_vertex_data;
        }
        else if (multi_index_normal_data.size() != 0 && multi_index_uv_data.size() == 0) {
            auto[unified_indices, unified_vertices, unified_normals] = two_index_unifier(
                vertex_indices, multi_index_vertex_data, 3,
                normal_indices, multi_index_normal_data, 3
            );

            unifiedIndexMesh.indices = unified_indices;
            unifiedIndexMesh.vertices = unified_vertices;
            unifiedIndexMesh.normals = unified_normals;
        }
        else if (multi_index_normal_data.size() == 0 && multi_index_uv_data.size() != 0) {
            auto[unified_indices, unified_vertices, unified_uvs] = two_index_unifier(
                vertex_indices, multi_index_vertex_data, 3,
                normal_indices, multi_index_uv_data, 2
            );

            unifiedIndexMesh.indices = unified_indices;
            unifiedIndexMesh.vertices = unified_vertices;
            unifiedIndexMesh.uvs = unified_uvs;
        }
        else{
            auto[unified_indices, unified_vertices, unified_normals, unified_uvs] = three_index_unifier(
                vertex_indices, multi_index_vertex_data, 3,
                normal_indices, multi_index_normal_data, 3,
                uv_indices, multi_index_uv_data, 2);

            unifiedIndexMesh.indices = unified_indices;
            unifiedIndexMesh.vertices = unified_vertices;
            unifiedIndexMesh.normals = unified_normals;
            unifiedIndexMesh.uvs = unified_uvs;
        }

        //unifiedIndexMesh.uvs.resize(unifiedIndexMesh.vertices.size() * 2 / 3);
        return unifiedIndexMesh;

    }

    MeshGeometry::Data generateGeometry(const MeshGeometry& m) {
        return loadMeshFile(m.value<Filename>().value().c_str());
    }

}// namespace geometry
}// namespace givr
//------------------------------------------------------------------------------
// END mesh.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start triangle_soup.cpp
//------------------------------------------------------------------------------

using TriangleSoupGeometry = givr::geometry::TriangleSoupGeometry;
using namespace givr::geometry;

TriangleSoupGeometry::Data givr::geometry::generateGeometry(TriangleSoupGeometry const &t) {
    typename TriangleSoupGeometry::Data data;
    data.vertices.reserve(9 * t.triangles().size());
    auto push_vertex = [&](std::vector<float> &v, vec3f const &p) {
        for(std::size_t i = 0; i < 3; ++i) {
            v.push_back(p[i]);
        }
    };
    for(const auto &tri : t.triangles()) {
        auto normal = glm::normalize(glm::cross(tri.p2()-tri.p1(), tri.p3()-tri.p1()));

        push_vertex(data.vertices, tri.value<Point1>());
        push_vertex(data.normals, normal);
        push_vertex(data.vertices, tri.value<Point2>());
        push_vertex(data.normals, normal);
        push_vertex(data.vertices, tri.value<Point3>());
        push_vertex(data.normals, normal);
    }
    return data;
}
//------------------------------------------------------------------------------
// END triangle_soup.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start quad.cpp
//------------------------------------------------------------------------------

using QuadGeometry = givr::geometry::QuadGeometry;

QuadGeometry::Data givr::geometry::generateGeometry(QuadGeometry const &t) {
    QuadGeometry::Data data;
    vec3f normal1 = glm::normalize(glm::cross(t.p3()-t.p2(), t.p1()-t.p2()));
    vec3f normal2 = glm::normalize(glm::cross(t.p4()-t.p3(), t.p1()-t.p3()));
    vec3f avg = glm::normalize(normal1 + normal2);
    data.uvs.reserve(8);
    data.normals.reserve(12);
    data.vertices.reserve(12);
    auto push_vertex = [&](vec3f const &p, vec3f const &n) {
        for(std::size_t i = 0; i < 3; ++i) {
            data.vertices.push_back(p[i]);
            data.normals.push_back(n[i]);
        }
    };
    auto push_uvs = [&](float u, float v) {
        data.uvs.push_back(u);
        data.uvs.push_back(v);
    };
    auto add_tri = [&](std::uint32_t i, std::uint32_t j, std::uint32_t k) {
        data.indices.push_back(i);
        data.indices.push_back(j);
        data.indices.push_back(k);
    };
    push_vertex(t.p1(), normal1);
    push_uvs(0.0, 0.0);
    push_vertex(t.p2(), avg);
    push_uvs(0.0, 1.0);
    push_vertex(t.p3(), avg);
    push_uvs(1.0, 1.0);
    push_vertex(t.p4(), normal2);
    push_uvs(1.0, 0.0);

    add_tri(0, 1, 2);
    add_tri(0, 2, 3);
    return data;
}
//------------------------------------------------------------------------------
// END quad.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start line.cpp
//------------------------------------------------------------------------------

using LineGeometry = givr::geometry::LineGeometry;
LineGeometry::Data givr::geometry::generateGeometry(LineGeometry const &l) {
    LineGeometry::Data data;
    data.vertices.reserve(6);
    auto push_vertex = [&](vec3f const &p) {
        for(std::size_t i = 0; i < 3; ++i) {
            data.vertices.push_back(p[i]);
        }
    };
    push_vertex(l.p1());
    push_vertex(l.p2());
    return data;
}
//------------------------------------------------------------------------------
// END line.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start program.cpp
//------------------------------------------------------------------------------
#include <glm/gtc/type_ptr.hpp>

using Program = givr::Program;
using vec2f = givr::vec2f;
using vec3f = givr::vec3f;
using mat4f = givr::mat4f;

Program::Program(
    GLuint vertex,
    GLuint fragment
) : m_programID{glCreateProgram()}
{
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, fragment);
    linkAndErrorCheck();
}

Program::Program(
    GLuint vertex,
    GLuint geometry,
    GLuint fragment
) : m_programID{glCreateProgram()}
{
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, geometry);
    glAttachShader(m_programID, fragment);
    linkAndErrorCheck();
}

void Program::linkAndErrorCheck() {

    glLinkProgram(m_programID);
    GLint success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_programID, 512, NULL, infoLog);
        std::ostringstream out;
        out << "Unable to link program: " << infoLog;
        // TODO(lw): Consider a better exception here
        throw std::runtime_error(out.str());
    }
}

Program::~Program() {
    glDeleteProgram(m_programID);
}

void Program::use() {
    glUseProgram(m_programID);
}

void Program::setVec2(const std::string &name, vec2f const &value) const
{
    glUniform2fv(glGetUniformLocation(m_programID, name.c_str()), 1, value_ptr(value));
}
void Program::setVec3(const std::string &name, vec3f const &value) const
{
    glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, value_ptr(value));
}
void Program::setMat4(const std::string &name, mat4f const &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, value_ptr(mat));
}
void Program::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), static_cast<int>(value));
}
void Program::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Program::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

/*
void Program::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}
void Program::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}
void Program::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_programID, name.c_str()), x, y);
}
void Program::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_programID, name.c_str()), x, y, z);
}
void Program::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}
void Program::setVec4(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(m_programID, name.c_str()), x, y, z, w);
}
void Program::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Program::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}*/

//------------------------------------------------------------------------------
// END program.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start shader.cpp
//------------------------------------------------------------------------------

using Shader = givr::Shader;

Shader::Shader(
    const std::string &source,
    GLenum shaderType
) : m_shaderID{glCreateShader(shaderType)}
{
    const GLchar *source_char = source.c_str();
    glShaderSource(m_shaderID, 1, &source_char, NULL);
    glCompileShader(m_shaderID);
    GLint success;
    glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(m_shaderID, 512, NULL, infoLog);
        std::ostringstream out;
        out << "Unable to compile Shader " << infoLog;
        // TODO(lw): Consider a better exception here
        throw std::runtime_error(out.str());
    }
}

Shader::~Shader() {
    glDeleteShader(m_shaderID);
}
//------------------------------------------------------------------------------
// END shader.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start texture.cpp
//------------------------------------------------------------------------------
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION


using Texture = givr::Texture;

Texture::Texture(
) : m_textureID{0}
{
    alloc();
}
// TODO(lw): make a version that just receives the source directly.
Texture::~Texture() {
    dealloc();
}

void Texture::alloc()
{
    dealloc();
    glGenTextures(1, &m_textureID);
}
void Texture::dealloc()
{
    if (m_textureID) {
        //Temporarily disabled until better way to manage IDs is decided on
        //glDeleteBuffers(1, &m_textureID);
    }
}

void Texture::bind(GLenum target)
{
    glBindTexture(target, m_textureID);
}

void Texture::load(GLenum target, std::string filename, GLint level)
{
    int width, height, comp;
    unsigned char *image = stbi_load(filename.c_str(),
        &width, &height, &comp, 0);

    if ((image == nullptr) || (comp > 4)) {
        throw std::runtime_error("Failed to load texture");
    } else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        const GLenum formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };

        glActiveTexture(GL_TEXTURE0);    //Bind to avoid disturbing active units
        glBindTexture(target, m_textureID);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(target, level, formats[comp - 1], width,
            height, 0, formats[comp - 1], GL_UNSIGNED_BYTE, image);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);        //Return to default
    }

    stbi_image_free(image);

}
//------------------------------------------------------------------------------
// END texture.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start vertex_array.cpp
//------------------------------------------------------------------------------
#include <cassert>

using VertexArray = givr::VertexArray;

VertexArray::VertexArray(
) : m_vertexArrayID{}
{
    alloc();
}

void VertexArray::alloc() {
    dealloc();
    glGenVertexArrays(1, &m_vertexArrayID);
}

void VertexArray::dealloc() {
    if (m_vertexArrayID) {
        glDeleteVertexArrays(1, &m_vertexArrayID);
        m_vertexArrayID = 0;
    }
}

void VertexArray::bind() {
    glBindVertexArray(m_vertexArrayID);
}
void VertexArray::unbind() {
    glBindVertexArray(0);
}

VertexArray::~VertexArray() {
    dealloc();
}
//------------------------------------------------------------------------------
// END vertex_array.cpp
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start buffer.cpp
//------------------------------------------------------------------------------
#include <cassert>

using Buffer = givr::Buffer;

Buffer::Buffer(
) : m_bufferID{0}
{
    alloc();
}

void Buffer::alloc() {
    dealloc();
    glGenBuffers(1, &m_bufferID);
}
void Buffer::dealloc() {
    if (m_bufferID) {
        glDeleteBuffers(1, &m_bufferID);
    }
}

void Buffer::bind(GLenum target) {
    glBindBuffer(target, m_bufferID);
}
void Buffer::unbind(GLenum target) {
    glBindBuffer(target, 0);
}

Buffer::~Buffer() {
    dealloc();
}
//------------------------------------------------------------------------------
// END buffer.cpp
//------------------------------------------------------------------------------

