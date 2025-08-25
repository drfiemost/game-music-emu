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

#include "Z80_Cpu.h"

#include "blargg_source.h"

#include <string.h>

Z80_Cpu::Z80_Cpu()
{
	state = &state_;

	for ( int i = 0x100; --i >= 0; )
	{
		int even = 1;
		for ( int p = i; p; p >>= 1 )
			even ^= p;
		int n = (i & (S80 | F20 | F08)) | ((even & 1) * P04);
		szpc [i] = n;
		szpc [i + 0x100] = n | C01;
	}
	szpc [0x000] |= Z40;
	szpc [0x100] |= Z40;
}

void Z80_Cpu::reset( )
{
	check( state == &state_ );
	state = &state_;
	state_.time = 0;
	state_.base = 0;

	end_time_   = 0;

	memset( &r, 0, sizeof r );
}
