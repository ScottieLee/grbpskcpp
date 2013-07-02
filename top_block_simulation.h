#ifndef TOP_BLOCK_H
#define TOP_BLOCK_H

#include <gr_top_block.h>
#include <gr_vector_source_b.h>
#include <gr_vector_sink_b.h>

#include <tx_path_hierblock.h>
#include <rx_path_hierblock.h>
#include <gr_throttle.h>
#include <vector>
#include <string>
#include "rx_callback.h" //For callback function
#include "short_codec.h"

class top_block_simulation;
typedef boost::shared_ptr<top_block_simulation> top_block_simulation_sptr;// define a sptr type

top_block_simulation_sptr make_top_block_simulation_sptr(); // declare a function;


class top_block_simulation: public gr_top_block {

private:
    top_block_simulation();  // private default constructor
    friend top_block_simulation_sptr make_top_block_simulation_sptr();

    // transmit path
    tx_path_hierblock_sptr                   d_tx_path;
    // receive path
    rx_path_hierblock_sptr                   d_rx_path;
    gr_throttle_sptr                         d_throttle; // For simulation
    short_codec_sptr                         d_short_encoder;
    short_codec_sptr                         d_short_decoder;
    // data members

public:

    bool send_pkt(std::string mpdu, bool eof);
};


#endif // TOP_BLOCK_H
