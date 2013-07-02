#include "top_block_simulation.h"
#include <gr_io_signature.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include "string_functions.h"
#include "bpsk_hierblock.h" // For bpsk_mod_params
//#define ACCESS_CODE_LEN 128

top_block_simulation_sptr make_top_block_simulation_sptr()
{
    return gnuradio::get_initial_sptr(new top_block_simulation());
}

top_block_simulation::top_block_simulation():gr_top_block("turbo_bpsk_phy_top_block")
{
    //----------------------------------Transmit Path------------------------------------
    d_short_encoder = boost::make_shared<repetition_encoder>(5);
    bpsk_mod_params bpsk_mod_parameters = { 4, 0.35};  // FIXME!

    d_tx_path = make_tx_path_hierblock_sptr (reinterpret_cast<mod_params*> (&bpsk_mod_parameters),
                                             BPSK_MOD,
                                             1,
                                             4); //FIXME!

    //----------------------------------Receive Path------------------------------------
    d_short_decoder = boost::make_shared<repetition_decoder>(5);
    bpsk_demod_params bpsk_demod_parameters = { 4, 0.35, 0.02, 0.1, 0.005};  // FIXME!
    d_rx_path = make_rx_path_hierblock_sptr (reinterpret_cast<demod_params*> (&bpsk_demod_parameters),
                                             BPSK_DEMOD,
                                             callback1,
                                             4,
                                             10,
                                             "00000011010001110111011011000111001001110010100010010101101100001111110010111000100010010011100011011000110101110110101001001111",
                                             d_short_decoder);

    this->connect (d_tx_path, 0, d_rx_path, 0);
//    this->connect (d_throttle, 0, d_rx_path, 0);

}

bool top_block_simulation::send_pkt(std::string mpdu, bool eof)
{
    return d_tx_path ->send_pkt(mpdu, eof);
}
