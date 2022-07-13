#include "Sound.h"
#include "Util.h"

#include <cassert>

Sound::Sound(const std::string& wavFileName) {
	if (SDL_LoadWAV(wavFileName.c_str(), &m_audioSpec, &m_audioBuf, &m_audioLen) == nullptr) {
		const char* errMsg = SDL_GetError();
		throw GraphicsException(errMsg);
	}
}

Sound::~Sound() {
	SDL_FreeWAV(m_audioBuf);
}

AudioDevice::AudioDevice() {
	SDL_AudioSpec want, have;

	SDL_zero(want);
	want.freq = 44100;
	want.format = AUDIO_F32;
	want.channels = 2;
	want.samples = 4096;
	m_deviceId = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	if (m_deviceId == 0) {
		const char* errMsg = SDL_GetError();
		throw GraphicsException(errMsg);
	}

	SDL_PauseAudioDevice(m_deviceId, 0);
}

AudioDevice::~AudioDevice() {
	SDL_CloseAudioDevice(m_deviceId);
}

void AudioDevice::playSound(const Sound &sound) {
	SDL_QueueAudio(m_deviceId, sound.m_audioBuf, sound.m_audioLen);
}