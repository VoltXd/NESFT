#include "IO/Shader.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
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
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    // fShader
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentID);
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
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
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            std::cerr << e.what() << '\n';
            exit(EXIT_FAILURE);
        }

        const char* gShaderCode = geometryCode.c_str();
        geometryID = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryID, 1, &gShaderCode, nullptr);
        glCompileShader(geometryID);
        glGetShaderiv(geometryID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geometryID, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            exit(EXIT_FAILURE);
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
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    if (geometryPath != nullptr)
        glDeleteShader(geometryID);
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
