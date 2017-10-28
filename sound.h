
void InitSounds();
void PlaySound(int index,BOOL loop);
void StopSound(int index);
void DeleteSounds();

enum
{
	SND_PING,
	SND_FORCE,
	SND_WARP,
	SND_DEAD,
};
