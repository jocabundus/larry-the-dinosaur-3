#ifndef FAKE_FMOD_H
#define FAKE_FMOD_H

#include <SDL_mixer.h>

#define FSOUND_SAMPLE Mix_Chunk
#define FMUSIC_MODULE Mix_Music

#define FMUSIC_SetPaused(a, b) do { if (b) { Mix_PauseMusic(); } else { Mix_ResumeMusic(); } } while (0)
#define FMUSIC_LoadSong(filename) Mix_LoadMUS(filename)
#define FMUSIC_LoadSongEx(a,b,c,d,e,f) Mix_LoadMUS(a)
#define FMUSIC_PlaySong(a) Mix_PlayMusic(a, -1)
#define FMUSIC_StopSong(sng) Mix_HaltMusic()
#define FMUSIC_StopAllSongs() Mix_HaltMusic()
#define FMUSIC_FreeSong(sng) Mix_FreeMusic(sng)
#define FMUSIC_SetMasterVolume(sng, vol) Mix_VolumeMusic(vol / 2)
#define FMUSIC_SetLooping(sng, val) do { printf("STUB: FMUSIC_SetLooping(%p, %d)\n", sng, val); } while (0)


//NOTE(davidgow): LD3 doesn't use anything but 1.0 here
#define FMUSIC_SetMasterSpeed(sng, speed) do {} while(0)


#define FSOUND_FREE -1
#define FSOUND_NORMAL 0
#define FSOUND_LOOP_OFF 1
#define FSOUND_Sample_Load(a, filename, b, c, d) Mix_LoadWAV(filename)
#define FSOUND_Sample_Free(s) Mix_FreeChunk(s)
#define FSOUND_PlaySound(a, b) Mix_PlayChannel((a), (b), 0)
// NOTE(davidgow): LD3 hardcodes a channel here, so I don't know how this'll ever work?
#define FSOUND_StopSound(ch) Mix_HaltChannel((ch))
#define FSOUND_SetSFXMasterVolume(vol) Mix_Volume(-1, vol / 2)

#endif
