#ifndef CORRELATE_SYNC_WORD_H
#define CORRELATE_SYNC_WORD_H

#include <gr_sync_block.h>
#include <string>
#include <bitset>
#define ACCESS_CODE_LEN 128
class correlate_sync_word;
typedef boost::shared_ptr<correlate_sync_word> correlate_sync_word_sptr;

/*!
 * \param access_code is represented with 1 byte per bit, e.g., "010101010111000100"
 * \param len of access code must equals to ACCESS_CODE_LEN, if want to change ACCESS_CODE_LEN, need to recompile
 * \param threshold maximum number of bits that may be wrong
 */
correlate_sync_word_sptr
make_correlate_sync_word (const std::bitset<ACCESS_CODE_LEN> &access_code, size_t access_code_len, int threshold);

/*!
 * \brief Examine input for specified access code, one bit at a time.
 * \ingroup sync_blk
 *
 * input:  stream of bits, 1 bit per input byte (data in LSB)
 * output: stream of bits, 2 bits per output byte (data in LSB, flag in next higher bit)
 *
 * Each output byte contains two valid bits, the data bit, and the
 * flag bit.  The LSB (bit 0) is the data bit, and is the original
 * input data, delayed 1 bits.  Bit 1 is the
 * flag bit and is 1 if the corresponding data bit is the first data
 * bit following the access code. Otherwise the flag bit is 0.
 */
class correlate_sync_word : public gr_sync_block {
friend correlate_sync_word_sptr
  make_correlate_sync_word (const std::bitset<ACCESS_CODE_LEN> &access_code, size_t access_code_len, int threshold);
private:
  std::bitset<ACCESS_CODE_LEN> d_access_code;	// access code to locate start of packet
                                        //   access code is left justified in the word
  std::bitset<ACCESS_CODE_LEN> d_data_reg;	        // used to look for access_code
  std::bitset<ACCESS_CODE_LEN> d_flag_reg;	        // keep track of decisions
  std::bitset<ACCESS_CODE_LEN> d_flag_bit;	        // mask containing 1 bit which is location of new flag

                                        //   N is the number of bits in the access code)
  unsigned int	     d_threshold;	// how many bits may be wrong in sync vector
  int                d_phase_flag;

protected:
  correlate_sync_word(const std::bitset<ACCESS_CODE_LEN> &access_code, size_t access_code_len, int threshold);

public:
  ~correlate_sync_word();

  int work(int noutput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);


};
#endif // CORRELATE_SYNC_WORD_H
