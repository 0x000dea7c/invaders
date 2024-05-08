#include "invaders_missile.h"
#include "invaders_player.h"
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "invaders_opengl.h"
#include "invaders_timer.h"
#include "invaders.h"

// !!! One translation unit to speed up compilation !!!
#include "invaders_input.cpp"
#include "invaders_sim.cpp"
#include "invaders_resources.cpp"
#include "invaders_renderer.cpp"

using namespace Game;

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
PFNGLUNIFORM4FPROC glUniform4f;

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
    glUniform4f = (PFNGLUNIFORM4FPROC)getGLProcAddress("glUniform4f");
}

TexInfo loadTexFromFile(const LoadTexFromFileArgs& args)
{
    if(args.flip) {
      stbi_set_flip_vertically_on_load(true);
    }
    int width, height, channels;
    unsigned char* data{ stbi_load(args.filepath, &width, &height, &channels, 0) };
    if(!data) {
      std::cerr << __FUNCTION__ << " failed loading image -> " << args.filepath << '\n';
      exit(EXIT_FAILURE);
    }
    // don't assume dimensions are multiple of 4
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 (args.alpha) ? GL_RGBA : GL_RGB,
                 width,
                 height,
                 0,
                 (args.alpha) ? GL_RGBA : GL_RGB
                 ,
                 GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, args.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, args.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
    if(args.flip) {
      stbi_set_flip_vertically_on_load(false);
    }
    return TexInfo {
        .width = width,
        .height = height,
        .channels = channels,
        .id = id
    };
}

// this depends on the underlying platform, in this case, x11, that's why
// this function it's here and not within the InputManager
static Input::Key map_x11_key_to_game(const KeySym& key)
{
    switch(key) {
    case XK_a: return Input::KEY_A;
    case XK_d: return Input::KEY_D;
    case XK_q: return Input::KEY_Q;
    case XK_Escape: return Input::KEY_ESCAPE;
    case XK_space: return Input::KEY_SPACE;
    case XK_Left: return Input::KEY_LEFT;
    case XK_Right: return Input::KEY_RIGHT;
    case XK_Return: return Input::KEY_ENTER;
    case XK_Up: return Input::KEY_UP;
    case XK_Down: return Input::KEY_DOWN;
    case XK_w: return Input::KEY_W;
    case XK_s: return Input::KEY_S;
    case XK_p: return Input::KEY_P;
    default: assert(false && "map_x11_key_to_game -> This should not happen");
    }
}

// implementing something more coherent like an ECS
// requires a good understanding of game arch, which I don't have at the moment.
// The reason why this works decently is because this game is small, there are no
// tests and it won't scale.
Res::ResourceManager      gResourceManager;
Input::InputManager       gInputManager;
Game::PlayerManager       gPlayerManager;
Game::EnemyManager        gEnemyManager;
Game::MissileManager      gMissileManager;
Game::ExplosionManager    gExplosionManager;
Sim::SimulationManager    gSimulationManager;
Renderer::RendererManager gRenderManager;
// AudioManager      gAudioManager;

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
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
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
  gInputManager.init();
  gResourceManager.init();
  gMissileManager.init();
  gExplosionManager.init();
  gSimulationManager.init();
  gRenderManager.init();
  // gAudioManager.init();
  // run the game
  constexpr float kFpsMax = 1000.0f / 144.0f; // 144fps is the cap
  float lastFrame = 0.0f;
  while(!gSimulationManager.shouldEnd()) {
    // computing the delta time like this is a good idea, but remember that this delta
    // time corresponds to the previous frame, not the current one, so that's problematic
    // if this frame runs slower than the previous one. To solve this, if this frame ends
    // faster than expected, you put it to sleep for the remaining duration, effectively
    // making all frames render at the same time.
    float currentFrame = Game::time();
    float delta = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glViewport(0, 0, gwa.width, gwa.height);
    gInputManager.beginFrame();
    // polling for x11 window events
    while(XPending(display)) {
      XNextEvent(display, &xev);
      if(xev.type == KeyPress || xev.type == KeyRelease) {
        const bool is_pressed = xev.type == KeyPress;
        const KeySym x_key = XLookupKeysym(&xev.xkey, 0);
        const Input::Key key = map_x11_key_to_game(x_key);
        gInputManager.updateKey(key, is_pressed);
      }
    }
    // the simulation manager will update the world and then it'll call the renderer
    gSimulationManager.update(delta);
    XGetWindowAttributes(display, window, &gwa);
    glXSwapBuffers(display, window);
    // put main thread to sleep if necessary, ensuring consistent delta times
    const float frameTime = Game::time() - currentFrame;
    if(frameTime < kFpsMax) {
      // turns out this could be a problem bc people say it's not accurate. Don't know
      // what that really means...
      std::this_thread::sleep_for(std::chrono::duration<float>(kFpsMax - frameTime));
    }
  }
  // there might not be a "need" to release resources in PCs because the OS will do
  // all the cleanup, but it's considered a good practice, so might as well do it.
  // If there was a situation where cleanup would slow down too much the game's closing
  // procedure, perhaps it's would be a good idea to get rid of these function calls.
  gRenderManager.close();
  gSimulationManager.close();
  gExplosionManager.close();
  gMissileManager.close();
  gResourceManager.close();
  gInputManager.close();
  // gAudioManager.close();
  glXMakeCurrent(display, None, nullptr);
  glXDestroyContext(display, glContext);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  return EXIT_SUCCESS;
}
