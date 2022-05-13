
#ifndef AUDIO_H
#define AUDIO_H
#include <iosfwd>
#include <SDL2/SDL_mixer.h>
#include <iostream>

#include <map>

class AudioWAV {
private:
	int selectedChannel = -1;
	const char* status;
	Mix_Music* chunk = nullptr;
	Mix_Chunk* chunkw = nullptr;
	bool music = false;
	bool loop = false;
	std::string fname;
public:
	static std::map<std::string, AudioWAV*> audiofiles;
	const char
		*SDL_UNABLE_TO_INIT = "sdl_unable_to_init",
		*WAV_FILE_MISSING = "wav_file_missing",
		*WAV_FILE_NULL = "wav_file_null",
		*WAV_SUCCESS = "wav_ok",
		*WAV_UNINIT = "wav_uninitialized",
		*WAV_UNABLE_TO_PLAY = "wav_unable_to_play"
		;
	AudioWAV(std::string filename, std::string audioName = "\0", bool _music = false) {
		if (audioName == "\0") {
			audioName = filename;
		}
		if (audiofiles.contains(audioName)) {
			AudioWAV * w = audiofiles.at(audioName);
			audiofiles.erase(audioName);
			w->operator~();
		}
		fname = filename;
		audiofiles.insert({audioName, this});
		music = _music;
		if (music) {
			chunk = Mix_LoadMUS(filename.c_str());
			status = (chunk == nullptr) ? Mix_GetError() : WAV_SUCCESS;
		}
		else {
			chunkw = Mix_LoadWAV(filename.c_str());
			status = (chunkw == nullptr) ? Mix_GetError() : WAV_SUCCESS;
		}
	}
	AudioWAV() {
		status = WAV_UNINIT;
		chunk = nullptr;
		chunkw = nullptr;
	}
	static AudioWAV* lu_new(std::string _nm) {
		return new AudioWAV(_nm);
	}
	const char* GetError() { return status; }
	void Loop () {
		loop = true;
	}
	void unLoop () {
		loop = false;
	}
	void Stop() {
		(music) ? Mix_HaltMusic() : Mix_HaltChannel(selectedChannel);
	}
	void Play() {
		int channel = selectedChannel;
		bool success;
		if (music) {
			if (chunk != nullptr) {
				success = Mix_PlayMusic(chunk, (loop)?-1:0);
			}
			else { std::cout << "Error playing sound, Music is null!"; }
		}
		else {
			if (chunkw != nullptr) {
				if (Mix_Playing(selectedChannel) || selectedChannel == -2 ) {
					selectedChannel = Mix_PlayChannel(-1, chunkw, (loop)?-1:0);
					Mix_HaltChannel(selectedChannel);
				}
				Mix_PlayChannel(selectedChannel, chunkw, (loop) ? -1 : 0);
			}
			else { std::cout << "Error playing sound, Audio is null!"; }
		}
	}

	bool isPlaying() {
		int channel = selectedChannel;
		return (music)?Mix_PlayingMusic():Mix_Playing (channel); 
	}
	
	void SetVolume(int v) {
		/*VOLUME: 0-128*/
		Mix_Volume(selectedChannel, v);
	}
	int GetVolume() {
		return Mix_Volume(selectedChannel, -1);
	}
	std::string name() {
		return this->fname;
	}
	bool operator ==(std::string o) {
		return (this->fname == o);
	}
	bool operator ==(AudioWAV o) {
	}
	bool operator == (nullptr_t) {
		return ((music && (chunk == nullptr)) && (!music && (chunk==nullptr))) ? true : false;
	}
	bool operator != (nullptr_t) {
		return ((music && (chunk != nullptr)) && (!music && (chunk != nullptr))) ? true : false;
	}
	void operator ~() {
		(music) ? Mix_HaltMusic() : Mix_HaltChannel(selectedChannel);
	}
	void operator !() {
		// Toggle loop
		loop = !loop;
	}

};
std::map<std::string, AudioWAV*> AudioWAV::audiofiles = std::map<std::string, AudioWAV*>();
#endif
