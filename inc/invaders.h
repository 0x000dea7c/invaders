#pragma once

#include <memory>
#include <string>
#include <array>

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

  //
  // audio, you know this needs cleanup, you know ittttttttttttttttttttttttt
  //
  class AudioDevice final {
  public:
    std::string m_name;
    int m_index; // maybe this interface is not enough for windows or some other platforms
  };

  enum class AudioType {
    MUSIC,
    EFFECT
  };

  class AudioData final {
  public:
    void* m_data;
    AudioType m_type;
  };

  std::vector<AudioDevice> getAudioDevices();
  bool initAudioSystem(const AudioDevice& audioDevice);
  std::unique_ptr<AudioData> openAudioFile(const char* filepath, const AudioType type);
  void playAudioTrack(AudioData* data);
  void stopAudioTrack(AudioData* data, const unsigned int delay);
  void increaseVolume();
  void decreaseVolume();
  float getNormalizedVolumeValue();
  void closeAudioSystem();

  // misc functions that need to know about the underlying OS (probably)
  // even though you're using std::fstream, people say that it's too slow, so that's why
  // it's in here, in case you want to use underlying OS or a lib to improve perf
  class ScoreEntry final {
  public:
    std::array<char, 21> m_datetimebuff{ 0 };
    unsigned int m_score{ 0 };
    bool m_currentScore{ false };
  };

  void saveAndGetScores(const unsigned int score, std::array<ScoreEntry, 5>& scores);

};
