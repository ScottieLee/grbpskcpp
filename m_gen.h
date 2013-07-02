


#ifndef M_GEN_H
#define M_GEN_H

#include <stdexcept>
#include <stdint.h>
#include <stdio.h>
#define LFSR_MASK (uint32_t) 0x5341	//for XOR operation
#define LFSR_SEED (uint32_t) 0x4A80	//init value after 8 packets
#define LFSR_LENGTH (uint32_t) 12	//shift register length


class m_gen
{
	private:
		uint32_t d_shift_register;

	public:
		m_gen()
			: d_shift_register(LFSR_SEED)
		{ ; }

		unsigned char next_bit_loop(void)
		{
            uint32_t bit = (d_shift_register & LFSR_MASK);
            int accum = 0;
            for(int v = 0; v < 32; v++){
                accum += (bit & 0x01);
                bit = bit >> 1;
            }
            bit = int(accum % 2);
            d_shift_register = ((d_shift_register>>1) | (bit<<(LFSR_LENGTH-1)));
            return bit;
/*            unsigned char bit = (d_shift_register & LFSR_MASK);*/
            //printf("%.2x\t", bit);
            //bit = int((1==bit) | (2==bit));
            ////printf("%d", bit);
            //d_shift_register = ((d_shift_register>>1) | (bit<<(LFSR_LENGTH-1)));
            /*return bit;*/

		}
		unsigned char next_byte(void)
		{
			unsigned char byte = 0;
			for (int i=0; i<8; i++)
			{
				byte = (byte << 1) | ((unsigned char)next_bit_loop());
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

#endif // M_GEN_H
