#include "invaders_shaders.h"

#include <array>
#include <iostream>
#include <fstream>
#include <sstream>

namespace Game {

    static bool
    compile_errors(const u32 object,
                   const ShaderType type)
    {
        int success{ 0 };
        std::array<char, 1024> log{ '\0' };
        std::string shader;

        switch(type) {
        case ShaderType::VERTEX:
            shader = "VERTEX";
            // fall through
        case ShaderType::FRAGMENT:
            shader = "FRAGMENT";
            // fall through
        case ShaderType::GEOMETRY:
            shader = "GEOMETRY";
            glGetShaderiv(object, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(object, log.size(), nullptr, &log[0]);
                std::cerr << "Couldn't compile shader type " << shader << " : " << log.data() << '\n';
                return true;
            }
            break;
        case ShaderType::PROGRAM:
            glGetProgramiv(object, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(object, log.size(), nullptr, &log[0]);
                std::cerr << "Couldn't link program: " << log.data() << '\n';
                return true;
            }
            break;
        }

        return false;
    }

    u32 load_compile_shaders(const char* vert_file,
                             const char* frag_file)
    {
        std::ifstream vert_file_stream(vert_file);
        if(!vert_file_stream) {
            std::cerr << "Couldn't open vertex file: " << vert_file << '\n';
            return false;
        }

        std::ifstream frag_file_stream(frag_file);
        if(!frag_file_stream) {
            std::cerr << "Couldn't open fragment file: " << frag_file << '\n';
            return false;
        }

        std::stringstream vertFileSS;
        vertFileSS << vert_file_stream.rdbuf();

        std::stringstream fragFileSS;
        fragFileSS << frag_file_stream.rdbuf();

        u32 vert_id{ 0 }, frag_id{ 0 };
        const std::string vert_code{ vertFileSS.str() };
        const std::string frag_code{ fragFileSS.str() };
        const char* vert_codeC{ vert_code.c_str() };
        const char* frag_codeC{ frag_code.c_str() };

        vert_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_id, 1, &vert_codeC, nullptr);
        glCompileShader(vert_id);
        bool errors{ compile_errors(vert_id, ShaderType::VERTEX) };
        if(errors) {
            glDeleteShader(vert_id);
            return 0;
        }

        frag_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_id, 1, &frag_codeC, nullptr);
        glCompileShader(frag_id);
        errors = compile_errors(frag_id, ShaderType::FRAGMENT);
        if(errors) {
            glDeleteShader(vert_id);
            glDeleteShader(frag_id);
            return 0;
        }

        u32 program_id{ glCreateProgram() };
        glAttachShader(program_id, vert_id);
        glAttachShader(program_id, frag_id);

        glLinkProgram(program_id);
        errors = compile_errors(program_id, ShaderType::PROGRAM);

        if(errors) {
            glDeleteShader(vert_id);
            glDeleteShader(frag_id);
            return 0;
        }

        glDeleteShader(vert_id);
        glDeleteShader(frag_id);

        return program_id;
    }

};
