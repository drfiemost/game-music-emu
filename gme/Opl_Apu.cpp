#include "Opl_Apu.h"

#include "blargg_source.h"

extern "C" {
#include "ext/emu2413.h"
}

static unsigned char ym2413_inst[(16 + 3) * 8] =
{
#include "ext/2413tone.h"
};

static unsigned char vrc7_inst[(16 + 3) * 8] =
{
#include "ext/vrc7tone.h"
};

Opl_Apu::Opl_Apu() { opl = 0; opl_memory = 0; }

blargg_err_t Opl_Apu::init( long clock, long rate, blip_time_t period, type_t type )
{
	type_ = type;
	clock_ = clock;
	rate_ = rate;
	period_ = period;
	set_output( 0, 0 );
	volume( 1.0 );
	switch (type)
	{
	case type_opll:
	case type_msxmusic:
	case type_smsfmunit:
		CHECK_ALLOC( opl = OPLL_new( clock, rate ) );
		OPLL_SetChipMode((OPLL *) opl, 0);
		OPLL_setPatch((OPLL *) opl, ym2413_inst);
		break;

	case type_vrc7:
		CHECK_ALLOC( opl = OPLL_new( clock, rate ) );
		OPLL_SetChipMode((OPLL *) opl, 1);
		OPLL_setPatch((OPLL *) opl, vrc7_inst);
		break;
#if 0
	case type_opl:
		opl = ym3526_init( clock, rate );
		break;

	case type_msxaudio:
		//logfile = fopen("c:\\temp\\msxaudio.log", "wb");
		opl = y8950_init( clock, rate );
		opl_memory = malloc( 32768 );
		y8950_set_delta_t_memory( opl, opl_memory, 32768 );
		break;

	case type_opl2:
		opl = ym3812_init( clock, rate );
		break;
#else
    default:
        break;
#endif
	}
	reset();
	return 0;
}

Opl_Apu::~Opl_Apu()
{
	if (opl)
	{
		switch (type_)
		{
		case type_opll:
		case type_msxmusic:
		case type_smsfmunit:
		case type_vrc7:
			OPLL_delete( (OPLL*)opl );
			break;
#if 0
		case type_opl:
			ym3526_shutdown( opl );
			break;

		case type_msxaudio:
			y8950_shutdown( opl );
			free( opl_memory );
			//fclose( logfile );
			break;

		case type_opl2:
			ym3812_shutdown( opl );
			break;
#else
    default:
        break;
#endif
		}
	}
}

void Opl_Apu::reset()
{
	next_time = 0;
	last_amp = 0;

	switch (type_)
	{
	case type_opll:
	case type_msxmusic:
	case type_smsfmunit:
	case type_vrc7:
		OPLL_reset( (OPLL*)opl );
		break;
#if 0
	case type_opl:
		ym3526_reset_chip( opl );
		break;

	case type_msxaudio:
		y8950_reset_chip( opl );
		break;

	case type_opl2:
		ym3812_reset_chip( opl );
		break;
#else
    default:
        break;
#endif
	}
}

void Opl_Apu::write_data( blip_time_t time, int addr, int data )
{
	run_until( time );
	switch (type_)
	{
	case type_opll:
	case type_msxmusic:
	case type_smsfmunit:
	case type_vrc7:
		OPLL_writeIO( (OPLL *) opl, 0, addr );
		OPLL_writeIO( (OPLL *) opl, 1, data );
		break;
#if 0
	case type_opl:
		ym3526_write( opl, 0, addr );
		ym3526_write( opl, 1, data );
		break;

	case type_msxaudio:
		/*if ( addr >= 7 && addr <= 7 + 11 )
		{
			unsigned char temp [2] = { addr - 7, data };
			fwrite( &temp, 1, 2, logfile );
		}*/
		y8950_write( opl, 0, addr );
		y8950_write( opl, 1, data );
		break;

	case type_opl2:
		ym3812_write( opl, 0, addr );
		ym3812_write( opl, 1, data );
		break;
#else
    default:
        break;
#endif
	}
}

int Opl_Apu::read( blip_time_t time, int port )
{
    (void)port;
	run_until( time );
	switch (type_)
	{
	case type_opll:
	case type_msxmusic:
	case type_smsfmunit:
	case type_vrc7:
		return 0;//ym2413_read( opl, port );
#if 0
	case type_opl:
		return ym3526_read( opl, port );

	case type_msxaudio:
		{
			int ret = y8950_read( opl, port );
			/*unsigned char temp [2] = { port + 0x80, ret };
			fwrite( &temp, 1, 2, logfile );*/
			return ret;
		}

	case type_opl2:
		return ym3812_read( opl, port );
#else
    default:
        break;
#endif
	}

	return 0;
}

void Opl_Apu::end_frame( blip_time_t time )
{
	run_until( time );
	next_time -= time;

	if ( output_ )
		output_->set_modified();
}

void Opl_Apu::run_until( blip_time_t end_time )
{
	if ( end_time > next_time )
	{
		//blip_time_t time_delta = end_time - next_time;
		blip_time_t time = next_time;
		//unsigned count = time_delta / period_ + 1;
		switch (type_)
		{
		case type_opll:
		case type_msxmusic:
		case type_smsfmunit:
		case type_vrc7:
			{
				e_int32 buffer [2];
				e_int32* buffers[2] = {&buffer[0], &buffer[1]};

				if ( output_ )
				{
					// optimal case
					do
					{
						OPLL_calc_stereo( (OPLL *) opl, buffers, 1, -1 );
						int amp = buffer [0] + buffer [1];
						int delta = amp - last_amp;
						if ( delta )
						{
							last_amp = amp;
							synth.offset_inline( time, delta, output_ );
						}
						time += period_;
					}
					while ( time < end_time );
				}
				else
				{
					last_amp = 0;
					do
					{
						OPLL_advance( (OPLL *) opl );
						for ( int i = 0; i < osc_count; ++i )
						{
							if ( output_ )
							{
								OPLL_calc_stereo( (OPLL *) opl, buffers, 1, i );
								int amp = buffer [0] + buffer [1];
								int delta = amp - last_amp;
								if ( delta )
								{
									last_amp = amp;
									synth.offset( time, delta, output_ );
								}
							}
						}
						time += period_;
					}
					while ( time < end_time );
				}
				next_time = time;
			}
			break;
#if 0
		case type_opl:
		case type_msxaudio:
		case type_opl2:
			{
				OPLSAMPLE buffer[ 1024 ];

				while ( count > 0 )
				{
					unsigned todo = count;
					if ( todo > 1024 ) todo = 1024;
					switch (type_)
					{
					case type_opl:      ym3526_update_one( opl, buffer, todo ); break;
					case type_msxaudio: y8950_update_one( opl, buffer, todo ); break;
					case type_opl2:     ym3812_update_one( opl, buffer, todo ); break;
					default: break;
					}

					if ( output_ )
					{
						int last_amp = this->last_amp;
						for ( unsigned i = 0; i < todo; i++ )
						{
							int amp = buffer [i];
							int delta = amp - last_amp;
							if ( delta )
							{
								last_amp = amp;
								synth.offset_inline( time, delta, output_ );
							}
							time += period_;
						}
						this->last_amp = last_amp;
					}
					else time += period_ * todo;

					count -= todo;
				}
			}
			break;
#else
    default:
        break;
#endif
		}
		next_time = time;
	}
}