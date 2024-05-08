#pragma once

namespace Input {

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
    KEY_P,

    KEY_COUNT
  };

  // the only goal of this class is to manage and store key states.
  // Since in this game you only need a keyboard, that's all it has.
  class InputManager {
  public:
    InputManager();
    ~InputManager();
    void init();
    void close();
    bool isKeyPressed(const Key k) const;
    bool isKeyHeld(const Key k) const;
    // store in prevkeys the contents of currkeys and update currkeys with recent user's input.
    // This is needed to determine if a key is being pressed or held. If you didn't do that, a single
    // key press would be very hard to get right because it "extends" through multiple frames.
    void beginFrame();
    // update key state
    void updateKey(const Key k, const bool pressed);
  private:
    // presses, of course
    bool currkeys[Key::KEY_COUNT];
    bool prevkeys[Key::KEY_COUNT];
  };

};
