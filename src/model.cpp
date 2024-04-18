//
// Created by lodrik on 24-04-14.
//

#include "model.h"

#include <utility>

namespace splatwasm {

    bool create_model(PathPrefix pp, Splat splat, Model &model) {
        model.splat = std::move(splat);

        {
            std::string vertex_source;
            if (!load_shader(pp, "gsplat.vs", vertex_source)) {
                fmt::println("Failed to load vertex shader source!");
                return false;
            }

            GLuint vertex_shader;
            if (!create_and_compile_shader(vertex_source, GL_VERTEX_SHADER, vertex_shader)) {
                fmt::println("Failed to compile vertex shader.");
                return false;
            }

            std::string fragment_source;
            if(!load_shader(pp, "gsplat.fs", fragment_source)) {
                fmt::println("Failed to load fragment shader source!");
                return false;
            }

            GLuint fragment_shader;
            if (!create_and_compile_shader(fragment_source, GL_FRAGMENT_SHADER, fragment_shader)) {
                fmt::println("Failed to compile fragment shader.");
                return false;
            }

            // Link the vertex and fragment shader into a shader program
            GLuint shaders[]{vertex_shader, fragment_shader};
            model.program = create_and_link_program(shaders, sizeof(shaders)/sizeof(GLuint));

            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
        }

        glUseProgram(model.program);

        // Generate Empty Buffer.
        glGenVertexArrays(1, &model.vao);
        glBindVertexArray(model.vao);

        glGenBuffers (1, &model.vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo_position);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribDivisor(0, 1); // Update Vertex Shader for each instance


        glGenBuffers (1, &model.vbo_color);
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo_color);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1); // Update Vertex Shader for each instance


        glGenBuffers (1, &model.vbo_cov3d);
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo_cov3d);

        // There is a bug here...
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0 * sizeof(float)));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2); glEnableVertexAttribArray(3); glEnableVertexAttribArray(4);
        glVertexAttribDivisor(2, 1); glVertexAttribDivisor(3, 1); glVertexAttribDivisor(4, 1);

        glGenBuffers (1, &model.vbo_opacity);
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo_opacity);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(5);
        glVertexAttribDivisor(5, 1);

        return true;
    }

    void update_buffers(Model &model) {
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo_position);
        glBufferData(GL_ARRAY_BUFFER, model.splat.count * sizeof(glm::vec3), model.splat.positions.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, model.vbo_color);
        glBufferData(GL_ARRAY_BUFFER, model.splat.count * sizeof(glm::vec3), model.splat.colors.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, model.vbo_cov3d);
        glBufferData(GL_ARRAY_BUFFER, model.splat.count * sizeof(glm::mat3), model.splat.cov3Ds.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, model.vbo_opacity);
        glBufferData(GL_ARRAY_BUFFER, model.splat.count * sizeof(float), model.splat.opacities.data(), GL_STATIC_DRAW);
    }


    void render(App &app, Model &model) {
        glUseProgram(model.program);
        glBindVertexArray(model.vao);

        int loc_proj = glGetUniformLocation(model.program, "mproj");
        glUniformMatrix4fv(loc_proj, 1, GL_FALSE, glm::value_ptr(app.cam.proj));

        int loc_view = glGetUniformLocation(model.program, "mview");
        glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(app.cam.view));

        int loc_res = glGetUniformLocation(model.program, "uresolution");
        glUniform2f(loc_res, float(app.W), float(app.H));

        // This function call should avoid round trip between the CPU and GPU and make it more efficient.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, model.splat.count);
    }

    void destroy_model(Model& model) {
        glDeleteBuffers(1, &model.vbo_opacity);
        glDeleteBuffers(1, &model.vbo_cov3d);
        glDeleteBuffers(1, &model.vbo_color);
        glDeleteBuffers(1, &model.vbo_position);

        glDeleteVertexArrays(1, &model.vao);
        glDeleteProgram(model.program);

        destroy_splat(model.splat);
    }
    
}