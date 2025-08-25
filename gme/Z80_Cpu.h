// Z80 CPU emulator

/* Copyright (C) 2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

// Game_Music_Emu https://github.com/libgme/game-music-emu/
#ifndef Z80_CPU_H
#define Z80_CPU_H

#include "blargg_endian.h"

typedef int32_t cpu_time_t;

class Z80_Cpu {
public:
	// Time of beginning of next instruction
	cpu_time_t time() const             { return state->time + state->base; }

	// Alter current time. Not supported during run() call.
	void set_time( cpu_time_t t )       { state->time = t - state->base; }
	void adjust_time( int delta )       { state->time += delta; }

	#if BLARGG_BIG_ENDIAN
		struct regs_t { uint8_t b, c, d, e, h, l, flags, a; };
	#else
		struct regs_t { uint8_t c, b, e, d, l, h, a, flags; };
	#endif
	static_assert( sizeof (regs_t) == 8, "Invalid register size, padding issue?" );

	struct pairs_t { uint16_t bc, de, hl, fa; };

	// Registers are not updated until run() returns
	struct registers_t {
		uint16_t pc;
		uint16_t sp;
		uint16_t ix;
		uint16_t iy;
		union {
			regs_t b; //  b.b, b.c, b.d, b.e, b.h, b.l, b.flags, b.a
			pairs_t w; // w.bc, w.de, w.hl. w.fa
		};
		union {
			regs_t b;
			pairs_t w;
		} alt;
		uint8_t iff1;
		uint8_t iff2;
		uint8_t r;
		uint8_t i;
		uint8_t im;
	};
	//registers_t r; (below for efficiency)

	// can read this far past end of memory
	enum { cpu_padding = 0x100 };

protected:
	// flags, named with hex value for clarity
	enum {
		S80 = 0x80,
		Z40 = 0x40,
		F20 = 0x20,
		H10 = 0x10,
		F08 = 0x08,
		V04 = 0x04,
		P04 = 0x04,
		N02 = 0x02,
		C01 = 0x01
	};

	Z80_Cpu();

	// Clear registers and map all pages to unmapped
	void reset();

protected:
	uint8_t szpc [0x200];
	struct state_t {
		cpu_time_t base;
		cpu_time_t time;
	};
	state_t* state; // points to state_ or a local copy within run()
	state_t state_;
	cpu_time_t end_time_;

	void set_end_time( cpu_time_t t );
public:
	registers_t r;
};

inline void Z80_Cpu::set_end_time( cpu_time_t t )
{
	cpu_time_t delta = state->base - t;
	state->base = t;
	state->time += delta;
}

#endif
