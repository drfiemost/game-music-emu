// Z80 CPU emulator

// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
#ifndef AY_CPU_H
#define AY_CPU_H

#include "Z80_Cpu.h"

// must be defined by caller
void ay_cpu_out( class Ay_Cpu*, cpu_time_t, unsigned addr, int data );
int ay_cpu_in( class Ay_Cpu*, unsigned addr );

class Ay_Cpu : public Z80_Cpu {
public:
	// Clear all registers and keep pointer to 64K memory passed in
	void reset( void* mem_64k );

	// Run until specified time is reached. Returns true if suspicious/unsupported
	// instruction was encountered at any point during run.
	bool run( cpu_time_t end_time );

public:
	Ay_Cpu();
private:
	uint8_t* mem;
};

#endif
