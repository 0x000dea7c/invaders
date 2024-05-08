#include "invaders.h"
#include "invaders_text_renderer.h"
#include "invaders_types.h"
#include "invaders_math.h"
#include "invaders_physics.h"
#include "invaders_opengl.h"
#include "invaders_shaders.h"
#include "invaders_timer.h"
#include "invaders_world.h"

#include <vector>
#include <utility>
#include <iostream>

#define MISSILE_PLAYER_VEL_Y 12.f
#define MISSILE_ALIEN_VEL_Y  8.f

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

    const auto alien_atlas = load_texture("./res/textures/aliens.png",
                                          true,
                                          false,
                                          GL_CLAMP_TO_EDGE,
                                          GL_CLAMP_TO_EDGE);

    texture_manager.data["alien"] = alien_atlas;

    const auto bg = load_texture("./res/textures/space.jpg",
                                 false,
                                 false,
                                 GL_REPEAT,
                                 GL_REPEAT);

    texture_manager.data["bg"] = bg;

    const auto missile = load_texture("./res/textures/missile_player.png",
                                      true,
                                      true,
                                      GL_CLAMP_TO_EDGE,
                                      GL_CLAMP_TO_EDGE);

    texture_manager.data["player_missile"] = missile;

    const auto missile_a = load_texture("./res/textures/missile_alien.png",
                                        true,
                                        true,
                                        GL_CLAMP_TO_EDGE,
                                        GL_CLAMP_TO_EDGE);

    texture_manager.data["alien_missile"] = missile_a;

    const auto explosion = load_texture("./res/textures/explosion.png",
                                        true,
                                        true,
                                        GL_CLAMP_TO_EDGE,
                                        GL_CLAMP_TO_EDGE);

    texture_manager.data["explosion"] = explosion;
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
    for (u32 i = 0; i <= gr.cols; ++i) { // 6
      // Vertical lines
      float x = i * gr.cell_size.x;
      vertices.push_back(x);
      vertices.push_back(0.0f);
      vertices.push_back(x);
      vertices.push_back(gr.rows * gr.cell_size.y);
    }
    for (u32 j = 0; j <= gr.rows; ++j) { // 7
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

  static void load_missile_shader(const m4& m, const char* name)
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

    shader_manager.data[name] = { .id = id, .VAO = vao, .VBO = vbo };

    use_program(id);
    set_uniform_int(id, "image", 0);
    set_uniform_mat4(id, "projection", m);
  }

  static void load_emitter_shader(const m4& m)
  {
    u32 id{ load_compile_shaders("./res/shaders/emitter.vert", "./res/shaders/emitter.frag") };
    if(!id) {
      std::cerr << "Error creating particle emitter shader\n";
      exit(EXIT_FAILURE);
    }

    u32 vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // TODO: hardcoded
    glBufferData(GL_ARRAY_BUFFER, sizeof(Explosion_Instance_Data) * 16 * 200, nullptr, GL_DYNAMIC_DRAW);

    // model matrix
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(0));
    glVertexAttribDivisor(0, 1);  // one value per instance

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(v4)));
    glVertexAttribDivisor(1, 1);  // one value per instance

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(v4) * 2));
    glVertexAttribDivisor(2, 1);  // one value per instance

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(v4) * 3));
    glVertexAttribDivisor(3, 1);  // one value per instance

    // quad pos and texture coords
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(m4)));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(m4) + sizeof(v4)));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 2));
    glVertexAttribDivisor(6, 1);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 3));
    glVertexAttribDivisor(7, 1);

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 4));
    glVertexAttribDivisor(8, 1);

    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 5));
    glVertexAttribDivisor(9, 1);

    // colour
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(Explosion_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 6));
    glVertexAttribDivisor(10, 1);

    shader_manager.data["explosion"] = { .id = id, .VAO = vao, .VBO = vbo };

    use_program(id);
    set_uniform_int(id, "image", 0);
    set_uniform_mat4(id, "projection", m);
  }

  static void update_player_lives_instance_data(Player& p)
  {
    p.lives.instance_data.clear();

    for(u32 i = 0; i < p.lives.current_lives; ++i) {
      m4 model = identity();
      model = scale(model, v3{ 30.f, 30.f, 0.f });
      model = translate(model, v3{ i * 70.f + 40.f, 50.f, 0.f });
      p.lives.instance_data.emplace_back(Player_Lives_Instance_Data{
          .model = model,
          .vertex_data = {{
              { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
              { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
              { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
              { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f},  // top left
              { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
              { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
            }}
        });
    }

    glBindBuffer(GL_ARRAY_BUFFER, shader_manager.data.at("player_lives").VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(Player_Lives_Instance_Data) * p.lives.instance_data.size(),
                    p.lives.instance_data.data());
  }

  static void load_player_lives_shader(Player& p, const m4& m)
  {
    u32 id{ load_compile_shaders("./res/shaders/player_lives.vert", "./res/shaders/player_lives.frag") };
    if(!id) {
      std::cerr << "Error creating player's live shader\n";
      exit(EXIT_FAILURE);
    }

    u32 vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Player_Lives_Instance_Data) * p.lives.current_lives, nullptr, GL_STATIC_DRAW);

    // model
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(0));
    glVertexAttribDivisor(0, 1);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(v4)));
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(v4) * 2));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(v4) * 3));
    glVertexAttribDivisor(3, 1);

    // quad pos and texture coords
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(m4)));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(m4) + sizeof(v4)));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 2));
    glVertexAttribDivisor(6, 1);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 3));
    glVertexAttribDivisor(7, 1);

    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 4));
    glVertexAttribDivisor(8, 1);

    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(Player_Lives_Instance_Data), (void*)(sizeof(m4) + sizeof(v4) * 5));
    glVertexAttribDivisor(9, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_manager.data["player_lives"] = { .id = id, .VAO = vao, .VBO = vbo };

    use_program(id);
    set_uniform_int(id, "image", 0);
    set_uniform_mat4(id, "projection", m);

    update_player_lives_instance_data(p);
  }

  static void init_player_lives(Player& p, const m4& m)
  {
    p.lives.current_lives = 4;
    load_player_lives_shader(p, m);
  }

  static void load_menu_shader()
  {
    const u32 id{ load_compile_shaders("./res/shaders/menu.vert", "./res/shaders/menu.frag") };
    if(!id) {
      std::cerr << "Error creating menu shader\n";
      exit(EXIT_FAILURE);
    }

    constexpr f32 vertices[] {
      // pos
      -0.5f, -0.5f, // bottom left
       0.5f, -0.5f, // bottom right
      -0.5f,  0.5f, // top left

      -0.5f,  0.5f, // top left
       0.5f,  0.5f, // top right
       0.5f, -0.5f, // bottom right
    };

    u32 vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader_manager.data["menu"] = { .id = id, .VAO = vao, .VBO = vbo };

    use_program(id);
  }

  static void load_text_renderer_shader(const m4& m)
  {
    const u32 id{ load_compile_shaders("./res/shaders/text_renderer.vert", "./res/shaders/text_renderer.frag") };
    if(!id) {
      std::cerr << "Error creating text renderer shader\n";
      exit(EXIT_FAILURE);
    }
    u32 VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    use_program(id);
    set_uniform_int(id, "image", 0);
    set_uniform_mat4(id, "projection", m);
    shader_manager.data["text_renderer"] = { .id = id, .VAO = VAO, .VBO = VBO };
  }

  static void load_shaders(const f32 sceneWidth, const f32 sceneHeight, Player& p)
  {
    m4 m = orthographic({
        .left   =  0.f,
        .right  = sceneWidth,
        .bottom =  0.f,
        .top    = sceneHeight,
        .zNear  = -1.f,
        .zFar   =  1.f
      });
    load_player_shaders(m);
    load_alien_shaders(m);
    load_bg_shader(m);
    load_missile_shader(m, "player_missile");
    load_missile_shader(m, "alien_missile");
    load_emitter_shader(m);
    load_menu_shader();
    load_text_renderer_shader(m);
    // loads lives instance data, that's why it's here
    init_player_lives(p, m);
  }

  void init(Invaders& g)
  {
    // at this point the platform already initialized opengl and all of that
    load_textures(g);
    load_shaders(g.scene_width_m * Meters_per_pixel, g.scene_height_m * Meters_per_pixel, g.player);
    load_levels(g);
    init_grid(g.grid, g.scene_width_px, g.scene_height_px);
    init_text_renderer(g.text_renderer, "./res/fonts/FreeMonoBold.ttf", 24);

    // TEMP
    load_grid_shader(g.grid);

    // renderer initial settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  // TODO Refactor
  static bool is_key_pressed(Invaders& g, const Key key)
  {
    return g.keys[key] && !g.prev_keys[key];
  }

  void process_input(Invaders& g)
  {
    if(is_key_pressed(g, Game::KEY_Q)) {
      g.is_running = false;
      return;
    }
    if(g.state == Game_State::PLAYING) {
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
      if(is_key_pressed(g, Game::KEY_ESCAPE)) {
        g.state = Game_State::MENU;
      }
    } else if(g.state == Game_State::MENU) {
      if(is_key_pressed(g, Game::KEY_ESCAPE)) {
        g.state = Game_State::PLAYING;
      }
      if(is_key_pressed(g, Game::KEY_W) || is_key_pressed(g, Game::KEY_UP)) {
        --g.current_menu_option;
      }
      if(is_key_pressed(g, Game::KEY_S) || is_key_pressed(g, Game::KEY_DOWN)) {
        ++g.current_menu_option;
      }
    }
    for (auto& key : g.keys) {
      g.prev_keys[key.first] = key.second;
    }
  }

  void update_player(Invaders& g, const f32 dt)
  {
    if(g.player.destroyed) {
      // reset
      g.player.pos.x = 0.f;
      g.player.pos.y = 0.f;
      g.player.destroyed = false;
      --g.player.lives.current_lives;
      update_player_lives_instance_data(g.player);

      if(g.player.lives.current_lives == 0) {
        g.state = Game_State::LOST;
      }
    } else {
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

      if(g.player.pos.y + g.player.size.y > g.scene_height_m * 0.3f) {
        g.player.pos.y = g.scene_height_m * 0.3f - g.player.size.y;
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
  }

  static void spawn_alien_missile(Missiles& m, const v2 pos)
  {
    static const auto missile_texture = texture_manager.data.at("alien_missile");
    static const auto missile_width   = missile_texture.width;
    static const auto missile_height  = missile_texture.height;
    static const auto missile_size_x  = missile_width * 0.3f / Meters_per_pixel;
    static const auto missile_size_y  = missile_height * 0.3f / Meters_per_pixel;

    m.data.emplace_back(Missile
                        {
                          .pos    = v3{ pos.x, pos.y, 0.f },
                          .size   = v2{ missile_size_x, missile_size_y },
                          .vel    = v2{ 0.f, -MISSILE_ALIEN_VEL_Y },
                          .width  = (f32)missile_width,
                          .height = (f32)missile_height
                        });

    m.instance_data.emplace_back(Instance_Data{});
  }

  static void update_aliens(Invaders& g, const f32 dt)
  {
    if(g.aliens.data.empty() && g.current_level < g.max_levels - 1) {
      g.state = Game_State::WIN_LEVEL;
      return;
    } else if(g.aliens.data.empty() && g.current_level == g.max_levels - 1) {
      g.state = Game_State::WIN_GAME;
      return;
    }

    for(u32 i{ 0 }; i < g.aliens.data.size(); ++i) {
      if(g.aliens.data[i].destroyed) {
        std::swap(g.aliens.data[i], g.aliens.data.back());
        std::swap(g.aliens.instance_data[i], g.aliens.instance_data.back());
        g.aliens.data.pop_back();
        g.aliens.instance_data.pop_back();
        --i;
        continue;
      }

      // check if it's time to spawn missiles
      if(g.aliens.data[i].currcd > g.aliens.data[i].firecd) {
        spawn_alien_missile(g.alien_missiles, v2{ g.aliens.data[i].pos.x, g.aliens.data[i].pos.y });
        g.aliens.data[i].currcd = 0;
      } else {
        ++g.aliens.data[i].currcd;
      }

      if(g.aliens.data[i].pos.x <= g.aliens.data[i].min_x || g.aliens.data[i].pos.x >= g.aliens.data[i].max_x) {
        g.aliens.data[i].dir = -g.aliens.data[i].dir;
        g.aliens.data[i].vel = scale(g.aliens.data[i].vel, g.aliens.data[i].dir);
      }

      g.aliens.data[i].pos.x += g.aliens.data[i].vel.x * dt;

      // transforms
      g.aliens.instance_data[i].model = identity();

      g.aliens.instance_data[i].model = scale(g.aliens.instance_data[i].model, v3{
          .x = g.aliens.data[i].size.x * Meters_per_pixel,
          .y = g.aliens.data[i].size.y * Meters_per_pixel,
          .z = 1.f
        });

      g.aliens.instance_data[i].model = translate(g.aliens.instance_data[i].model, v3{
          .x = g.aliens.data[i].pos.x * Meters_per_pixel,
          .y = g.aliens.data[i].pos.y * Meters_per_pixel,
          .z = 0.f
        });

      // nasty, there's probably a better way but don't know it yet
      g.aliens.instance_data[i].vertex_data = {{
          { .x = -1.f, .y = -1.f, .z = (g.aliens.data[i].idx_x * g.aliens.data[i].width) / 1024.f, .w = ((g.aliens.data[i].idx_y + 1) * g.aliens.data[i].height) / 1024.f }, // bottom left
          { .x =  1.f, .y = -1.f, .z = ((g.aliens.data[i].idx_x + 1) * g.aliens.data[i].width) / 1024.f, .w = ((g.aliens.data[i].idx_y + 1) * g.aliens.data[i].height) / 1024.f }, // bottom right
          { .x = -1.f, .y =  1.f, .z = (g.aliens.data[i].idx_x * g.aliens.data[i].width) / 1024.f, .w = (g.aliens.data[i].idx_y * g.aliens.data[i].height) / 1024.f }, // top left

          { .x = -1.f, .y =  1.f, .z = (g.aliens.data[i].idx_x * g.aliens.data[i].width) / 1024.f, .w = (g.aliens.data[i].idx_y * g.aliens.data[i].height) / 1024.f },   // top left
          { .x =  1.f, .y =  1.f, .z = ((g.aliens.data[i].idx_x + 1) * g.aliens.data[i].width) / 1024.f, .w = (g.aliens.data[i].idx_y * g.aliens.data[i].height) / 1024.f },   // top right
          { .x =  1.f, .y = -1.f, .z = ((g.aliens.data[i].idx_x + 1) * g.aliens.data[i].width) / 1024.f, .w = ((g.aliens.data[i].idx_y + 1) * g.aliens.data[i].height) / 1024.f } // bottom right
        }};

    }

    // update vbo with new data
    glBindBuffer(GL_ARRAY_BUFFER, shader_manager.data.at("alien").VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Instance_Data) * g.aliens.instance_data.size(), g.aliens.instance_data.data());
  }

  static void spawn_explosion(Explosions& explosions, const v3 pos)
  {
    explosions.data.emplace_back(Explosion
                                 {
                                   .pos    = v3{ pos.x, pos.y, 0.f },
                                   .vel    = v2{ 1.f, 1.f },
                                   .size   = v2{texture_manager.data.at("explosion").width * 0.1f / Meters_per_pixel,
                                                texture_manager.data.at("explosion").width * 0.1f / Meters_per_pixel },
                                   .life   = 1.f,
                                 });

    explosions.instance_data.emplace_back(Explosion_Instance_Data{});
  }

  static void spawn_player_missiles(Missiles& m, const Player& p)
  {
    static const auto missile_texture = texture_manager.data.at("player_missile");
    static const auto missile_width   = missile_texture.width;
    static const auto missile_height  = missile_texture.height;
    static const auto missile_size_x  = missile_width * 0.3f / Meters_per_pixel;
    static const auto missile_size_y  = missile_height * 0.3f / Meters_per_pixel;

    // spawn a pair
    v2 left = v2{ .x = p.pos.x + 0.1f,
                  .y = p.pos.y + p.size.y };

    v2 right = v2{ .x = p.pos.x + p.size.x - 0.1f,
                   .y = p.pos.y + p.size.y };

    m.data.emplace_back(Missile
                        {
                          .pos    = v3{ left.x , left.y, 0.f },
                          .size   = v2{ missile_size_x, missile_size_y },
                          .vel    = v2{ 0.f, MISSILE_PLAYER_VEL_Y },
                          .width  = (f32)missile_width,
                          .height = (f32)missile_height
                        });

    m.data.emplace_back(Missile
                        {
                          .pos    = v3{ right.x , right.y, 0.f },
                          .size   = v2{ missile_size_x, missile_size_y },
                          .vel    = v2{ 0.f, MISSILE_PLAYER_VEL_Y },
                          .width  = (f32)missile_width,
                          .height = (f32)missile_height
                        });

    m.instance_data.emplace_back(Instance_Data{});

    m.instance_data.emplace_back(Instance_Data{});
  }

  static void update_missiles(Invaders& g, const f32 dt)
  {
    // TODO: this should be in another function
    if(g.player.shooting && g.player.currcd > g.player.shooting_cd) {
      spawn_player_missiles(g.player_missiles, g.player);
      g.player.currcd = 0;
    } else {
      ++g.player.currcd;
    }

    std::set<Entity_Grid_Data, Entity_Grid_Data_Comp> nearby;

    //
    // player missiles
    //
    for(u32 i = 0; i < g.player_missiles.data.size(); ++i) {
      g.player_missiles.data[i].pos.y += g.player_missiles.data[i].vel.y * dt;

      if(g.player_missiles.data[i].pos.y >= g.scene_height_m) {
        g.player_missiles.data[i].destroyed = true;
      }

      if(!g.player_missiles.data[i].destroyed) {
        const v2 missile_pos_px{
          .x = g.player_missiles.data[i].pos.x * Meters_per_pixel,
          .y = g.player_missiles.data[i].pos.y * Meters_per_pixel,
        };

        const auto missile_AABB = AABB{
          .min = v2{ g.player_missiles.data[i].pos.x - g.player_missiles.data[i].size.x,
                     g.player_missiles.data[i].pos.y - g.player_missiles.data[i].size.y },
          .max = v2{ g.player_missiles.data[i].pos.x + g.player_missiles.data[i].size.x,
                     g.player_missiles.data[i].pos.y + g.player_missiles.data[i].size.y }
        };

        get_nearby_ent_type(g.grid, missile_pos_px, Entity_Type::ALIEN, nearby);

        for(const auto& e : nearby) {
          Alien* a = (Alien*)e.data;

          // TODO: should you pass the previous position? before applying vel...
          const auto alien_AABB = AABB{
            .min = v2{ a->pos.x - a->size.x,
                       a->pos.y - a->size.y },
            .max = v2{ a->pos.x + a->size.x,
                       a->pos.y + a->size.y }
          };

          if(aabb_aabb_test(missile_AABB,
                            alien_AABB,
                            v2{ 0.f, g.player_missiles.data[i].vel.y * dt },
                            4)) {
            g.player_missiles.data[i].destroyed = true;
            a->destroyed = true;
            spawn_explosion(g.explosions, a->pos);
          }
        }
      }

      if (g.player_missiles.data[i].destroyed) {
        std::swap(g.player_missiles.data[i], g.player_missiles.data.back());
        std::swap(g.player_missiles.instance_data[i], g.player_missiles.instance_data.back());
        g.player_missiles.data.pop_back();
        g.player_missiles.instance_data.pop_back();
        --i;
        continue;
      }

      g.player_missiles.instance_data[i].vertex_data = {{
          { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
          { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
          { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
          { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f},  // top left
          { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
          { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
        }};

      g.player_missiles.instance_data[i].model = identity();

      g.player_missiles.instance_data[i].model = scale(g.player_missiles.instance_data[i].model,
                                                       v3{
                                                         .x = g.player_missiles.data[i].size.x * Meters_per_pixel,
                                                         .y = g.player_missiles.data[i].size.y * Meters_per_pixel,
                                                         .z = 1.f
                                                       });

      g.player_missiles.instance_data[i].model = translate(g.player_missiles.instance_data[i].model,
                                                           v3{
                                                             .x = g.player_missiles.data[i].pos.x * Meters_per_pixel,
                                                             .y = g.player_missiles.data[i].pos.y * Meters_per_pixel,
                                                             .z = 0.f
                                                           });
    }

    glBindBuffer(GL_ARRAY_BUFFER, shader_manager.data.at("player_missile").VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Instance_Data) * g.player_missiles.instance_data.size(), g.player_missiles.instance_data.data());

    // alien's missiles
    for(u32 i = 0; i < g.alien_missiles.data.size(); ++i) {
      g.alien_missiles.data[i].pos.y += g.alien_missiles.data[i].vel.y * dt;

      if(g.alien_missiles.data[i].pos.y < 0.f) {
        g.alien_missiles.data[i].destroyed = true;
      }

      if(!g.alien_missiles.data[i].destroyed) {
        const v2 missile_pos_px{
          .x = g.alien_missiles.data[i].pos.x * Meters_per_pixel,
          .y = g.alien_missiles.data[i].pos.y * Meters_per_pixel,
        };

        get_nearby_ent_type(g.grid, missile_pos_px, Entity_Type::PLAYER, nearby);

        if(!nearby.empty()) {
          const auto missile_AABB = AABB{
            .min = v2{ g.alien_missiles.data[i].pos.x - g.alien_missiles.data[i].size.x,
                       g.alien_missiles.data[i].pos.y - g.alien_missiles.data[i].size.y },
            .max = v2{ g.alien_missiles.data[i].pos.x + g.alien_missiles.data[i].size.x,
                       g.alien_missiles.data[i].pos.y + g.alien_missiles.data[i].size.y }
          };

          const auto player_AABB = AABB{
            .min = v2{ g.player.pos.x - g.player.size.x, g.player.pos.y - g.player.size.y },
            .max = v2{ g.player.pos.x + g.player.size.x, g.player.pos.y + g.player.size.y }
          };

          if(aabb_aabb_test(missile_AABB,
                            player_AABB,
                            v2{ 0.f, g.alien_missiles.data[i].vel.y * dt },
                            4)) {
            g.alien_missiles.data[i].destroyed = true;
            g.player.destroyed = true;
            spawn_explosion(g.explosions, g.player.pos);
          }
        }
      }

      if (g.alien_missiles.data[i].destroyed) {
        std::swap(g.alien_missiles.data[i], g.alien_missiles.data.back());
        std::swap(g.alien_missiles.instance_data[i], g.alien_missiles.instance_data.back());
        g.alien_missiles.data.pop_back();
        g.alien_missiles.instance_data.pop_back();
        --i;
        continue;
      }

      g.alien_missiles.instance_data[i].vertex_data = {{
          { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
          { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
          { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
          { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f},  // top left
          { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
          { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
        }};

      g.alien_missiles.instance_data[i].model = identity();

      g.alien_missiles.instance_data[i].model = scale(g.alien_missiles.instance_data[i].model,
                                                      v3{
                                                        .x = g.alien_missiles.data[i].size.x * Meters_per_pixel,
                                                        .y = g.alien_missiles.data[i].size.y * Meters_per_pixel,
                                                        .z = 1.f
                                                      });

      g.alien_missiles.instance_data[i].model = translate(g.alien_missiles.instance_data[i].model,
                                                          v3{
                                                            .x = g.alien_missiles.data[i].pos.x * Meters_per_pixel,
                                                            .y = g.alien_missiles.data[i].pos.y * Meters_per_pixel,
                                                            .z = 0.f
                                                          });

    }

    glBindBuffer(GL_ARRAY_BUFFER, shader_manager.data.at("alien_missile").VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Instance_Data) * g.alien_missiles.instance_data.size(), g.alien_missiles.instance_data.data());
  }

  static void update_explosions(Explosions& explosions, const f32 dt)
  {
    for(u32 i{ 0 }; i < explosions.data.size(); ++i) {
      explosions.data[i].life -= dt;
      if(explosions.data[i].life > 0.f) {
        explosions.data[i].pos.x -= explosions.data[i].vel.x * dt;
        explosions.data[i].pos.y -= explosions.data[i].vel.y * dt;
        explosions.instance_data[i].colour.w -= 1.5f * dt;

        explosions.instance_data[i].vertex_data = {{
            { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
            { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
            { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
            { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f},  // top left
            { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
            { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
          }};

        explosions.instance_data[i].model = identity();
        explosions.instance_data[i].model = scale(explosions.instance_data[i].model,
                                                  v3{
                                                    .x = explosions.data[i].size.x * Meters_per_pixel,
                                                    .y = explosions.data[i].size.y * Meters_per_pixel,
                                                    .z = 1.f
                                                  });
        explosions.instance_data[i].model = translate(explosions.instance_data[i].model,
                                                      v3{
                                                        .x = explosions.data[i].pos.x * Meters_per_pixel,
                                                        .y = explosions.data[i].pos.y * Meters_per_pixel,
                                                        .z = 0.f
                                                      });
      } else {
        std::swap(explosions.data[i], explosions.data.back());
        std::swap(explosions.instance_data[i], explosions.instance_data.back());
        explosions.data.pop_back();
        explosions.instance_data.pop_back();
        --i;
      }
    }

    glBindBuffer(GL_ARRAY_BUFFER, shader_manager.data.at("explosion").VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Explosion_Instance_Data) * explosions.instance_data.size(), explosions.instance_data.data());
  }

  static void reset_screen(Invaders& g)
  {
      g.player.pos.x = 0;
      g.player.pos.y = 0;
      g.player_missiles.data.clear();
      g.player_missiles.instance_data.clear();
      g.alien_missiles.data.clear();
      g.alien_missiles.instance_data.clear();
  }

  static void eval_game_state(Invaders& g)
  {
    if(g.state == Game_State::LOST) {

    } else if(g.state == Game_State::WIN_GAME) {
      g.current_level = 0;
      reset_screen(g);
      load_levels(g);
      g.state = Game_State::PLAYING;
    } else if(g.state == Game_State::WIN_LEVEL) {
      ++g.current_level;
      reset_screen(g);
      load_levels(g);
      g.state = Game_State::PLAYING;
    }
  }

  void update(Invaders& g, const f32 dt)
  {
    if(g.state == Game_State::PLAYING) {
      update_player(g, dt);
      update_aliens(g, dt);
      update_missiles(g, dt);
      update_grid(g.grid, g.aliens, g.player);
      update_explosions(g.explosions, dt);
    }

    eval_game_state(g);
  }

  void render(Invaders& g)
  {
    if(g.state == Game_State::PLAYING) {
      glClearColor(1.f, 1.f, 1.f, 0.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glActiveTexture(GL_TEXTURE0);
      static const auto player_shader   = shader_manager.data.at("player");
      static const auto player_texture  = texture_manager.data.at("player");
      static const auto aliens_shader   = shader_manager.data.at("alien");
      static const auto aliens_texture  = texture_manager.data.at("alien");
      static const auto grid_shader     = shader_manager.data.at("grid"); // TEMP
      static const auto bg_shader       = shader_manager.data.at("bg");
      static const auto bg_texture      = texture_manager.data.at("bg");
      static const auto player_missile_shader  = shader_manager.data.at("player_missile");
      static const auto player_missile_texture = texture_manager.data.at("player_missile");
      static const auto alien_missile_shader   = shader_manager.data.at("alien_missile");
      static const auto alien_missile_texture  = texture_manager.data.at("alien_missile");
      static const auto emitter_shader  = shader_manager.data.at("explosion");
      static const auto emitter_texture = texture_manager.data.at("explosion");
      static const auto player_lives_shader  = shader_manager.data.at("player_lives");
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
      // player lives
      use_program(player_lives_shader.id);
      glBindVertexArray(player_lives_shader.VAO);
      glBindTexture(GL_TEXTURE_2D, player_texture.id);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g.player.lives.instance_data.size());
      // player
      use_program(player_shader.id);
      glBindVertexArray(player_shader.VAO);
      glBindTexture(GL_TEXTURE_2D, player_texture.id);
      set_uniform_mat4(player_shader.id, "model", g.player.model);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // explosion
      use_program(emitter_shader.id);
      glBindVertexArray(emitter_shader.VAO);
      glBindTexture(GL_TEXTURE_2D, emitter_texture.id);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g.explosions.instance_data.size());
      // player missiles
      use_program(player_missile_shader.id);
      glBindVertexArray(player_missile_shader.VAO);
      glBindTexture(GL_TEXTURE_2D, player_missile_texture.id);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g.player_missiles.instance_data.size());
      // alien missiles
      use_program(alien_missile_shader.id);
      glBindVertexArray(alien_missile_shader.VAO);
      glBindTexture(GL_TEXTURE_2D, alien_missile_texture.id);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g.alien_missiles.instance_data.size());
      // TEMP grid
      use_program(grid_shader.id);
      glBindVertexArray(grid_shader.VAO);
      glDrawArrays(GL_LINES, 0, gSize / 2);
      glBindVertexArray(0);
    } else if(g.state == Game_State::MENU) {
      static const auto menu_shader = shader_manager.data.at("menu");
      static const auto text_renderer_shader = shader_manager.data.at("text_renderer");
      use_program(menu_shader.id);
      glBindVertexArray(menu_shader.VAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      use_program(text_renderer_shader.id);
      glBindVertexArray(text_renderer_shader.VAO);
      render_text(g.text_renderer, "PLAY",  600.f, 550.f, 1.f, text_renderer_shader.VBO, text_renderer_shader.id, (g.current_menu_option == Menu_Option::PLAY) ? v4{ 1.f, 1.f, 0.f, 1.f } : v4{ 1.f, 1.f, 1.f, 1.f });
      render_text(g.text_renderer, "SOUND", 600.f, 500.f, 1.f, text_renderer_shader.VBO, text_renderer_shader.id, (g.current_menu_option == Menu_Option::SOUND) ? v4{ 1.f, 1.f, 0.f, 1.f } : v4{ 1.f, 1.f, 1.f, 1.f });
      render_text(g.text_renderer, "EXIT",  600.f, 450.f, 1.f, text_renderer_shader.VBO, text_renderer_shader.id, (g.current_menu_option == Menu_Option::EXIT) ? v4{ 1.f, 1.f, 0.f, 1.f } : v4{ 1.f, 1.f, 1.f, 1.f });
    }
  }
};

