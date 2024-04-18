//
// Created by lodrik on 24-04-05.
//

#include "load.h"

namespace splatwasm {

    bool load_splat_ply(PathPrefix pp, std::string_view filename, Splat &data) {
        std::string filepath;
        filepath.reserve(pp.scene.size() + filename.size());
        filepath += pp.scene;
        filepath += filename;

        std::ifstream file_stream(filepath);
        try
        {
            if (!file_stream || file_stream.fail()) throw std::runtime_error("file_stream failed to open " + filepath);

            PlyFile file;
            file.parse_header(file_stream);

            // Load value into pointer;
            std::shared_ptr<PlyData> position, harmonic, opacity, scale, rotation;
            {
                try { position = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
                catch (const std::exception & e) { fmt::println("tinyply exception {}", e.what()); return false; }

                try { harmonic = file.request_properties_from_element("vertex", { "f_dc_0", "f_dc_1", "f_dc_2" }); }
                catch (const std::exception & e) { fmt::println("tinyply exception {}", e.what()); return false; }

                try { opacity = file.request_properties_from_element("vertex", { "opacity" }); }
                catch (const std::exception & e) { fmt::println("tinyply exception {}", e.what()); return false; }

                try { scale = file.request_properties_from_element("vertex", { "scale_0", "scale_1", "scale_2" }); }
                catch (const std::exception & e) { fmt::println("tinyply exception {}", e.what()); return false; }

                // "rot_0" is equal to the W coordinate.
                try { rotation = file.request_properties_from_element("vertex", { "rot_1", "rot_2", "rot_3", "rot_0" }); }
                catch (const std::exception & e) { fmt::println("tinyply exception {}", e.what()); return false; }
            }

            reserve_splat(data, position->count);
            file.read(file_stream);

            // Example One: converting to your own application types
            {

                {
                    assert(data.count == position->count);

                    // Copy Position information
                    auto vec = reinterpret_cast<glm::vec3*>(position->buffer.get());
                    std::copy(vec, vec + position->count, std::back_inserter(data.positions));
                }

                // Approximate Color using the lower Harmonical basis.
                {
                    assert(data.count == harmonic->count);

                    std::vector<glm::vec3> harms; harms.reserve(harmonic->count);
                    auto vec = reinterpret_cast<glm::vec3*>(harmonic->buffer.get());
                    std::copy(vec, vec + harmonic->count, std::back_inserter(harms));

                    for (auto h : harms) {
                        float SH_C0 = 0.28209479177387814l;
                        // Use only the first basis of SPH to concatenate each Spherical Harmonic into one.
                        // Make it lose more complex properties, but should be good enough.
                        data.colors.emplace_back( 0.5f + SH_C0 * h[0], 0.5f + SH_C0 * h[1], 0.5f + SH_C0 * h[2]);
                    }
                }


                // Convert opacity to [0, 1] as in the original work
                {
                    assert(data.count == opacity->count);

                    std::vector<float> ops; ops.reserve(opacity->count);
                    auto vec = reinterpret_cast<float*>(opacity->buffer.get());
                    std::copy(vec, vec + opacity->count, std::back_inserter(ops));

                    for (auto op : ops) {
                        data.opacities.emplace_back( 1. / (1. + exp(-op)) );
                    }
                }


                // Compute the model matrix
                {
                    assert(data.count == rotation->count);
                    assert(data.count == scale->count);

                    //Make sure that quat memory is layout similarly to buffer.
                    auto rots = reinterpret_cast<glm::vec4*>(rotation->buffer.get());
                    auto scls = reinterpret_cast<glm::vec3*>(scale->buffer.get());

                    for (int i = 0; i < data.count; i += 1) {

                        // The first rot_0 is equal to the w....
                        glm::quat r = glm::normalize(glm::quat(rots[i][0], rots[i][1], rots[i][2], rots[i][3]));
                        glm::vec3 s = glm::exp(scls[i]);

                        // Compute Scale Matrix
                        glm::mat3 S = glm::scale(glm::mat4(1.), s);

                        // Compute Rotation Matrix

                        // Found out that when comparing to other software on github that the rotation matrix is computed diffirently than glm::toMat3...
                        glm::mat3 R = glm::transpose(glm::toMat3(r));

                        glm::mat3 M = S * R;

                        // Sigma in the paper
                        data.cov3Ds.emplace_back(glm::transpose(M) * M);
                    }
                }

            }
        }
        catch (const std::exception & e)
        {
            fmt::println("Caught tinyply exception: {}", e.what());
            return false;
        }


        return true;
    }

}