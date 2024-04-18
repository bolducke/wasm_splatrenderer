#include "utils.h"

namespace splatwasm{

    bool load_shader(PathPrefix pp, std::string_view filename, std::string &source) {
        std::string filepath;
        filepath.reserve(pp.shader.size() + filename.size());
        filepath += pp.shader;
        filepath += filename;

        // Load content into file
        std::ifstream file_stream(filepath, std::ios::in);
        if (file_stream) {
            std::stringstream buffer;
            buffer << file_stream.rdbuf();
            source = std::move(buffer.str());
        } else {
            return false;
        }

        return true;
    }

    bool create_and_compile_shader(std::string_view source, GLenum mode, GLuint& shader) {
        shader = glCreateShader(mode);
        const char* code = source.data();
        glShaderSource(shader, 1, &code, NULL);
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            GLchar errorLog[maxLength];
            glGetShaderInfoLog(shader, maxLength, NULL, &errorLog[0]);

            fmt::println("error - {}", std::string(errorLog));
            return false;
        }

        return true;
    }

    GLuint create_and_link_program(GLuint* shaders, int count) {
        GLuint shader_program = glCreateProgram();

        for(int i = 0; i < count; i += 1) {
            glAttachShader(shader_program, shaders[i]);
        }
        glLinkProgram(shader_program);

        return shader_program;
    }
}