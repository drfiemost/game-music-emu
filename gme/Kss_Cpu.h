// Z80 CPU emulator

// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
#ifndef KSS_CPU_H
#define KSS_CPU_H

#include "Z80_Cpu.h"

// must be defined by caller
void kss_cpu_out( class Kss_Cpu*, cpu_time_t, unsigned addr, int data );
int  kss_cpu_in( class Kss_Cpu*, cpu_time_t, unsigned addr );
void kss_cpu_write( class Kss_Cpu*, unsigned addr, int data );

class Kss_Cpu : public Z80_Cpu {
public:
	// Clear registers and map all pages to unmapped
	void reset( void* unmapped_write, void const* unmapped_read );

	// Map memory. Start and size must be multiple of page_size.
	static const unsigned int page_size = 0x2000;
	void map_mem( unsigned addr, uint32_t size, void* write, void const* read );

	// Map address to page
	uint8_t* write( unsigned addr );
	uint8_t const* read( unsigned addr );

	// Run until specified time is reached. Returns true if suspicious/unsupported
	// instruction was encountered at any point during run.
	bool run( cpu_time_t end_time );

	static const unsigned int idle_addr = 0xFFFF;

public:
	Kss_Cpu();
	static const unsigned int page_shift = 13;
	static const int page_count = 0x10000 >> page_shift;
private:
	struct mem_state_t {
		uint8_t const* read  [page_count + 1];
		uint8_t      * write [page_count + 1];
	};
	mem_state_t* mem_state; // points to mem_state_ or a local copy within run()
	mem_state_t mem_state_;
	void set_page( int i, void* write, void const* read );
};

#if BLARGG_NONPORTABLE
	#define KSS_CPU_PAGE_OFFSET( addr ) (addr)
#else
	#define KSS_CPU_PAGE_OFFSET( addr ) ((addr) & (page_size - 1))
#endif

inline uint8_t* Kss_Cpu::write( unsigned addr )
{
	return mem_state->write [addr >> page_shift] + KSS_CPU_PAGE_OFFSET( addr );
}

inline uint8_t const* Kss_Cpu::read( unsigned addr )
{
	return mem_state->read [addr >> page_shift] + KSS_CPU_PAGE_OFFSET( addr );
}

#endif
