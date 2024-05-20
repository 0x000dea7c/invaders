#define _CRT_SECURE_NO_WARNINGS

// gotta use glad bc win is a mess
#include "glad/glad.h"
#include "glad.c"

#include "common.h"
#include "event.h"
#include "invaders.h"
#include "invaders_opengl.h"
#include "invaders_timer.h"
#include "invaders_grid.h"
#include "invaders_input.h"
#include "invaders_level.h"
#include "invaders_menu.h"
#include "invaders_missile.h"
#include "invaders_player.h"
#include "invaders_text.h"
#include "invaders_sim.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <SDL.h>
#include <SDL_mixer.h>

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>

// platform impl
namespace Game {

    TexInfo loadTexFromFile(const LoadTexFromFileArgs& args)
    {
        if (args.m_flip) {
            stbi_set_flip_vertically_on_load(true);
        }
        int width, height, channels;
        unsigned char* data{ stbi_load(args.m_filepath, &width, &height, &channels, 0) };
        if (!data) {
            std::cerr << __FUNCTION__ << " failed loading image -> " << args.m_filepath << '\n';
            exit(EXIT_FAILURE);
        }
        // don't assume dimensions are multiple of 4
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        unsigned int id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D,
            0,
            (args.m_alpha) ? GL_RGBA : GL_RGB,
            width,
            height,
            0,
            (args.m_alpha) ? GL_RGBA : GL_RGB
            ,
            GL_UNSIGNED_BYTE,
            data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, args.m_wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, args.m_wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
        if (args.m_flip) {
            stbi_set_flip_vertically_on_load(false);
        }
        return Game::TexInfo{
          .m_width = width,
          .m_height = height,
          .m_channels = channels,
          .m_id = id
        };
    }

    static float volume{ MIX_MAX_VOLUME };

    std::unique_ptr<AudioData> openAudioFile(const char* filepath, const AudioType type)
    {
        AudioData data{
            .m_data = nullptr,
            .m_type = type
        };
        switch (data.m_type) {
        case AudioType::MUSIC:
            data.m_data = Mix_LoadMUS(filepath);
            if (!data.m_data) {
                std::cerr << __FUNCTION__ << ": couldn't open file " << filepath << ':' << Mix_GetError() << '\n';
                return nullptr;
            }
            break;
        case AudioType::EFFECT:
            data.m_data = Mix_LoadWAV(filepath);
            if (!data.m_data) {
                std::cerr << __FUNCTION__ << ": couldn't open file " << filepath << ':' << Mix_GetError() << '\n';
                return nullptr;
            }
            break;
        }
        return std::make_unique<AudioData>(data);
    }

    bool initAudioSystem(const AudioDevice& audioDevice)
    {
        if (Mix_OpenAudioDevice(44100, MIX_DEFAULT_FORMAT, 2, 2048, audioDevice.m_name.c_str(), 0) < 0) {
            std::cerr << __FUNCTION__ << ": couldn't open audio device: " << Mix_GetError() << '\n';
            return false;
        }
        Mix_AllocateChannels(32);
        return true;
    }

    std::vector<AudioDevice> getAudioDevices()
    {
        std::vector<AudioDevice> res;
        for (int i{ 0 }; i < SDL_GetNumAudioDevices(0); ++i) {
            res.emplace_back(AudioDevice{ .m_name = std::string(SDL_GetAudioDeviceName(i, 0)), .m_index = i });
        }
        return res;
    }

    void playAudioTrack(AudioData* data, const bool loop)
    {
        switch (data->m_type) {
        case AudioType::MUSIC:
            // @NOTE: beware! only one music can be played at a time!
            // -1 means loop forevah
            Mix_PlayMusic(reinterpret_cast<Mix_Music*>(data->m_data), (loop) ? -1 : 0);
            break;
        case AudioType::EFFECT:
            // you can play as many effects as you want
            // -1: play on the first free channel you find
            //  0; -1: play once and stop; play in loop
            if (Mix_PlayChannel(-1, reinterpret_cast<Mix_Chunk*>(data->m_data), (loop) ? -1 : 0) == -1) {
                assert(false && "audio couldn't be played!!!!!!!!!!!!!!!!!!");
            }
            break;
        }
    }

    void stopAudioTrack(AudioData* data, const unsigned int delay)
    {
        // you only need to stop main music for now
        switch (data->m_type) {
        case AudioType::MUSIC:
            Mix_FadeOutMusic(delay);
            break;
        default:
            break;
        }
    }

    void increaseVolume()
    {
        volume = min(volume + 13.3f, static_cast<float>(MIX_MAX_VOLUME));
        Mix_MasterVolume(static_cast<int>(volume));
        Mix_VolumeMusic(static_cast<int>(volume));
    }

    void decreaseVolume()
    {
        volume = max(volume - 13.3f, 0.0f);
        Mix_MasterVolume(static_cast<int>(volume));
        Mix_VolumeMusic(static_cast<int>(volume));
    }

    float getNormalizedVolumeValue()
    {
        return std::round((volume / MIX_MAX_VOLUME) * 10.f) / 10.f;
    }

    void closeAudioSystem()
    {
        Mix_CloseAudio();
        SDL_Quit();
    }

    void closeAudioFile(AudioData* data)
    {
        if (data->m_type == AudioType::MUSIC) {
            Mix_FreeMusic(reinterpret_cast<Mix_Music*>(data->m_data));
        }
        else {
            Mix_FreeChunk(reinterpret_cast<Mix_Chunk*>(data->m_data));
        }
    }

    // the file is always sorted and always contains max 5 lines
    bool saveAndGetScores(const unsigned int score, std::array<ScoreEntry, 5>& scores)
    {
        // @YOLO: can definitely be improved in many ways
        const auto t = std::chrono::system_clock::now(); // boring bit
        const auto timeT = std::chrono::system_clock::to_time_t(t); // boring bit
        const auto localTime = *std::localtime(&timeT); // boring bit
        std::stringstream ss;	// cut this out
        ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S"); // cut this out
        const auto timeStr = ss.str(); // cut this out
        std::ifstream fileread("scoreboard.dat", std::ios::binary);
        if (fileread.is_open()) {
            // if the file already exists, load scores
            for (unsigned int i{ 0 }; i < scores.size(); ++i) {
                fileread.read(reinterpret_cast<char*>(scores[i].m_datetimebuff.data()), sizeof(scores[i].m_datetimebuff));
                fileread.read(reinterpret_cast<char*>(&scores[i].m_score), sizeof(scores[i].m_score));
                scores[i].m_currentScore = false;
            }
        }
        bool topfive{ false };
        auto it = std::find_if(scores.begin(), scores.end(), [score](const ScoreEntry& a) { return score > a.m_score; });
        if (it != scores.end()) {
            topfive = true;
        }
        // since the file is sorted at all times, you can just replace the last element and then sort
        if (topfive) {
            scores[4].m_score = score;
            scores[4].m_currentScore = true;
            std::copy(timeStr.begin(), timeStr.end(), scores[4].m_datetimebuff.begin());
            std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
                return a.m_score > b.m_score;
            });
        }
        // write back
        std::ofstream filewrite("scoreboard.dat", std::ios::binary);
        if (filewrite.is_open()) {
            for (const auto& e : scores) {
                filewrite.write(reinterpret_cast<const char*>(e.m_datetimebuff.data()), sizeof(e.m_datetimebuff));
                filewrite.write(reinterpret_cast<const char*>(&e.m_score), sizeof(e.m_score));
            }
        }
        else {
            std::clog << __FUNCTION__ << ": couldn't save scoreboard.\n";
        }
        return topfive;
    }

};

// one bloody translation file
#include "../src/invaders_resources.cpp"
#include "../src/invaders_input.cpp"
#include "../src/invaders_player.cpp"
#include "../src/invaders_enemy.cpp"
#include "../src/invaders_missile.cpp"
#include "../src/invaders_explosion.cpp"
#include "../src/invaders_grid.cpp"
#include "../src/invaders_sim.cpp"
#include "../src/invaders_renderer.cpp"
#include "../src/invaders_level.cpp"
#include "../src/invaders_text.cpp"
#include "../src/invaders_menu.cpp"

Input::Key map_sdl_key_to_game(SDL_Keycode key)
{
    switch (key) {
    case SDLK_a:      return Input::Key::KEY_A;
    case SDLK_d:      return Input::Key::KEY_D;
    case SDLK_q:      return Input::Key::KEY_Q;
    case SDLK_ESCAPE: return Input::Key::KEY_ESCAPE;
    case SDLK_SPACE:  return Input::Key::KEY_SPACE;
    case SDLK_LEFT:   return Input::Key::KEY_LEFT;
    case SDLK_RIGHT:  return Input::Key::KEY_RIGHT;
    case SDLK_RETURN: return Input::Key::KEY_ENTER;
    case SDLK_UP:     return Input::Key::KEY_UP;
    case SDLK_DOWN:   return Input::Key::KEY_DOWN;
    case SDLK_w:      return Input::Key::KEY_W;
    case SDLK_s:      return Input::Key::KEY_S;
    case SDLK_p:      return Input::Key::KEY_P;
    default:          return Input::Key::KEY_UNKNOWN;
    }
}

int main(int argc, char* argv[])
{
    std::ios::sync_with_stdio(false);
    SDL_Init(SDL_INIT_EVERYTHING);
    // cba dealing with windows' API
    auto* window = SDL_CreateWindow(WINDOW_NAME_WIN32, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    if (window == nullptr) {
        std::cerr << "Couldn't create SDL window!\n";
        return EXIT_FAILURE;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    auto context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!\n";
        return EXIT_FAILURE;
    }
    SDL_GL_SetSwapInterval(1);
    Ev::EventManager eventManager;
    Res::ResourceManager resourceManager(eventManager);
    Input::InputManager inputManager;
    Game::ExplosionManager explosionManager(resourceManager, eventManager);
    Game::GridManager gridManager(WINDOW_WIDTH, WINDOW_HEIGHT, eventManager);
    Game::MissileManager missileManager(resourceManager, gridManager, eventManager);
    Game::PlayerManager playerManager(resourceManager, inputManager, missileManager, gridManager, eventManager);
    Game::EnemyManager enemyManager(resourceManager, missileManager, gridManager, eventManager);
    Game::LevelManager levelManager(enemyManager);
    Renderer::TextRenderer textRenderer("./res/fonts/FreeMonoBold.ttf",
        28,
        resourceManager);
    Game::MenuManager menuManager(eventManager, inputManager);
    Renderer::RendererManager renderManager(resourceManager, textRenderer, menuManager);
    Sim::SimulationManager simulationManager(resourceManager,
        inputManager,
        playerManager,
        enemyManager,
        missileManager,
        explosionManager,
        gridManager,
        renderManager,
        menuManager,
        eventManager,
        levelManager,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);
    // run the game
    auto lastFrame = 0.0f;
    while (!simulationManager.shouldEnd()) {
        // computing the delta time like this is a good idea, but remember that this delta
        // time corresponds to the previous frame, not the current one, so that's problematic
        // if this frame runs slower than the previous one. To solve this, if this frame ends
        // faster than expected, you put it to sleep for the remaining duration, effectively
        // making all frames render at the same time, howver not doing it for now
        auto currentFrame = Game::time();
        auto delta = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        inputManager.beginFrame();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                simulationManager.setShouldEnd(true);
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                bool is_pressed = event.type == SDL_KEYDOWN;
                auto key = map_sdl_key_to_game(event.key.keysym.sym);
                inputManager.updateKey(key, is_pressed);
            }
        }
        // the simulation manager will update the world and then it'll call the renderer
        simulationManager.update(delta);
        SDL_GL_SwapWindow(window);
    }
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    return EXIT_SUCCESS;
}