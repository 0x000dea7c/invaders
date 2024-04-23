#include "invaders.h"
#include "invaders_types.h"
#include "invaders_math.h"

#include <array>
#include <vector>
#include <set>
#include <utility>
#include <cassert>

namespace Game {
    // -------------------------------------------
    // data, everything is on meters except when rendering
    // -------------------------------------------
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

    // -------------------------------------------
    // internal globals
    // -------------------------------------------
    static Texture_Manager texture_manager;
    static Shader_Manager shader_manager;
    static int gSize = 0; // TEMP

    // -------------------------------------------
    // functions
    // -------------------------------------------
    static void load_textures(Invaders& g)
    {
        const auto player_texture = load_texture("./res/textures/player.png",
                                                 true,
                                                 false,
                                                 GL_CLAMP_TO_EDGE,
                                                 GL_CLAMP_TO_EDGE);

        g.player.size.x = (player_texture.width / Meters_per_pixel) * 0.5f;
        g.player.size.y = (player_texture.height / Meters_per_pixel) * 0.5f;

        texture_manager.data["player"] = player_texture;

        const auto alien_atlas = load_texture("./res/textures/stop_inverting_bitch.png",
                                              true,
                                              false,
                                              GL_CLAMP_TO_EDGE,
                                              GL_CLAMP_TO_EDGE);

        texture_manager.data["alien"] = alien_atlas;

        const auto bg = load_texture("./res/textures/test.jpg",
                                     false,
                                     false,
                                     GL_REPEAT,
                                     GL_REPEAT);

        texture_manager.data["bg"] = bg;

        const auto missile = load_texture("./res/textures/missile.png",
                                          true,
                                          true,
                                          GL_CLAMP_TO_EDGE,
                                          GL_CLAMP_TO_EDGE);

        texture_manager.data["missile"] = missile;
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

    // TEMP
    static void load_grid_shader(Grid& gr)
    {
        u32 id{ load_compile_shaders("./res/shaders/grid.vert", "./res/shaders/grid.frag") };
        if(!id) {
            std::cerr << "Error creating grid shader\n";
            exit(EXIT_FAILURE);
        }

        std::vector<float> vertices;
        for (int i = 0; i <= gr.cols; ++i) { // 6
            // Vertical lines
            float x = i * gr.cell_size.x;
            vertices.push_back(x);
            vertices.push_back(0.0f);
            vertices.push_back(x);
            vertices.push_back(gr.rows * gr.cell_size.y);
        }
        for (int j = 0; j <= gr.rows; ++j) { // 7
            // Horizontal lines
            float y = j * gr.cell_size.y;
            vertices.push_back(0.0f); // x
            vertices.push_back(y); // y
            vertices.push_back(gr.cols * gr.cell_size.x);
            vertices.push_back(y);
        }

        u32 vbo, vao;
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        gSize = vertices.size();

        shader_manager.data["grid"] = { .id = id, .VAO = vao, .VBO = vbo };

        m4 m = orthographic({
            .left   =  0.f,
            .right  = WINDOW_WIDTH,
            .bottom =  0.f,
            .top    = WINDOW_HEIGHT,
            .zNear  = -1.f,
            .zFar   =  1.f
        });

        use_program(id);
        set_uniform_int(id, "image", 0);
        set_uniform_mat4(id, "projection", m);
    }

    static void load_bg_shader(const m4& m)
    {
        u32 id{ load_compile_shaders("./res/shaders/bg.vert", "./res/shaders/bg.frag") };
        if(!id) {
            std::cerr << "Error creating background shader\n";
            exit(EXIT_FAILURE);
        }

        constexpr f32 vertices[] {
            // pos -- tex
            -1.f, -1.f, 0.f, 0.f, // bottom left
             1.f, -1.f, 1.f, 0.f,  // bottom right
            -1.f,  1.f, 0.f, 1.f, // top left

            -1.f,  1.f, 0.f, 1.f, // top left
             1.f,  1.f, 1.f, 1.f,  // top right
             1.f, -1.f, 1.f, 0.f,  // bottom right
        };

        u32 vbo, vao;
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        shader_manager.data["bg"] = { .id = id, .VAO = vao, .VBO = vbo };

        use_program(id);
        set_uniform_int(id, "image", 0);
        set_uniform_mat4(id, "projection", m);
    }

    static void load_missile_shader(const m4& m)
    {
        u32 id{ load_compile_shaders("./res/shaders/missiles.vert", "./res/shaders/missiles.frag") };
        if(!id) {
            std::cerr << "Error creating missile shader\n";
            exit(EXIT_FAILURE);
        }

        u32 vbo, vao;
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(Instance_Data) * 32, nullptr, GL_DYNAMIC_DRAW);

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

        shader_manager.data["missile"] = { .id = id, .VAO = vao, .VBO = vbo };

        use_program(id);
        set_uniform_int(id, "image", 0);
        set_uniform_mat4(id, "projection", m);
    }

    static void load_shaders(const f32 sceneWidth, const f32 sceneHeight)
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
        load_bg_shader(m);
        load_missile_shader(m);
    }

    void init(Invaders& g)
    {
        // at this point the platform already initialized opengl and all of
        // that
        load_textures(g);
        load_shaders(g.scene_width_m * Meters_per_pixel, g.scene_height_m * Meters_per_pixel);
        load_levels(g);

        init_grid(g.grid, g.scene_width_px, g.scene_height_px);

        // TEMP
        load_grid_shader(g.grid);

        // renderer initial settings
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

        if(g.keys[Game::KEY_SPACE]) {
            g.player.shooting = true;
        } else {
            g.player.shooting = false;
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

        if(g.player.pos.x + g.player.size.x >= g.scene_width_m) {
            g.player.pos.x = g.scene_width_m - g.player.size.x;
        } else if(g.player.pos.x - g.player.size.x < 0.f) {
            g.player.pos.x = g.player.size.x;
        }

        if(g.player.pos.y + g.player.size.y > g.scene_height_m) {
            g.player.pos.y = g.scene_height_m - g.player.size.y;
        } else if(g.player.pos.y - g.player.size.y < 0.f) {
            g.player.pos.y = g.player.size.y;
        }

        // transforms
        g.player.model = identity();
        g.player.model = scale(g.player.model, v3{
            .x = g.player.size.x * Meters_per_pixel,
            .y = g.player.size.y * Meters_per_pixel,
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
            // aliens.data[i].pos.x += std::sin(time()) * dt;

            // transforms
            aliens.instance_data[i].model = identity();

            aliens.instance_data[i].model = scale(aliens.instance_data[i].model, v3{
                .x = aliens.data[i].size.x * Meters_per_pixel,
                .y = aliens.data[i].size.y * Meters_per_pixel,
                .z = 1.f
            });

            aliens.instance_data[i].model = translate(aliens.instance_data[i].model, v3{
                .x = aliens.data[i].pos.x * Meters_per_pixel,
                .y = aliens.data[i].pos.y * Meters_per_pixel,
                .z = 0.f
            });

            // nasty, there's probably a better way but don't know it yet
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

    static void spawn_missiles(Missiles& m, Player& p)
    {
        static const auto missile_texture = texture_manager.data.at("missile");
        static const auto missile_width   = missile_texture.width;
        static const auto missile_height  = missile_texture.height;
        static const auto missile_size_x  = missile_width / Meters_per_pixel;
        static const auto missile_size_y  = missile_height / Meters_per_pixel;

        // spawn a pair
        v2 left = v2{ .x = p.pos.x + 0.1f,
                      .y = p.pos.y + p.size.y };

        v2 right = v2{ .x = p.pos.x + p.size.x - 0.1f,
                       .y = p.pos.y + p.size.y };

        m.data.emplace_back(Missile
                            {
                                .pos = v3{ left.x , left.y, 0.f },
                                .size = v2{ missile_size_x * 0.3f, missile_size_y * 0.3f },
                                .width = missile_width,
                                .height = missile_height,
                            });

        m.data.emplace_back(Missile
                            {
                                .pos = v3{ right.x , right.y, 0.f },
                                .size = v2{ missile_size_x * 0.3f, missile_size_y * 0.3f },
                                .width = missile_width,
                                .height = missile_height,
                            });

        m.instance_data.emplace_back(Instance_Data{});

        m.instance_data.emplace_back(Instance_Data{});
    }

    static void update_missiles(Invaders& g, const f32 dt)
    {
        if(g.player.shooting && g.player.shooting_cd <= 0.f) {
            spawn_missiles(g.missiles, g.player);
            g.player.shooting_cd = 20.f;
        }

        --g.player.shooting_cd;

        for(u32 i = 0; i < g.missiles.data.size(); ++i) {
            g.missiles.data[i].pos.y += 5.f * dt;

            if(g.missiles.data[i].pos.y > g.scene_height_m /* TODO || collided() */ ) {
                g.missiles.data[i].destroyed = true;
            }

            if(g.missiles.data[i].destroyed) {
                std::swap(g.missiles.data[i], g.missiles.data.back());
                std::swap(g.missiles.instance_data[i], g.missiles.instance_data.back());
                g.missiles.data.pop_back();
                g.missiles.instance_data.pop_back();
                --i;
                continue;
            }

            g.missiles.instance_data[i].vertex_data = {{
                { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
                { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
                { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
                { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f},  // top left
                { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
                { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
            }};

            g.missiles.instance_data[i].model = identity();

            g.missiles.instance_data[i].model = scale(g.missiles.instance_data[i].model,
                                                      v3{
                                                          .x = g.missiles.data[i].size.x * Meters_per_pixel,
                                                          .y = g.missiles.data[i].size.y * Meters_per_pixel,
                                                          .z = 1.f
                                                      });

            g.missiles.instance_data[i].model = translate(g.missiles.instance_data[i].model,
                                                          v3{
                                                              .x = g.missiles.data[i].pos.x * Meters_per_pixel,
                                                              .y = g.missiles.data[i].pos.y * Meters_per_pixel,
                                                              .z = 0.f
                                                          });
        }

        glBindBuffer(GL_ARRAY_BUFFER, shader_manager.data.at("missile").VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Instance_Data) * g.missiles.instance_data.size(), g.missiles.instance_data.data());
    }

    static void do_collisions(Invaders& g)
    {
        v2 px{ g.player.pos.x * Meters_per_pixel,
               g.player.pos.y * Meters_per_pixel };

        // detect collisions only with missiles -> aliens
        // std::set<Entity_Grid_Data, Entity_Grid_Data_Comp> ents;
        // get_nearby(g.grid, px, &g.player, ents);
    }

    void update(Invaders& g, const f32 dt)
    {
        update_player(g, dt);
        update_aliens(g.aliens, dt);
        update_grid(g.grid, g.player, g.aliens);
        update_missiles(g, dt);

        // do_collisions(g);
    }

    void render(Invaders& g, const f32 dt)
    {
        glClearColor(1.f, 1.f, 1.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static const auto player_shader   = shader_manager.data.at("player");
        static const auto player_texture  = texture_manager.data.at("player");
        static const auto aliens_shader   = shader_manager.data.at("alien");
        static const auto aliens_texture  = texture_manager.data.at("alien");
        static const auto grid_shader     = shader_manager.data.at("grid"); // TEMP
        static const auto bg_shader       = shader_manager.data.at("bg");
        static const auto bg_texture      = texture_manager.data.at("bg");
        static const auto missile_shader  = shader_manager.data.at("missile");
        static const auto missile_texture = texture_manager.data.at("missile");

        glActiveTexture(GL_TEXTURE0);

        // background
        use_program(bg_shader.id);
        set_uniform_float(bg_shader.id, "time", Game::time() * 0.8f);
        glBindVertexArray(bg_shader.VAO);
        glBindTexture(GL_TEXTURE_2D, bg_texture.id);
        glDrawArrays(GL_TRIANGLES, 0, 6);


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

        // missiles (also batch render)
        use_program(missile_shader.id);
        glBindVertexArray(missile_shader.VAO);
        glBindTexture(GL_TEXTURE_2D, missile_texture.id);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g.missiles.instance_data.size());

        // TEMP grid
        use_program(grid_shader.id);
        glBindVertexArray(grid_shader.VAO);
        glDrawArrays(GL_LINES, 0, gSize / 2);
        glBindVertexArray(0);
    }

};

