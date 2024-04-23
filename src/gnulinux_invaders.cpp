#include <iostream>
#include <cstdlib>
#include <array>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "invaders_opengl.h"
#include "invaders_timer.h"
#include "invaders.h"

// !!! One translation unit !!!
#include "invaders_shaders.cpp"
#include "invaders_world.cpp"
#include "invaders.cpp"

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLUNIFORM1FPROC glUniform1f;

void Game::init_OpenGL_fptrs()
{
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)getGLProcAddress("glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)getGLProcAddress("glUniform1i");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)getGLProcAddress("glGenVertexArrays");
    glGenBuffers = (PFNGLGENBUFFERSPROC)getGLProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)getGLProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)getGLProcAddress("glBufferData");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)getGLProcAddress("glBindVertexArray");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)getGLProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)getGLProcAddress("glVertexAttribPointer");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)getGLProcAddress("glDeleteBuffers");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)getGLProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getGLProcAddress("glGetShaderInfoLog");
    glCreateShader = (PFNGLCREATESHADERPROC)getGLProcAddress("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)getGLProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)getGLProcAddress("glCompileShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)getGLProcAddress("glDeleteShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)getGLProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)getGLProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)getGLProcAddress("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)getGLProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)getGLProcAddress("glGetProgramInfoLog");
    glUseProgram = (PFNGLUSEPROGRAMPROC)getGLProcAddress("glUseProgram");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)getGLProcAddress("glGenerateMipmap");
    glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) glXGetProcAddress((const GLubyte*)"glDeleteVertexArrays");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) glXGetProcAddress((const GLubyte*)"glUniformMatrix4fv");
    glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)getGLProcAddress("glDrawArraysInstanced");
    glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)getGLProcAddress("glVertexAttribDivisor");
    glBufferSubData = (PFNGLBUFFERSUBDATAPROC)getGLProcAddress("glBufferSubData");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)getGLProcAddress("glDeleteProgram");
    glUniform1f = (PFNGLUNIFORM1FPROC)getGLProcAddress("glUniform1f");
}

// definitions of invaders.h
namespace Game {
    Texture_Info load_texture(char const* filepath,
                              const bool alpha,
                              const bool flip,
                              const i32 wrap_s,
                              const i32 wrap_t)
    {
        if(flip) {
            stbi_set_flip_vertically_on_load(true);
        }

        int width, height, channels;
        unsigned char* data{ stbi_load(filepath, &width, &height, &channels, 0) };
        if(!data) {
            std::cerr << "failed loading image\n";
            exit(EXIT_FAILURE);
        }

        // don't assume dimensions are multiple of 4
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        u32 id;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexImage2D(GL_TEXTURE_2D, 0, (alpha) ? GL_RGBA : GL_RGB, width, height, 0, (alpha) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        if(flip) {
            stbi_set_flip_vertically_on_load(false);
        }

        return {
            .width = width,
            .height = height,
            .channels = channels,
            .id = id
        };
    }
};

static Game::Key
map_x11_key_to_game(const KeySym& key)
{
    switch(key) {
    case XK_a: return Game::KEY_A;
    case XK_d: return Game::KEY_D;
    case XK_q: return Game::KEY_Q;
    case XK_Escape: return Game::KEY_ESCAPE;
    case XK_space: return Game::KEY_SPACE;
    case XK_Left: return Game::KEY_LEFT;
    case XK_Right: return Game::KEY_RIGHT;
    case XK_Return: return Game::KEY_ENTER;
    case XK_Up: return Game::KEY_UP;
    case XK_Down: return Game::KEY_DOWN;
    case XK_w: return Game::KEY_W;
    case XK_s: return Game::KEY_S;
    default: return Game::KEY_UNHANDLED;
    }
}

int main()
{
    std::ios::sync_with_stdio(false);

    Game::init_OpenGL_fptrs();

    // get a matching fb config
    static int attrs[] = {
        GLX_X_RENDERABLE    , True,
        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
        GLX_RED_SIZE        , 8,
        GLX_GREEN_SIZE      , 8,
        GLX_BLUE_SIZE       , 8,
        GLX_ALPHA_SIZE      , 8,
        GLX_DEPTH_SIZE      , 24,
        GLX_STENCIL_SIZE    , 8,
        GLX_DOUBLEBUFFER    , True,
        //GLX_SAMPLE_BUFFERS  , 1,
        //GLX_SAMPLES         , 4,
        None
    };

    XSetWindowAttributes setWindowAttrs;
    XWindowAttributes gwa;

    Display* display{ XOpenDisplay(NULL) };

    if(!display) {
        std::cerr << "Couldn't connect to X server\n";
        exit(EXIT_FAILURE);
    }

    // grab best fb
    int best_fbc{ -1 }, worst_fbc{ -1 }, best_num_samp{ -1 }, worst_num_samp{ 999 };
    int fbcount;
    GLXFBConfig* fbc{ glXChooseFBConfig(display, DefaultScreen(display), attrs, &fbcount) };
    for(int i{ 0 }; i < fbcount; ++i) {
        XVisualInfo* vi{ glXGetVisualFromFBConfig(display, fbc[i]) };

        if(vi) {
            int samp_buf, samples;
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES       , &samples  );

            if(best_fbc < 0 || (samp_buf && samples > best_num_samp)) {
                best_fbc = i;
                best_num_samp = samples;
            }

            if(worst_fbc < 0 || !samp_buf || samples < worst_num_samp) {
                worst_fbc = i;
                worst_num_samp = samples;
            }
        }

        XFree(vi);
    }

    GLXFBConfig bestFbc{ fbc[ best_fbc ] };

    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree( fbc );

    // this root window is kinda useless, but needed, the interesting window is the other one
    Window root{ DefaultRootWindow(display) };

    XVisualInfo* visualInfo{ glXGetVisualFromFBConfig(display, bestFbc) };

    if(!visualInfo) {
        std::cerr << "Couldn't find appropiate visual\n";
        exit(EXIT_FAILURE);
    }

    Colormap cmap{ XCreateColormap(display, root, visualInfo->visual, AllocNone) };

    // listening for key presses, releases and exposures (window resizes and such)
    setWindowAttrs.colormap = cmap;
    setWindowAttrs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;

    Window window{ XCreateWindow(display,
                                 root,
                                 0,
                                 0,
                                 WINDOW_WIDTH,
                                 WINDOW_HEIGHT,
                                 0,
                                 visualInfo->depth,
                                 InputOutput,
                                 visualInfo->visual,
                                 CWColormap | CWEventMask,
                                 &setWindowAttrs) };

    XStoreName(display, window, "Invaders");

    XMapWindow(display, window);

    // pretty important, setup opengl context for renderdoc
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBProc) glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

    int contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        None
    };

    GLXContext glContext{ glXCreateContextAttribsARB(display, bestFbc, 0, True, contextAttribs) };
    XSync(display, False);

    glXMakeCurrent(display, window, glContext);

    // Enable VSync
    glXSwapIntervalEXT(display, window, 1);

    XEvent xev;

    //
    // game stuff starts here
    //
    Game::Invaders game;

    game.scene_width_m   = WINDOW_WIDTH / Game::Meters_per_pixel;
    game.scene_height_m  = WINDOW_HEIGHT / Game::Meters_per_pixel;
    game.scene_width_px  = WINDOW_WIDTH;
    game.scene_height_px = WINDOW_HEIGHT;

    Game::init(game);

    f32 last_frame{ 0.f };
    while(game.is_running) {
        f32 current_frame{ Game::time() };
        f32 dt{ current_frame - last_frame };
        last_frame = current_frame;

        glViewport(0, 0, gwa.width, gwa.height);

        while(XPending(display)) {
            XNextEvent(display, &xev);

            if(xev.type == KeyPress || xev.type == KeyRelease) {
                const bool is_pressed{ xev.type == KeyPress };
                const KeySym x_key{ XLookupKeysym(&xev.xkey, 0) };
                const Game::Key key{ map_x11_key_to_game(x_key) };
                game.keys[key] = is_pressed;
            }
        }

        Game::process_input(game);
        Game::update(game, dt);
        Game::render(game, dt);

        XGetWindowAttributes(display, window, &gwa);

        glXSwapBuffers(display, window);
    }

    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, glContext);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return EXIT_SUCCESS;
}
