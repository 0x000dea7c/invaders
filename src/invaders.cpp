#include "invaders.h"
#include "invaders_types.h"
#include "invaders_math.h"

#include <unordered_map>
#include <array>
#include <vector>
#include <utility>

#define ALIEN_YELLOW 0
#define ALIEN_BEIGE  1
#define ALIEN_GREEN  2
#define ALIEN_PINK   3
#define ALIEN_BLUE   4

namespace Game {
    // -------------------------------------------
    // data
    // -------------------------------------------
    constexpr f32 Meters_per_pixel{ 50.f };

    struct Texture_Manager final {
        ~Texture_Manager()
        {
            for(auto& d : data) {
                glDeleteTextures(1, &d.second.id);
            }
        }

        std::unordered_map<const char*, Texture_Info> data;
    };

    struct Shader_Info final {
        u32 id{ 0 };
        u32 VAO{ 0 };
        u32 VBO{ 0 };
    };

    struct Shader_Manager final {
        ~Shader_Manager()
        {
            for(auto& d : data) {
                glDeleteProgram(d.second.id);
            }
        }

        std::unordered_map<const char*, Shader_Info> data;
    };

    struct Movement_Spec final {
        v3 dir{ 0.f, 0.f, 0.f };
        f32 dirScale{ 80.f };
        f32 drag{ .8f };
        bool unit{ true };
    };

    struct Player final {
        m4 model{ identity() };
        Movement_Spec ms;
        v3 pos{ 0.f, 0.f, 0.f };
        v3 vel{ 0.f, 0.f, 0.f };
        v2 size{ 0.f, 0.f };
        f32 tex_width{ 0.f };
        f32 tex_height{ 0.f };
    };

    enum Key {
        KEY_Q,
        KEY_A,
        KEY_W,
        KEY_S,
        KEY_D,
        KEY_ESCAPE,
        KEY_LEFT,
        KEY_UP,
        KEY_RIGHT,
        KEY_DOWN,
        KEY_SPACE,
        KEY_ENTER,

        KEY_UNHANDLED
    };

    struct Instance_Data final {
        m4 model{ identity() };
        std::array<v4, 6> vertex_data;
    };

    struct Alien final {
        v2 pos{ 0.f, 0.f };
        f32 width{ 0.f };
        f32 height{ 0.f };
        i32 idx_x{ 0 };
        i32 idx_y{ 0 };
    };

    struct Aliens final {
        std::vector<Instance_Data> instance_data;
        std::vector<Alien> data;
    };

    struct Invaders final {
        Player player;
        Aliens aliens;
        std::unordered_map<Game::Key, bool> keys;
        u32 current_level{ 0 };
        i32 sceneWidth{ 0 };
        i32 sceneHeight{ 0 };
        bool is_running{ true };
    };

    // -------------------------------------------
    // internal globals
    // -------------------------------------------
    static Texture_Manager texture_manager;
    static Shader_Manager shader_manager;

    // -------------------------------------------
    // functions
    // -------------------------------------------
    static void load_textures(Invaders& g)
    {
        const auto player_texture = load_texture("./res/textures/player.png", true, false);

        g.player.tex_width = player_texture.width;
        g.player.tex_height = player_texture.height;
        g.player.size.x = g.player.tex_width / Meters_per_pixel;
        g.player.size.y = g.player.tex_height / Meters_per_pixel;

        texture_manager.data["player"] = player_texture;

        const auto alien_atlas = load_texture("./res/textures/stop_inverting_bitch.png", true, false);

        texture_manager.data["alien"] = alien_atlas;
    }

    static void load_player_shaders(const m4& perspective)
    {
        u32 id{ load_compile_shaders("./res/shaders/shader.vert", "./res/shaders/shader.frag") };
        if(!id) {
            std::cerr << "Error creating player shader\n";
            exit(EXIT_FAILURE);
        }

        constexpr float vertices[]{
            // pos -- tex
            -1.f, -1.f, 0.f, 0.f, // bottom left
             1.f, -1.f, 1.f, 0.f,  // bottom right
            -1.f,  1.f, 0.f, 1.f, // top left

            -1.f,  1.f, 0.f, 1.f, // top left
             1.f,  1.f, 1.f, 1.f,  // top right
             1.f, -1.f, 1.f, 0.f,  // bottom right
        };

        u32 VAO;
        u32 VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(0));
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        shader_manager.data["player"] = { .id = id, .VAO = VAO };

        // TODO: use UBOs for image and projection?
        use_program(id);
        set_uniform_int(id, "image", 0);
        set_uniform_mat4(id, "projection", perspective);
    }

    static void load_alien_shaders(const m4& perspective)
    {
        u32 id{ load_compile_shaders("./res/shaders/aliens.vert", "./res/shaders/aliens.frag") };
        if(!id) {
            std::cerr << "Error creating player shader\n";
            exit(EXIT_FAILURE);
        }

        u32 VAO;
        u32 VBO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        // TODO: that 16 hurts, should be a constant
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Instance_Data) * 16, nullptr, GL_DYNAMIC_DRAW);

        // model matrix
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(0));
        glVertexAttribDivisor(0, 1);  // one value per instance

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(v4)));
        glVertexAttribDivisor(1, 1);  // one value per instance

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(v4) * 2));
        glVertexAttribDivisor(2, 1);  // one value per instance

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(v4) * 3));
        glVertexAttribDivisor(3, 1);  // one value per instance

        // texture coords
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(m4)));
        glVertexAttribDivisor(4, 1);

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(m4) + sizeof(v4)));
        glVertexAttribDivisor(5, 1);

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 2));
        glVertexAttribDivisor(6, 1);

        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 3));
        glVertexAttribDivisor(7, 1);

        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 4));
        glVertexAttribDivisor(8, 1);

        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 5));
        glVertexAttribDivisor(9, 1);

        shader_manager.data["alien"] = { .id = id, .VAO = VAO, .VBO = VBO };

        use_program(id);
        set_uniform_int(id, "image", 0);
        set_uniform_mat4(id, "projection", perspective);
    }

    static void load_shaders(const i32 sceneWidth, const i32 sceneHeight)
    {
        m4 m = orthographic({
            .left   =  0.f,
            .right  = (float)sceneWidth,
            .bottom =  0.f,
            .top    = (float)sceneHeight,
            .zNear  = -1.f,
            .zFar   =  1.f
        });

        load_player_shaders(m);
        load_alien_shaders(m);
    }

    static std::pair<f32, f32> get_alien_tex_dims(const i32 type)
    {
        // hardcoded values bc each tex dim is different, :(
        switch(type) {
            case ALIEN_YELLOW: {
                return {125.f, 108.f};
            } break;
            case ALIEN_BEIGE: {
                return {125.f, 122.f};
            } break;
            case ALIEN_GREEN: {
                return {125.f, 123.f};
            } break;
            case ALIEN_PINK: {
                return {125.f, 127.f};
            } break;
            case ALIEN_BLUE: {
                return {125.f, 144.f};
            } break;
            default: {
                assert(false && "get_alien_tex_dims: Invalid alien type!");
            } break;
        }
    }

    static std::pair<i32, i32> get_alien_atlas_idx(const i32 type)
    {
        switch(type) {
            case ALIEN_YELLOW: {
                return {0, 0};
            } break;
            case ALIEN_BEIGE: {
                return {1, 0};
            } break;
            case ALIEN_GREEN: {
                return {2, 0};
            } break;
            case ALIEN_PINK: {
                return {3, 0};
            } break;
            case ALIEN_BLUE: {
                return {4, 0};
            } break;
            default: {
                assert(false && "get_alien_atlas_idx: Invalid alien type!");
            } break;
        }
    }

    static void load_levels(Invaders& g)
    {
        constexpr i32 first[4][4] {
            { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
            { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
            { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW },
            { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW }
        };

        const i32 (*levels[])[4][4] {
            &first
        };

        static constexpr f32 spacingX{ 50.f / Meters_per_pixel };
        static const f32 initialYOffset{ (g.sceneHeight - 200.f) / Meters_per_pixel };
        static constexpr f32 spacingY{ 50.f / Meters_per_pixel };

        for(u32 i{ 0 }; i < 4; ++i) {
            for(u32 j{ 0 }; j < 4; ++j) {
                const auto alien_type = (*levels[g.current_level])[i][j];
                const auto alien_tex_dim = get_alien_tex_dims(alien_type);
                const auto alien_atlas_idx = get_alien_atlas_idx(alien_type);
                const f32 startX{ (g.sceneWidth / Meters_per_pixel) - (alien_tex_dim.first / Meters_per_pixel) * 4 };

                g.aliens.data.emplace_back(Alien{
                    .pos    = v2{ .x = startX - (j * ((alien_tex_dim.first / Meters_per_pixel) + spacingX)),
                                  .y = initialYOffset - i * ((alien_tex_dim.second / Meters_per_pixel) + spacingY) },
                    .width  = alien_tex_dim.first,
                    .height = alien_tex_dim.second,
                    .idx_x  = alien_atlas_idx.first,
                    .idx_y  = alien_atlas_idx.second
                });

                g.aliens.instance_data.emplace_back(Instance_Data{});
            }
        }
    }

    void init(Invaders& g)
    {
        // at this point the platform already initialized opengl and all of
        // that
        //
        // now, load textures on memory, compile shaders, etc
        // ...
        load_textures(g);
        load_shaders(g.sceneWidth, g.sceneHeight);
        load_levels(g);

        // renderer initial settings
        // glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void process_input(Invaders& g)
    {
        if(g.keys[Game::KEY_Q] || g.keys[Game::KEY_ESCAPE]) {
            g.is_running = false;
            return;
        }

        g.player.ms = Movement_Spec{};

        if(g.keys[Game::KEY_W] || g.keys[Game::KEY_UP]) {
            g.player.ms.dir.y =  1.f;
        }

        if(g.keys[Game::KEY_S] || g.keys[Game::KEY_DOWN]) {
            g.player.ms.dir.y = -1.f;
        }

        if(g.keys[Game::KEY_A] || g.keys[Game::KEY_LEFT]) {
            g.player.ms.dir.x = -1.f;
        }

        if(g.keys[Game::KEY_D] || g.keys[Game::KEY_RIGHT]) {
            g.player.ms.dir.x =  1.f;
        }
    }

    void update_player(Invaders& g, const f32 dt)
    {
        v3 direction{ g.player.ms.dir };

        // normalize direction if required
        if(g.player.ms.unit) {
            direction = normalize(direction);
        }

        // calculate acceleration
        v3 acc = scale(direction, g.player.ms.dirScale);

        // calculate drag based on current velocity
        v3 drag = scale(g.player.vel, -g.player.ms.drag);
        acc = add(acc, drag);

        // update velocity by applying acceleration
        g.player.vel = add(g.player.vel, scale(acc, dt));

        // calculate displacement due to new velocity for this frame
        v3 velocity_dt = scale(g.player.vel, dt);

        // additional position adjustment due to acceleration using the kinematic equation: Δx = ½at²
        v3 acc_dt = scale(acc, 0.5f * square(dt));

        // update position by adding both velocity displacement and acceleration displacement
        // clamp with screen dimensions
        g.player.pos = add(g.player.pos, velocity_dt);
        g.player.pos = add(g.player.pos, acc_dt);

        static auto right_screen_edge = g.sceneWidth / Meters_per_pixel;
        static auto up_screen_edge = g.sceneHeight / Meters_per_pixel;

        if(g.player.pos.x + g.player.size.x >= right_screen_edge) {
            g.player.pos.x = right_screen_edge - g.player.size.x;
        } else if(g.player.pos.x - g.player.size.x < 0.f) {
            g.player.pos.x = g.player.size.x;
        }

        if(g.player.pos.y + g.player.size.y > up_screen_edge) {
            g.player.pos.y = up_screen_edge - g.player.size.y;
        } else if(g.player.pos.y - g.player.size.y < 0.f) {
            g.player.pos.y = g.player.size.y;
        }

        // transforms
        g.player.model = identity();
        g.player.model = scale(g.player.model, v3{
            // .x = g.player.tex_width,
            // .y = g.player.tex_height,
            .x = g.player.tex_width * 0.5f,
            .y = g.player.tex_height * 0.5f,
            .z = 1.f
        });
        g.player.model = translate(g.player.model, v3{
            .x = g.player.pos.x * Meters_per_pixel,
            .y = g.player.pos.y * Meters_per_pixel,
            .z = g.player.pos.z * Meters_per_pixel
        });
    }

    static void update_aliens(Aliens& aliens, const f32 dt)
    {
        for(u32 i{ 0 }; i < aliens.data.size(); ++i) {
            aliens.data[i].pos.x += std::sin(time()) * dt;
            aliens.data[i].pos.y += std::cos(time()) * dt;

            // transforms
            aliens.instance_data[i].model = identity();

            aliens.instance_data[i].model = scale(aliens.instance_data[i].model, v3{
                .x = aliens.data[i].width * 0.5f,
                .y = aliens.data[i].height * 0.5f,
                .z = 1.f
            });

            aliens.instance_data[i].model = translate(aliens.instance_data[i].model, v3{
                .x = aliens.data[i].pos.x * Meters_per_pixel,
                .y = aliens.data[i].pos.y * Meters_per_pixel,
                .z = 0.f
            });

            // aliens.instance_data[i].vertex_data = {{
            //     { .x = -1.f, .y = -1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = (1024.f - (aliens.data[i].idx_y * aliens.data[i].height)) / 1024.f }, // bottom left
            //     { .x =  1.f, .y = -1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = (1024.f - (aliens.data[i].idx_y * aliens.data[i].height)) / 1024.f }, // bottom right
            //     { .x = -1.f, .y =  1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = (1024.f - ((aliens.data[i].idx_y + 1) * aliens.data[i].height)) / 1024.f }, // top left

            //     { .x = -1.f, .y =  1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = (1024.f - ((aliens.data[i].idx_y + 1) * aliens.data[i].height)) / 1024.f },   // top left
            //     { .x =  1.f, .y =  1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = (1024.f - ((aliens.data[i].idx_y + 1) * aliens.data[i].height)) / 1024.f },   // top right
            //     { .x =  1.f, .y = -1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = (1024.f - (aliens.data[i].idx_y * aliens.data[i].height)) / 1024.f } // bottom right
            // }};

            // aliens.instance_data[i].vertex_data = {{
            //         { .x = -1.f, .y = -1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = (aliens.data[i].idx_y * aliens.data[i].height) / 1024.f }, // bottom left
            //         { .x =  1.f, .y = -1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = (aliens.data[i].idx_y * aliens.data[i].height) / 1024.f }, // bottom right
            //         { .x = -1.f, .y =  1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = 1.f }, // top left

            //         { .x = -1.f, .y =  1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = 1.f },   // top left
            //         { .x =  1.f, .y =  1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = 1.f },   // top right
            //         { .x =  1.f, .y = -1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = (aliens.data[i].idx_y * aliens.data[i].height) / 1024.f } // bottom right
            //     }};

            aliens.instance_data[i].vertex_data = {{
                { .x = -1.f, .y = -1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = ((aliens.data[i].idx_y + 1) * aliens.data[i].height) / 1024.f }, // bottom left
                { .x =  1.f, .y = -1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = ((aliens.data[i].idx_y + 1) * aliens.data[i].height) / 1024.f }, // bottom right
                { .x = -1.f, .y =  1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = (aliens.data[i].idx_y * aliens.data[i].height) / 1024.f }, // top left

                { .x = -1.f, .y =  1.f, .z = (aliens.data[i].idx_x * aliens.data[i].width) / 1024.f, .w = (aliens.data[i].idx_y * aliens.data[i].height) / 1024.f },   // top left
                { .x =  1.f, .y =  1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = (aliens.data[i].idx_y * aliens.data[i].height) / 1024.f },   // top right
                { .x =  1.f, .y = -1.f, .z = ((aliens.data[i].idx_x + 1) * aliens.data[i].width) / 1024.f, .w = ((aliens.data[i].idx_y + 1) * aliens.data[i].height) / 1024.f } // bottom right
            }};

        }

        // update vbo with new data
        glBindBuffer(GL_ARRAY_BUFFER, shader_manager.data.at("alien").VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Instance_Data) * aliens.instance_data.size(), aliens.instance_data.data());
    }

    void update(Invaders& g, const f32 dt)
    {
        update_player(g, dt);
        update_aliens(g.aliens, dt);
    }

    void render(Invaders& g, const f32 dt)
    {
        glClearColor(1.f, 1.f, 1.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static const auto player_shader = shader_manager.data.at("player");
        static const auto player_texture = texture_manager.data.at("player");
        static const auto aliens_shader = shader_manager.data.at("alien");
        static const auto aliens_texture = texture_manager.data.at("alien");

        glActiveTexture(GL_TEXTURE0);

        // alien batch render call
        use_program(aliens_shader.id);
        glBindVertexArray(aliens_shader.VAO);

        glBindTexture(GL_TEXTURE_2D, aliens_texture.id);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g.aliens.instance_data.size());

        // player
        use_program(player_shader.id);
        glBindVertexArray(player_shader.VAO);

        glBindTexture(GL_TEXTURE_2D, player_texture.id);
        set_uniform_mat4(player_shader.id, "model", g.player.model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

};

