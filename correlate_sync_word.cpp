
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "correlate_sync_word.h"
#include <gr_io_signature.h>
#include <stdexcept>
#include <gr_count_bits.h>
#include <cstdio>
#include <iostream>
#include "string_functions.h"
#define VERBOSE 0


correlate_sync_word_sptr
make_correlate_sync_word (const std::bitset<ACCESS_CODE_LEN> &access_code, size_t access_code_len, int threshold)
{
  return correlate_sync_word_sptr (new correlate_sync_word (access_code, access_code_len, threshold));
}


correlate_sync_word::correlate_sync_word (
  const std::bitset<ACCESS_CODE_LEN>& access_code, size_t access_code_len, int threshold)
  : gr_sync_block ("correlate_sync_word",
                   gr_make_io_signature2 (2, 2, sizeof(unsigned char), sizeof(float)),
                   gr_make_io_signature2 (2, 2, sizeof(unsigned char), sizeof(float))),
    d_threshold(threshold),
    d_phase_flag(1)

{
    d_data_reg.reset();
    d_flag_reg.reset();
    d_flag_bit.set(0);  // new data always goes in 0x0000000000000001
    if (access_code_len != ACCESS_CODE_LEN) {
        fprintf(stderr, "correlate_access_code: access_code should be %d bits\n", ACCESS_CODE_LEN);
        throw std::out_of_range (" Correlate_sync_word:: access_code should be 128 bits");
    }
    else {
        d_access_code = access_code;
    }
}

correlate_sync_word::~correlate_sync_word ()
{
}


int
correlate_sync_word::work (int noutput_items,
                                   gr_vector_const_void_star &input_items,
                                   gr_vector_void_star &output_items)
{
  const unsigned char *in = (const unsigned char *) input_items[0];
  const float         *in_soft = (const float *) input_items[1];
  unsigned char *out = (unsigned char *) output_items[0];
  float *out_soft = (float *) output_items[1];
  std::bitset<ACCESS_CODE_LEN> bs1(0);
  for (int i = 0; i < noutput_items; i++){

    // compute output value
    unsigned char t = 0;
    t |= d_data_reg[0]; // Get lsb of data reg
    //printf("d_flag_reg = %lu\n", d_flag_reg.to_ulong());
    t |= (d_flag_reg[0]) << 1;	// Get flag bit then least two bits are  |flag|databit|
    out[i] = t; // Out is t
    out_soft[i] = d_phase_flag ? in_soft[i - 1] : (-in_soft[i - 1]); //handling soft output phase ambiguity
    // compute hamming distance between desired access code and current data
    std::bitset<ACCESS_CODE_LEN> wrong_bits(0);
    unsigned int nwrong = d_threshold + 1;
    int new_flag = 0;
    int phase_flag = 0;
    wrong_bits  = (d_data_reg ^ d_access_code);
    //std::cout << "bitset - to_string()" << wrong_bits.to_string() << std::endl;
    nwrong = wrong_bits.count();
    //printf("nwrong=%d",nwrong);
    // test for access code with up to threshold errors
    new_flag = (nwrong <= d_threshold);
    phase_flag = (nwrong >= (ACCESS_CODE_LEN - d_threshold)); //FIXME!!!!!!
    if (new_flag){
#if VERBOSE
        printf("round %d, positive sync found, nwrong = %d", i, nwrong);
#endif
        d_phase_flag = 1;   //d_phase_flag         1:detect positive syc   0:detect negative sync   d_phase_flag will hold its value if sync not detected
    }
    if (phase_flag){
        d_phase_flag = 0;
#if VERBOSE
        printf("round %d, negative sync found, nwrong = %d", i, nwrong);
#endif
    }
    bs1.reset();
    // shift in new data and new flag
    if (in[i] & 0x1){
        bs1.set(0);
    }

    d_data_reg = (d_data_reg << 1) | bs1;
    d_flag_reg = (d_flag_reg << 1);
    if (new_flag || phase_flag) {
#if VERBOSE
      printf("d_flag_bit = %lu, d_flag_reg = %lu\n", d_flag_bit.to_ulong(), d_flag_reg.to_ulong());
#endif
      d_flag_reg |= d_flag_bit;
#if VERBOSE
      printf("Flagging!, i =%d, d_flag_reg = %lu\n", i, d_flag_reg.to_ulong());
#endif
    }
  }

  return noutput_items;
}
