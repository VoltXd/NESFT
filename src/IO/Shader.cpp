#include "IO/Shader.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    mIsCompiled = true;
    // 1. Retrieve shaders from source files
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Ensure files can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(const std::ifstream::failure& e)
    {
        mErrorMessage << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        mErrorMessage << e.what() << '\n';
        mIsCompiled = false;
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    unsigned int vertexID, fragmentID, geometryID = 0;
    int success;
    char infoLog[512];

    // vShader
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vShaderCode, nullptr);
    glCompileShader(vertexID);
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexID, 512, nullptr, infoLog);
        mErrorMessage << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        mIsCompiled = false;
        return;
    }

    // fShader
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentID);
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentID, 512, nullptr, infoLog);
        mErrorMessage << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        mIsCompiled = false;
        return;
    }

    if (geometryPath != nullptr)
    {
        std::string geometryCode;
        std::ifstream gShaderFile;

        // Ensure files can throw exceptions
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            std::stringstream gShaderStream;

            gShaderFile.open(geometryPath);
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();

            geometryCode = gShaderStream.str();
        }
        catch(const std::ifstream::failure& e)
        {
            mErrorMessage << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            mErrorMessage << e.what() << '\n';
            mIsCompiled = false;
            return;
        }

        const char* gShaderCode = geometryCode.c_str();
        geometryID = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryID, 1, &gShaderCode, nullptr);
        glCompileShader(geometryID);
        glGetShaderiv(geometryID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geometryID, 512, nullptr, infoLog);
            mErrorMessage << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            mIsCompiled = false;
            return;
        }
    }

    // Shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertexID);
    glAttachShader(ID, fragmentID);
    if (geometryPath != nullptr)
        glAttachShader(ID, geometryID);
    glLinkProgram(ID);

    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        mErrorMessage << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        mIsCompiled = false;
        return;
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    if (geometryPath != nullptr)
        glDeleteShader(geometryID);
}

Shader::Shader(const char* vertexCode, const char* fragmentCode)
{
    // 1. Compile shaders
    unsigned int vertexID, fragmentID;
    int success;
    char infoLog[512];

    // vShader
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexCode, nullptr);
    glCompileShader(vertexID);
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexID, 512, nullptr, infoLog);
        mErrorMessage << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        mIsCompiled = false;
        return;
    }

    // fShader
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fragmentCode, nullptr);
    glCompileShader(fragmentID);
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentID, 512, nullptr, infoLog);
        mErrorMessage << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        mIsCompiled = false;
        return;
    }

    // Shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertexID);
    glAttachShader(ID, fragmentID);
    glLinkProgram(ID);

    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        mErrorMessage << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        mIsCompiled = false;
        return;
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
}

Shader::~Shader()
{
    glDeleteProgram(ID);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
