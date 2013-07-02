/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef SOFT_FRAME_CONTOR_H
#define SOFT_FRAME_CONTOR_H

#include <gr_sync_block.h>
#include <gr_msg_queue.h>
#include <vector>
#include <algorithm>
#include <bitset>
#include "short_codec.h"

/* brief decode PHY header and get data according to it. PHY header decoder sptr is a parameter passed in.
 * Notice that header bit length is fixed to 120 bit.
 *
*/
class soft_frame_contor;

/*
 * We use boost::shared_ptr's instead of raw pointers for all access
 * to gr_blocks (and many other data structures).  The shared_ptr gets
 * us transparent reference counting, which greatly simplifies storage
 * management issues.  This is especially helpful in our hybrid
 * C++ / Python system.
 *
 * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
 *
 * As a convention, the _sptr suffix indicates a boost::shared_ptr
 */
typedef boost::shared_ptr<soft_frame_contor> soft_frame_contor_sptr;

/*!
 * \brief Return a shared_ptr to a new instance of llnfs_lln_framer_sink.
 *
 * To avoid accidental use of raw pointers, llnfs_lln_framer_sink's
 * constructor is private.  llnfs_make_lln_framer_sink is the public
 * interface for creating new instances.
 */
soft_frame_contor_sptr make_soft_frame_contor (gr_msg_queue_sptr target_queue, short_codec_sptr header_decoder);

/*!
 * \brief
 * \ingroup block
 *
 * \ for a version that subclasses gr_sync_block.
 *
 */

#define BITS_PER_BYTE 8
class soft_frame_contor : public gr_sync_block
{
  friend soft_frame_contor_sptr 
  make_soft_frame_contor (gr_msg_queue_sptr target_queue, short_codec_sptr header_decoder);

 private:
  enum state_t {STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER};

  static const int MAX_PKT_LEN    = 4096;   // this is byte len!!!
  static const int HEADERBITLEN   = 120;    // 

  gr_msg_queue_sptr  d_target_queue;        // where to send the packet when received
  state_t            d_state;
  std::bitset<HEADERBITLEN>  d_header;      // header bitset
  int		         d_headerbitlen_cnt;    // how many so far
  int                d_payload_len;         // len is BYTES!
  unsigned char      d_packet[MAX_PKT_LEN]; // assembled payload
  float              d_soft_bit_array[MAX_PKT_LEN]; // assembled soft bits
  int                d_soft_float_counter;
  unsigned char	     d_packet_byte;         // byte being assembled
  int		         d_packet_byte_index;   // which bit of d_packet_byte we're working on
  int 		         d_packetlen;           // length of packet
  int		         d_packetlen_cnt;       // how many so far

  short_codec_sptr   d_short_decoder;        // short decoder is an abstract phy layer decoder
 protected:
  soft_frame_contor(gr_msg_queue_sptr target_queue, short_codec_sptr header_decoder);
  //void decode_bch(int *recd, int *decoded_data);
  void enter_search();
  void enter_have_sync();
  void enter_have_header(int payload_len);
  
  bool decode_header()
  {
    //--------------------------perform PHY Header Decoding---------------------------------------
    // Use an abstract decoding interface
    // step1 call the clear function to clear the input buffer
    d_short_decoder -> clear();
    // step2 call the load function to load the input buffer, type is vector<bool>
    std::vector<bool> input;

    for(int v = 0; v < d_header.size(); v++) {
        input.push_back(d_header[d_header.size()-v-1]);
    }
    d_short_decoder -> load_input(input);
    // step3 polymorphically call the decode function to decode the phy header, return decoding failed or not?
    return d_short_decoder -> process();
  }

  void get_payload_len(int *len)
  {
       // decoding result
       // NOTICE:
       // e.g. if vector<bool> is 1 1 0 1 ,then payload_len is dec(1101) = 13
       //                  index  0 1 2 3
       //transmitting order      1st2nd3rd ....

       std::vector<bool> decode_result = d_short_decoder -> result();
       int tmp_payload_len = 0;
       for (int v = 0; v < decode_result.size(); v++) {
            tmp_payload_len = tmp_payload_len << 1;
            tmp_payload_len |= decode_result[v];
       }
       d_payload_len = tmp_payload_len;
       *len = d_payload_len;
  }

 public:
  ~soft_frame_contor();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif
