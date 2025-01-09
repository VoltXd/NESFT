#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
    public:
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
    Shader(const char* vertexSource, const char* fragmentSource);
    ~Shader();

    void use();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    inline unsigned int getID() const { return ID; };
    inline bool isCompiled() const { return mIsCompiled; }
    inline std::string getErrorMessage() const { return mErrorMessage.str(); }

    private:
    unsigned int ID;
    bool mIsCompiled;
    std::stringstream mErrorMessage;
};

constexpr const char VERT_SHADER_DEFAULT[] =
"#version 460 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoords;\n"
"out vec2 texCoords;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
"    texCoords = aTexCoords;\n"
"}";

constexpr const char FRAG_SHADER_DEFAULT[] =
"#version 460 core\n"
"in vec2 texCoords;\n"
"out vec4 fragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"    fragColor = vec4(texture(screenTexture, texCoords).rgb, 1.0f);\n"
"}";

constexpr const char FRAG_SHADER_NEGATIVE[] =
"#version 460 core\n"
"in vec2 texCoords;\n"
"out vec4 fragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"    fragColor = vec4(vec3(1.0 - texture(screenTexture, texCoords)), 1.0);\n"
"}";

constexpr const char FRAG_SHADER_GRAYSCALE[] =
"#version 460 core\n"
"in vec2 texCoords;\n"
"out vec4 fragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"    fragColor = texture(screenTexture, texCoords);\n"
"    float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;\n"
"    fragColor = vec4(average, average, average, 1.0);\n"
"}";

constexpr const char FRAG_SHADER_SHARPEN[] =
"#version 460 core\n"
"in vec2 texCoords;\n"
"out vec4 fragColor;\n"
"uniform sampler2D screenTexture;\n"
"const float offset = 1.0 / 200.0;\n"
"void main()\n"
"{\n"
"    vec2 offsets[9] = vec2[](\n"
"        vec2(-offset,  offset), // top-left\n"
"        vec2( 0.0f,    offset), // top-center\n"
"        vec2( offset,  offset), // top-right\n"
"        vec2(-offset,  0.0f),   // center-left\n"
"        vec2( 0.0f,    0.0f),   // center-center\n"
"        vec2( offset,  0.0f),   // center-right\n"
"        vec2(-offset, -offset), // bottom-left\n"
"        vec2( 0.0f,   -offset), // bottom-center\n"
"        vec2( offset, -offset)  // bottom-right\n"
"    );\n"
"    const float kernel[9] = float[](\n"
"        -1, -1, -1,\n"
"        -1,  9, -1,\n"
"        -1, -1, -1\n"
"    );\n"
"    vec3 sampleTex[9];\n"
"    for(int i = 0; i < 9; i++)\n"
"    {\n"
"        sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offsets[i]));\n"
"    }\n"
"    vec3 col = vec3(0.0);\n"
"    for(int i = 0; i < 9; i++)\n"
"        col += sampleTex[i] * kernel[i];\n"
"    fragColor = vec4(col, 1.0);\n"
"}";

constexpr const char FRAG_SHADER_EDGES[] =
"#version 460 core\n"
"in vec2 texCoords;\n"
"out vec4 fragColor;\n"
"uniform sampler2D screenTexture;\n"
"const float offset = 1.0 / 200.0;  \n"
"void main()\n"
"{\n"
"    vec2 offsets[9] = vec2[](\n"
"        vec2(-offset,  offset), // top-left\n"
"        vec2( 0.0f,    offset), // top-center\n"
"        vec2( offset,  offset), // top-right\n"
"        vec2(-offset,  0.0f),   // center-left\n"
"        vec2( 0.0f,    0.0f),   // center-center\n"
"        vec2( offset,  0.0f),   // center-right\n"
"        vec2(-offset, -offset), // bottom-left\n"
"        vec2( 0.0f,   -offset), // bottom-center\n"
"        vec2( offset, -offset)  // bottom-right    \n"
"    );\n"
"    const float kernel[9] = float[](\n"
"        1,  1, 1,\n"
"        1, -8, 1,\n"
"        1,  1, 1\n"
"    );\n"
"    vec3 sampleTex[9];\n"
"    for(int i = 0; i < 9; i++)\n"
"    {\n"
"        sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offsets[i]));\n"
"    }\n"
"    vec3 col = vec3(0.0);\n"
"    for(int i = 0; i < 9; i++)\n"
"        col += sampleTex[i] * kernel[i];\n"
"    fragColor = vec4(col, 1.0);\n"
"}";