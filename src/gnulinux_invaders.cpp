#include "common.h"
#include "invaders_grid.h"
#include "invaders_input.h"
#include "invaders_level.h"
#include "invaders_menu.h"
#include "invaders_missile.h"
#include "invaders_player.h"
#include "invaders_text.h"
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>
#include <pulse/pulseaudio.h>
#include <iostream>
#include <future>
#include <pulse/sample.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

#include "invaders.h"
#include "invaders_opengl.h"
#include "invaders_timer.h"
#include "event.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

// --------------------------------------------------------
// OpenGL function pointers
// --------------------------------------------------------
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
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;

namespace Game {
  void initOpenGLfptrs()
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
    glUniform3f = (PFNGLUNIFORM3FPROC)getGLProcAddress("glUniform3f");
    glUniform4f = (PFNGLUNIFORM4FPROC)getGLProcAddress("glUniform4f");
  }

  TexInfo loadTexFromFile(const LoadTexFromFileArgs& args)
  {
    if(args.m_flip) {
      stbi_set_flip_vertically_on_load(true);
    }
    int width, height, channels;
    unsigned char* data{ stbi_load(args.m_filepath, &width, &height, &channels, 0) };
    if(!data) {
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
    if(args.m_flip) {
      stbi_set_flip_vertically_on_load(false);
    }
    return Game::TexInfo {
      .m_width = width,
      .m_height = height,
      .m_channels = channels,
      .m_id = id
    };
  }

  std::unique_ptr<AudioData> openAudioFile(const char* filepath)
  {
    AudioData data{
      .m_channels = 0,
      .m_sampleRate = 0,
      .m_samples = 0,
      .m_data = nullptr
    };
    // using a lightweight lib here bc cba dealing w/ audio
    data.m_samples = stb_vorbis_decode_filename(filepath, &data.m_channels, &data.m_sampleRate, &data.m_data);
    if(data.m_samples <= 0) {
      std::cerr << "Error loading audio " << filepath << '\n';
      return nullptr;
    }
    return std::make_unique<AudioData>(data);
  }

  // use pulseaudio async API bc pulseaudio-simple has functions that block the main thread and
  // that's not desired
  // initialise pulseaudio main loop and context, etc
  static pa_threaded_mainloop* mainloop{ nullptr };
  static pa_context* context{ nullptr };

  // this takes forever wtf
  bool initAudioSystem()
  {
    mainloop = pa_threaded_mainloop_new();
    assert(mainloop);
    auto* mainloopAPI = pa_threaded_mainloop_get_api(mainloop);
    assert(mainloopAPI);
    context = pa_context_new(mainloopAPI, "test");
    assert(context);
    pa_context_set_state_callback(context, []([[maybe_unused]]pa_context* ctx, void* data) {
      pa_threaded_mainloop_signal(static_cast<pa_threaded_mainloop*>(data), 0);
    }, mainloop);
    pa_threaded_mainloop_lock(mainloop); // locked!!!!!!!!!!!!!
    pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
    pa_threaded_mainloop_start(mainloop);
    while(true) {
      const auto state = pa_context_get_state(context);
      if(state == PA_CONTEXT_READY) {
        break;
      } else if(state == PA_CONTEXT_TERMINATED || state == PA_CONTEXT_FAILED) {
        pa_threaded_mainloop_unlock(mainloop);
        pa_context_unref(context);
        pa_threaded_mainloop_free(mainloop);
        assert(false);
      }
      pa_threaded_mainloop_wait(mainloop);
    }
    pa_threaded_mainloop_unlock(mainloop); // unlocked!!!!!!!!!!!
    return true;
  }

  void playAudioTrack(AudioData* data)
  {
    auto t = std::thread([data]() {
      pa_threaded_mainloop_lock(mainloop);
      auto sampleSpec = pa_sample_spec{
        .format = PA_SAMPLE_S16LE,
        .rate = static_cast<uint32_t>(data->m_sampleRate),
        .channels = static_cast<uint8_t>(data->m_channels)
      };
      auto* stream = pa_stream_new(context, "audio stream test", &sampleSpec, nullptr);
      if(!stream) {
        pa_threaded_mainloop_unlock(mainloop);
        assert(false);
      }
      pa_stream_set_state_callback(stream, []([[maybe_unused]]pa_stream*, void* data) {
        pa_threaded_mainloop_signal(static_cast<pa_threaded_mainloop*>(data), 0);
      }, mainloop);
      auto bufferAttrs = pa_buffer_attr{
        .maxlength = static_cast<uint32_t>(-1),
        .tlength   = static_cast<uint32_t>(pa_usec_to_bytes(20000, &sampleSpec)),
        .prebuf    = static_cast<uint32_t>(-1),
        .minreq    = static_cast<uint32_t>(pa_usec_to_bytes(10000, &sampleSpec)),
        .fragsize  = static_cast<uint32_t>(-1)
      };
      pa_stream_connect_playback(stream, "alsa_output.usb-C-Media_Electronics_Inc._USB_Advanced_Audio_Device-00.analog-stereo", &bufferAttrs, PA_STREAM_ADJUST_LATENCY, nullptr, nullptr);
      while(true) {
        const auto state = pa_stream_get_state(stream);
        if(state == PA_STREAM_READY) {
          break;
        } else if(state == PA_STREAM_FAILED || state == PA_STREAM_TERMINATED) {
          pa_stream_unref(stream);
          pa_threaded_mainloop_unlock(mainloop);
          assert(false);
        }
        pa_threaded_mainloop_wait(mainloop);
      }
      const auto bytesToWrite = data->m_samples * data->m_channels * sizeof(short);
      // std::clog << "bytesToWrite = " << bytesToWrite << std::endl;
      // assert(bytesToWrite >= 1800000 && bytesToWrite <= 2000000);
      // TODO: optimisation available look at pa wiki
      pa_stream_write(stream, data->m_data, bytesToWrite, nullptr, 0, PA_SEEK_RELATIVE);
      // Wait for the stream to finish playing
      pa_operation* drain_op = pa_stream_drain(stream, [](pa_stream* s, int success, void* userdata) {
        pa_threaded_mainloop_signal(static_cast<pa_threaded_mainloop*>(userdata), 0);
      }, mainloop);
      while (pa_operation_get_state(drain_op) == PA_OPERATION_RUNNING) {
        pa_threaded_mainloop_wait(mainloop);
      }
      pa_operation_unref(drain_op);
      pa_stream_disconnect(stream);
      pa_stream_unref(stream);
      pa_threaded_mainloop_unlock(mainloop);
    });
    t.detach();
  }

  void closeAudioSystem()
  {
    pa_threaded_mainloop_lock(mainloop);
    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_threaded_mainloop_unlock(mainloop);
    pa_threaded_mainloop_stop(mainloop);
    pa_threaded_mainloop_free(mainloop);
  }

};

// !!! One translation unit to speed up compilation !!!
#include "invaders_resources.cpp"
#include "invaders_input.cpp"
#include "invaders_player.cpp"
#include "invaders_enemy.cpp"
#include "invaders_missile.cpp"
#include "invaders_explosion.cpp"
#include "invaders_grid.cpp"
#include "invaders_sim.cpp"
#include "invaders_renderer.cpp"
#include "invaders_level.cpp"
#include "invaders_physics.cpp"
#include "invaders_text.cpp"
#include "invaders_menu.cpp"

// this depends on the underlying platform, in this case, x11, that's why
// this function it's here and not within the InputManager
static Input::Key map_x11_key_to_game(const KeySym& key)
{
  switch(key) {
    case XK_a:      return Input::Key::KEY_A;
    case XK_d:      return Input::Key::KEY_D;
    case XK_q:      return Input::Key::KEY_Q;
    case XK_Escape: return Input::Key::KEY_ESCAPE;
    case XK_space:  return Input::Key::KEY_SPACE;
    case XK_Left:   return Input::Key::KEY_LEFT;
    case XK_Right:  return Input::Key::KEY_RIGHT;
    case XK_Return: return Input::Key::KEY_ENTER;
    case XK_Up:     return Input::Key::KEY_UP;
    case XK_Down:   return Input::Key::KEY_DOWN;
    case XK_w:      return Input::Key::KEY_W;
    case XK_s:      return Input::Key::KEY_S;
    case XK_p:      return Input::Key::KEY_P;
    default:        return Input::Key::KEY_UNKNOWN;
  }
}

int main()
{
  // if we leave sync enabled it will slow down iostream a lot, don't need sync
  // between C and C++ streams anyways
  std::ios::sync_with_stdio(false);
  // first, init OpenGL's function pointers, this is probably specific for each
  // platform, so that's why it's in here; for another platform, say win32, the
  // idea is to have a win32_invaders.cpp and init OpenGL's function pointers in
  // a similar fashion
  Game::initOpenGLfptrs();
  // ---------------------------------------------
  // X11 specific code for init a window with OpenGL.
  // Don't know why X11, maybe should've used Wayland instead, well, cba
  // ---------------------------------------------
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
    GLX_SAMPLE_BUFFERS  , 1,
    GLX_SAMPLES         , 4,
    None
  };
  XSetWindowAttributes setWindowAttrs;
  XWindowAttributes gwa;
  Display* display = XOpenDisplay(NULL);
  if(!display) {
    std::cerr << "Couldn't connect to X server\n";
    std::exit(EXIT_FAILURE);
  }
  // grab best fb
  int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
  int fbcount;
  GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), attrs, &fbcount);
  for(int i = 0; i < fbcount; ++i) {
    XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[i]);
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
  GLXFBConfig bestFbc = fbc[best_fbc];
  // be sure to free the FBConfig list allocated by glXChooseFBConfig()
  XFree(fbc);
  // this root window is needed by X11, but useless for us; the interesting window is the next one
  Window root = DefaultRootWindow(display);
  XVisualInfo* visualInfo = glXGetVisualFromFBConfig(display, bestFbc);
  if(!visualInfo) {
    std::cerr << "Couldn't find appropiate visual info\n";
    std::exit(EXIT_FAILURE);
  }
  Colormap cmap = XCreateColormap(display, root, visualInfo->visual, AllocNone);
  // listening for key presses, releases and exposures
  setWindowAttrs.colormap = cmap;
  setWindowAttrs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;
  Window window = XCreateWindow(display, root, 0, 0,
                                WINDOW_WIDTH, WINDOW_HEIGHT, 0, visualInfo->depth,
                                InputOutput, visualInfo->visual, CWColormap | CWEventMask, &setWindowAttrs);
  XStoreName(display, window, WINDOW_NAME_GNU_LINUX);
  XMapWindow(display, window);
  // disable resizing because don't know how to handle game units correctly yet, which
  // means everything will use pixels. That being the case, if resizing is enabled, it
  // could mess things up
  XSizeHints hints;
  hints.flags = PMinSize | PMaxSize;
  hints.min_width = hints.max_width = WINDOW_WIDTH;
  hints.min_height = hints.max_height = WINDOW_HEIGHT;
  XSetWMNormalHints(display, window, &hints);
  // closing window gracefully when user clicks on the X btn
  Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wmDeleteMessage, 1);
  // pretty important, setup opengl context for renderdoc
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB =
      (glXCreateContextAttribsARBProc) glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");
  int contextAttribs[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
      GLX_CONTEXT_MINOR_VERSION_ARB, 3,
      GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
      None
  };
  // !!!renderdoc!!!
  GLXContext glContext = glXCreateContextAttribsARB(display, bestFbc, 0, True, contextAttribs);
  XSync(display, False);
  glXMakeCurrent(display, window, glContext);
  // enable VSync
  glXSwapIntervalEXT(display, window, 1);
  XEvent xev;
  // once OpenGL and X11 are ready to fight, game init takes place.
  // init all resource managers game has. If there's any error initialising them,
  // the game will straight up close with a diagnostic msg
  Ev::EventManager eventManager;
  Res::ResourceManager resourceManager;
  Input::InputManager inputManager;
  Game::ExplosionManager explosionManager(resourceManager);
  Game::GridManager gridManager(WINDOW_WIDTH, WINDOW_HEIGHT);
  Game::MissileManager missileManager(resourceManager, gridManager, explosionManager, eventManager);
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
  // constexpr float kFpsMax = 1000.0f / 144.0f; // 144fps is the cap
  float lastFrame = 0.0f;
  while(!simulationManager.shouldEnd()) {
    // computing the delta time like this is a good idea, but remember that this delta
    // time corresponds to the previous frame, not the current one, so that's problematic
    // if this frame runs slower than the previous one. To solve this, if this frame ends
    // faster than expected, you put it to sleep for the remaining duration, effectively
    // making all frames render at the same time.
    float currentFrame = Game::time();
    float delta = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glViewport(0, 0, gwa.width, gwa.height);
    inputManager.beginFrame();
    // polling for x11 window events
    while(XPending(display)) {
      XNextEvent(display, &xev);
      if(xev.type == KeyPress || xev.type == KeyRelease) {
        const bool is_pressed = xev.type == KeyPress;
        const KeySym x_key = XLookupKeysym(&xev.xkey, 0);
        const Input::Key key = map_x11_key_to_game(x_key);
        inputManager.updateKey(key, is_pressed);
      } else if(xev.type == ClientMessage) {
        if(static_cast<Atom>(xev.xclient.data.l[0]) == wmDeleteMessage) {
          simulationManager.setShouldEnd(true);
        }
      }
    }
    // the simulation manager will update the world and then it'll call the renderer
    simulationManager.update(delta);
    XGetWindowAttributes(display, window, &gwa);
    glXSwapBuffers(display, window);
    // put main thread to sleep if necessary, ensuring consistent delta times
    // const float frameTime = Game::time() - currentFrame;
    // if(frameTime < kFpsMax) {
    //   // turns out this could be a problem bc people say it's not accurate. Don't know
    //   // what that really means...
    //   std::this_thread::sleep_for(std::chrono::duration<float>(kFpsMax - frameTime));
    // }
  }
  glXMakeCurrent(display, None, nullptr);
  glXDestroyContext(display, glContext);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  return EXIT_SUCCESS;
}
