#pragma once

#include <memory>
#include <string>
#include <array>

namespace Game {
  //
  // code that depends on the underlying platform & libs
  //
  void initOpenGLfptrs();

  // ----------------------------------------------------------
  // textures
  // ----------------------------------------------------------
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

  // ----------------------------------------------------------
  // audio
  // ----------------------------------------------------------
  class AudioDevice final { // you know this needs cleanup
  public:
    std::string m_name;
    int m_index;
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
  std::unique_ptr<AudioData> openAudioFile(const char* filepath,
					   const AudioType type);
  void playAudioTrack(AudioData* data);
  void stopAudioTrack(AudioData* data, const unsigned int delay);
  void increaseVolume();
  void decreaseVolume();
  float getNormalizedVolumeValue();
  void closeAudioSystem();

  class ScoreEntry final {
  public:
    std::array<char, 20> m_datetimebuff{ 0 };
    unsigned int m_score{ 0 };
    bool m_currentScore{ false };
  };

  // ----------------------------------------------------------
  // misc
  // ----------------------------------------------------------

  // TRUE -> score in top 5, FALSE -> otherwise
  bool saveAndGetScores(const unsigned int score,
			std::array<ScoreEntry, 5>& scores);

};
