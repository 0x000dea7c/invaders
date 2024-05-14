#pragma once

#include <memory>

namespace Game {
  // code that depends on the underlying platform

  void initOpenGLfptrs();

  class TexInfo final {
  public:
    int m_width;
    int m_height;
    int m_channels;
    unsigned int m_id;
  };

  class LoadTexFromFileArgs final {
  public:
    const char* m_filepath;
    bool m_alpha;
    bool m_flip;
    int m_wrapS;
    int m_wrapT;
  };

  TexInfo loadTexFromFile(const LoadTexFromFileArgs& args);

  class AudioData final {
  public:
    int m_channels;
    int m_sampleRate;
    int m_samples;
    short* m_data;
  };

  // returns TRUE -> initialised correctly, FALSE otherwise
  bool initAudioSystem();
  std::unique_ptr<AudioData> openAudioFile(const char* filepath);
  void playAudioTrack(AudioData* data);
  void closeAudioSystem();
};
