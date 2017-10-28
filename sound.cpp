#include "hugo.h"
#include <mmreg.h>
#include <MSAcm.h>

struct SoundTable_
{
	LPDIRECTSOUNDBUFFER lpdsb[3];
	int current;
};

static UINT rIdSound[]={
	IDW_PING,
	IDW_FORCE,
	IDW_WARP,
	IDW_DEAD,
};

SoundTable_ SoundTable[4];
/********************************************************************/
/* DSoundBuffer::Load
/********************************************************************/
LPDIRECTSOUNDBUFFER LoadSound(WAVEFORMATEX * pWfx,char * lpData,int wSize)
{
DSBUFFERDESC dsbd;
LPVOID pbData,pbData2;
DWORD dwLength,dwLength2;
LPDIRECTSOUNDBUFFER lpdsb1;

    memset(&dsbd, 0, sizeof(DSBUFFERDESC));
    dsbd.dwSize       = sizeof(DSBUFFERDESC);
    dsbd.dwFlags      = 0xe0;//DSBCAPS_CTRLDEFAULT;
    dsbd.dwBufferBytes= wSize;
    dsbd.lpwfxFormat  = pWfx;
	if (FAILED(lpds->CreateSoundBuffer(&dsbd,&lpdsb1,NULL)))
	{
		return NULL;
    }
	if (FAILED(lpdsb1->Lock(0,wSize,&pbData,&dwLength,&pbData2,&dwLength2,0)))
	{
		lpdsb1->Release();
		return NULL;
	}
	memcpy(pbData,lpData,wSize);
	if (FAILED(lpdsb1->Unlock(pbData,dwLength,pbData2,dwLength2)))
	{
		return NULL;
	}
	return lpdsb1;
}


static void InitSound(UINT rID,int index)
{
	const char * SoundBuffer=(const char *)GetResourcePointer(NULL,MAKEINTRESOURCE(rID),"WAVE");
	const char * SoundData;
	DWORD SoundLen;
	DWORD dcl;
	WAVEFORMATEX * af;
	WAVEFORMATEX wfx;
	LPDIRECTSOUNDBUFFER lpdsb;
	char * lpBuff;
	HACMSTREAM has;
	ACMSTREAMHEADER ash;

	
	if (*(long*)SoundBuffer!=mmioFOURCC('R','I','F','F')) return;
	const char * p=SoundBuffer+8;
	if (*(long*)p!=mmioFOURCC('W','A','V','E')) return;
	p+=4;
	if (*(long*)p!=mmioFOURCC('f','m','t',' ')) return;
	af=(WAVEFORMATEX*)(p+8);
	//if (!adpcmIsValidFormat(&af->wfx)) return;
	while (*(long*)p!=mmioFOURCC('d','a','t','a')) p+=8+ *(long*)(p+4) ;
	SoundData=p+8;
	SoundLen=(*(long*)(p+4));

	wfx.wFormatTag=WAVE_FORMAT_PCM;         /* format type */
	wfx.nChannels=1;					    /* number of channels (i.e. mono, stereo..*/
	wfx.nSamplesPerSec=af->nSamplesPerSec;
	wfx.nAvgBytesPerSec=2*af->nSamplesPerSec;
	wfx.nBlockAlign=2;				        /* block size of data */
	wfx.wBitsPerSample=16;					/* number of bits per sample of mono data */
	wfx.cbSize=0;


	if (0==acmStreamOpen(&has,NULL,af,&wfx,NULL,NULL,0,ACM_STREAMOPENF_NONREALTIME))
	{
		acmStreamSize(has,SoundLen,&dcl,ACM_STREAMSIZEF_SOURCE);
		lpBuff=new char[dcl];
		ash.cbStruct=sizeof(ash);
		ash.fdwStatus=0;
		ash.pbSrc=(LPBYTE)SoundData;
		ash.cbSrcLength=SoundLen;
		ash.pbDst=(LPBYTE)lpBuff;
		ash.cbDstLength=dcl;
		acmStreamPrepareHeader(has,&ash,0);
		acmStreamConvert(has,&ash,ACM_STREAMCONVERTF_START|ACM_STREAMCONVERTF_END);
		acmStreamUnprepareHeader(has,&ash,0);
		acmStreamClose(has,0);
	
		lpdsb=LoadSound(&wfx,lpBuff,dcl);
		
		SoundTable[index].lpdsb[0]=lpdsb;
		lpds->DuplicateSoundBuffer(lpdsb,&SoundTable[index].lpdsb[1]);
		lpds->DuplicateSoundBuffer(lpdsb,&SoundTable[index].lpdsb[2]);
		SoundTable[index].current=0;
	}
}

void InitSounds()
{
	for(int i=0;i<4;i++)
	{
		InitSound(rIdSound[i],i);
	}
}


void PlaySound(int index,BOOL loop)
{
	LPDIRECTSOUNDBUFFER lpdsb=SoundTable[index].lpdsb[SoundTable[index].current];

	if (lpdsb) lpdsb->Play(0,0,loop?DSBPLAY_LOOPING:0);

	if (!loop) SoundTable[index].current=(SoundTable[index].current+1)%3;
}

void StopSound(int index)
{
	LPDIRECTSOUNDBUFFER lpdsb=SoundTable[index].lpdsb[SoundTable[index].current];

	if (lpdsb) lpdsb->Stop();
}

void DeleteSounds()
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<3;j++)
		{
			SoundTable[i].lpdsb[j]->Release();
		}
	}
}
