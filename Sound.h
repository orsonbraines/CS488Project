#pragma once

#include <string>
#include <SDL.h>

class AudioDevice;

class Sound {
public:
	Sound(const std::string &wavFileName);
	~Sound();
private:
	SDL_AudioSpec m_audioSpec;
	Uint8* m_audioBuf;
	Uint32 m_audioLen;

	friend class AudioDevice;
};

class AudioDevice {
public:
	AudioDevice();
	~AudioDevice();
	void playSound(const Sound& sound);
private:
	SDL_AudioDeviceID m_deviceId;
};