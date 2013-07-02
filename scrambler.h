#ifndef SCRAMBLER_H
#define SCRAMBLER_H

#include <stdexcept>
#include <stdint.h>

#define LFSR_MASK (uint32_t) 0x0003	//for XOR operation
#define LFSR_SEED (uint32_t) 0x4A80	//init value after 8 packets
#define LFSR_LENGTH (uint32_t) 15	//shift register length


class scrambler
{
	private:
		uint32_t d_shift_register;

	public:
		scrambler()
			: d_shift_register(LFSR_SEED)
		{ ; }

		unsigned char next_bit_loop(void)
		{
			unsigned char bit = (d_shift_register & LFSR_MASK);
			bit = int((1==bit) | (2==bit));
			d_shift_register = ((d_shift_register>>1) | (bit<<(LFSR_LENGTH-1)));
			return bit;
		}
		unsigned char next_byte(void)
		{
			unsigned char byte = 0;
			for (int i=0; i<8; i++)
			{
				byte = (byte << 1) | next_bit_loop();
			}
			return byte;
		}
		
		/*!
		* Reset shift register to initial seed value
		*/
		void reset() { d_shift_register = LFSR_SEED; }
		
		uint32_t shift_register(void) {
			return d_shift_register;
		}
		
		uint32_t shift_register(uint32_t seed)
		{
			d_shift_register = seed;
			return d_shift_register;
		}
		
};

#endif // SCRAMBLER_H
