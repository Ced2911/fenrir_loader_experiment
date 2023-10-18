// MODplay for the PS1
// Music Module Player
// Supports ProTracker (.mod) and Composer 669 (.669) music module formats

// Requires libADPCM!
// 669 timing is not very good and a bit wrong...

// If NO_PSX_LIB is defined, no parts using PSXSDK functions are compiled
// This is useful if you want to use the library to handle module files in tools

#ifndef NO_PSX_LIB
#include <psx.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef NO_PSX_LIB
#include <adpcm.h>
#endif

#ifdef YAUL
#include <yaul.h>
#include "../scsp.h"
#endif

#include "modplay.h"

#define ALIGN_32(addr) ((addr + 31) & -32)

// Configuration defines

// Size of ADPCM buffer used by MODUploadSamples when
// converting 8-bit unsigned PCM samples to PS1 ADPCM format
// By default it is set to 0x4000, 16 kilobytes

#define ADPCM_BUFFER_SIZE 0x4000

// Almost all data from the MOD is copied into another location
// in memory for the ModMusic structure when you use MODLoad()
// This means your free memory should be roughly the double of
// the size of the MOD you're loading.

// WRITE MODUNLOAD!!

int modplay_base_voice = 0;
int modplay_max_vol = 0x3fff;
int modplay_chan_vols[8];
int modplay_int_cnt = 0;
int modplay_samples_off[32];
int modplay_samples_sz[32];
int modplay_chan_mask = 0;
unsigned char modplay_adpcm_buffer[ADPCM_BUFFER_SIZE];

ModMusic *MODLoad_MOD(void *d);
void MODPlay_MOD(ModMusic *m, int *t);

ModMusic *MODLoad_669(void *d);
void MODPlay_669(ModMusic *m, int *t);

ModMusic *MODLoad(void *d)
{
	// Check if the module file we're loading is in Composer 669 format
	unsigned char *c = (unsigned char *)d;

	if (c[0] == 'i' && c[1] == 'f')
	{
		printf("Loading 669...\n");
		return MODLoad_669(d);
	}

	// If the module file was in no other format, assume the module file is
	// in ProTracker format. There's no real way to detect a ProTracker module
	// file 100% correctly so this will do.

	return MODLoad_MOD(d);
}

void MODUnload(ModMusic *m)
{
	int x;

	free(m->pattern_data);

	for (x = 0; x < m->sample_num; x++)
	{
		if (m->sample[x].length)
			free(m->sample[x].data);
	}

	free(m);
}

#ifdef YAUL
static const int logtbl[] = {
	/* 0 */ 0,
	/* 1 */ 1,
	/* 2 */ 2, 2,
	/* 4 */ 3, 3, 3, 3,
	/* 8 */ 4, 4, 4, 4, 4, 4, 4, 4,
	/* 16 */ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	/* 32 */ 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	/* 64 */ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	/* 128 */ 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

#define PCM_SCSP_FREQUENCY (44100)

#define PCM_CALC_OCT(smpling_rate) \
	((int)logtbl[PCM_SCSP_FREQUENCY / ((smpling_rate) + 1)])

#define PCM_CALC_SHIFT_FREQ(oct) \
	(PCM_SCSP_FREQUENCY >> (oct))

#define PCM_CALC_FNS(smpling_rate, shift_freq) \
	((((smpling_rate) - (shift_freq)) << 10) / (shift_freq))

#include "scsp_perdiod_tbl.h"

scsp_slot_regs_t mod_slots[32];

void MODPlay_func(ModMusic *m, int c, int s, int period, int vl, int vr)
{
	// return;
	int v = c + modplay_base_voice;

	emu_printf("c: %d / s: %d / p: %d\n", c, s, period);
	// return;
	//	static int mask = 0;

	int octr = 0;
	int shiftr = 0;
	int fnsr = 0;

// int sampleRate = (7093790 / 848);
#if 1
	int note_frq = 7159090 / 64 / (period);
	int sampleRate = 7159090 / (period * 2);
	octr = PCM_CALC_OCT(sampleRate);
	shiftr = PCM_CALC_SHIFT_FREQ(octr);
	fnsr = PCM_CALC_FNS(sampleRate, shiftr);
#elif 0
	int z = 7159090 / (period * 2);
	octr = -1;
	while (z < 1300)
	{
		octr++;
		z = z << 1;
	}
	z = 57272720 / z;

	while (z < 22050)
	{
		octr--;
		z = z << 1;
	}
	z -= (22050 * 190) >> 12;

	fnsr = z;
#elif 1
	int fnd = 0;
	for (int i = 0; i < 60; i++)
	{
		if (per_scsp_fns_oct[i].per == period)
		{
			octr = per_scsp_fns_oct[i].oct;
			fnsr = per_scsp_fns_oct[i].fns;

			fnd = 1;
			break;
		}
	}

	if (fnd == 0)
	{
		emu_printf("not found period: %d\n", period);
	}
#endif

	// SsVoiceVol(v, vl, vr);
	uint8_t pr = ((vr >> 10));
	uint8_t pl = ((vl >> 10));
	uint8_t pan = pr | (pl << 4);

	mod_slots[v].sbctl = (0 << 1) | (0 << 1);
	mod_slots[v].pcm8b = 1;
	mod_slots[v].lpctl = 0;

	mod_slots[v].sdir = 0;

	mod_slots[v].decay_l = 0x1f;
	mod_slots[v].attack_rate = 31;
	mod_slots[v].release_r = 31;

	mod_slots[v].fns = fnsr;
	mod_slots[v].oct = -octr;
	mod_slots[v].disdl = 7;
	mod_slots[v].dipan = pan;

	scsp_slot_set_fns_oct(v, mod_slots[v].fns, mod_slots[v].oct);
	scsp_slot_set_dipan(v, mod_slots[v].dipan);
	scsp_slot_set_disdl(v, mod_slots[v].disdl);

	if (s != -1)
	{
		scsp_slot_key_off(v);
		emu_printf("keyoff: %d\n", v);

		mod_slots[v].sa = modplay_samples_off[s] & 0x001fffff;
		mod_slots[v].lea = (modplay_samples_sz[s]) - 1;

		// scsp_slot_exec(&mod_slots[v], v);
		modplay_chan_mask |= (1 << v);
	}
}
#elif defined(NO_PSX_LIB)
void MODPlay_func(ModMusic *m, int c, int s, int p, int vl, int vr)
{
	// Just a stub
}
#else
void MODPlay_func(ModMusic *m, int c, int s, int p, int vl, int vr)
{
	int v = c + modplay_base_voice;
	//	static int mask = 0;

	if (s != -1)
	{
		//	SsKeyOff(v);
		SsVoicePitch(v, p);
	}

	if (modplay_max_vol != 0x3fff)
	{
		vl = (modplay_max_vol * vl) / 0x4000;
		vr = (modplay_max_vol * vr) / 0x4000;
		vl &= 0x3fff;
		vr &= 0x3fff;
	}

	SsVoiceVol(v, vl, vr);

	if (s != -1)
	{
		SsVoiceStartAddr(v, modplay_samples_off[s]);
		modplay_chan_mask |= (1 << v);

		// if(c == (m->channel_num - 1))
		//{
		// SsKeyOnMask(mask);
		// mask = 0;
		//}
		// SsKeyOn(v);
	}
}
#endif

uint32_t old_play_mask = 0;
void MODPlay(ModMusic *m, int *t)
{
	modplay_chan_mask = 0;

	switch (m->fmt)
	{
	case MOD_FMT_MOD:
		MODPlay_MOD(m, t);
		break;
	case MOD_FMT_669:
		MODPlay_669(m, t);
		break;
	}

#ifndef NO_PSX_LIB
	SsKeyOnMask(modplay_chan_mask);
#endif
#ifdef YAUL
	for (int i = 0; i < m->channel_num; i++)
	{

		// emu_printf("%s: %d\n", (modplay_chan_mask & (1 << i)) ? "on" : "off", i);
		// if ((modplay_chan_mask & (1 << i)) & (!(old_play_mask < (1 << i))))
		if (modplay_chan_mask & (1 << i))
		{
			emu_printf("key on %d|modplay_chan_mask:%8x\n", i, modplay_chan_mask);
			scsp_slot_exec(&mod_slots[i], i);
		}
	}

	old_play_mask = modplay_chan_mask;
#endif
}

void MODStop(ModMusic *m)
{
#ifdef YAUL
	int x;

	for (x = 0; x < m->channel_num; x++)
	{
		get_scsp_slot(x)->kyonb = 0;
		get_scsp_slot(x)->kyonex = 0;
	}

#elif !defined(NO_PSX_LIB)
	int mask = 0;
	int x;

	for (x = 0; x < m->channel_num; x++)
		mask |= 1 << (modplay_base_voice + x);

	SsKeyOffMask(mask);
#endif
}

#ifdef YAUL
int MODUploadSamples(ModMusic *m, int base_addr)
{
	int x, b;

	if (base_addr == -1)
		base_addr = SCSP_RAM;

	uint32_t addr = base_addr;
	uint16_t sz = 0;
	modplay_samples_off[0] = base_addr;

	for (x = 0; x < m->sample_num; x++)
	{
		modplay_samples_off[x] = addr;

		sz = m->sample[x].length;
		memcpy(modplay_samples_off[x], m->sample[x].data, sz);

		modplay_samples_sz[x] = ALIGN_32(sz);
		emu_printf("%d %d\n", x, sz);

		addr += sz;
		emu_printf("%d %d %08x/%08x\n", x, sz, addr, ALIGN_32(addr));
		// align
		addr = ALIGN_32(addr);
	}

	if (0)
	{

		int n = 4;
		int v = 0;

		int octr;
		int shiftr;
		int fnsr;
		int period = 360;

		int note_frq = 7159090 / 64 / (period);
		int sampleRate = 8100; // 7159090 / (period * 2);
		octr = PCM_CALC_OCT(sampleRate);
		shiftr = PCM_CALC_SHIFT_FREQ(octr);
		fnsr = PCM_CALC_FNS(sampleRate, shiftr);

		//
		uint8_t pr = ((0 >> 10));
		uint8_t pl = ((0 >> 10));
		uint8_t pan = pr | (pl << 4);
		scsp_slot_regs_t st = {};
		st.sa = modplay_samples_off[n] & 0x001fffff;
		st.lea = 0xfffff;
		st.pcm8b = 1;
		st.lpctl = 0;

		st.sbctl = (0 << 1) | (0 << 1);
		st.disdl = 7;
		// st.dipan = pan;
		st.sdir = 1;
		st.decay_l = 0x1f;

		st.attack_rate = 31;
		st.release_r = 31;

		// 44100/5
		st.fns = 1200 - 283;
		st.oct = -4;

		// hz = (psx_pitch*44100)>>12
		// psx_pitch = (hz << 12) / 44100

		emu_printf("sz:%d\n", modplay_samples_sz[n]);

		scsp_slot_exec(&st, v);
	}

	return modplay_samples_off[x];
}

#elif !defined(NO_PSX_LIB)
int MODUploadSamples(ModMusic *m, int base_addr)
{
	int x, b;

	if (base_addr == -1)
		base_addr = SPU_DATA_BASE_ADDR;

	modplay_samples_off[0] = base_addr;

	for (x = 0; x < m->sample_num; x++)
	{
		b = SsAdpcmPack(m->sample[x].data, modplay_adpcm_buffer,
						m->sample[x].length, FMT_U8, sizeof(modplay_adpcm_buffer), 0);
		SsUpload(modplay_adpcm_buffer, b, modplay_samples_off[x]);

		if (x != 30)
			modplay_samples_off[x + 1] = modplay_samples_off[x] + b;
	}

	return modplay_samples_off[x];
}

int MOD4PSX_Upload(void *d, int base_addr)
{
	unsigned char *c = d;
	int x;
	int o;
	int ro;
	int sz;
	int n;

	// Check magic string

	if (strncmp(c, "_mod4psx", 8) != 0)
		return -1;

	o = 12;
	n = *((int *)(c + 8));

	if (base_addr == -1)
		modplay_samples_off[0] = SPU_DATA_BASE_ADDR;
	else
		modplay_samples_off[0] = base_addr;

	printf("Number of samples: %d\n", n);

	for (x = 0; x < n; x++)
	{
		// Get size
		sz = *((int *)(c + o));
		printf("Size: %d\n", sz);
		// Ignore eight reserved bytes (for future expension)
		o += 12;
		SsUpload(c + o, sz, modplay_samples_off[x]);

		if (x != 30)
			modplay_samples_off[x + 1] = modplay_samples_off[x] + sz;

		o += sz;
	}

	return modplay_samples_off[x];
}

#endif

void MODSetBaseVoice(int base_voice)
{
	modplay_base_voice = base_voice;
}

void MODSetMaxVolume(unsigned short max_volume)
{
	// Default is 0x3fff.
	// Valid values 0 (MUTE) - 0x3FFF (MAX)

	modplay_max_vol = max_volume & 0x3fff;
}

#ifdef NO_PSX_LIB

// Some code might use this, so use a stub.

int SsFreqToPitch(int hz)
{
	return 0;
}

#endif
