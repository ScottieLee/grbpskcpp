#include "bpsk_hierblock.h"
#include <gr_io_signature.h>
#include <assert.h>
#include <iostream>

bpsk_mod_sptr make_bpsk_mod_sptr(mod_params* mod_p)
{
    bpsk_mod_params* bpsk_mod_p = reinterpret_cast<bpsk_mod_params*> (mod_p);
    return gnuradio::get_initial_sptr(new bpsk_mod(bpsk_mod_p->samples_per_symbol, bpsk_mod_p->rolloff_alpha));
}

bpsk_mod::bpsk_mod(unsigned int samples_per_symbol, double rolloff_alpha):
             gr_hier_block2("bpsk_modulation",
                            gr_make_io_signature(1, 1, sizeof(unsigned char)),
                            gr_make_io_signature(1, 1, sizeof(gr_complex))
                           ),
             d_samples_per_symbol(samples_per_symbol),
             d_rolloff_alpha(rolloff_alpha)
{
    //setup for BPSK mod
    d_packed_to_unpacked = gr_make_packed_to_unpacked_bb(1, GR_MSB_FIRST);
    gr_complex bit0(1,0);
    gr_complex bit1(-1,0);
    std::vector<gr_complex> symbol_table;
    symbol_table.push_back(bit0);
    symbol_table.push_back(bit1); // Neg Mapping: 0 -> 1, 1 -> -1
    d_chunks_to_symbols = gr_make_chunks_to_symbols_bc(symbol_table);
    d_rrc_tx_taps = gr_firdes::root_raised_cosine(static_cast<double>(d_samples_per_symbol),//GAIN
                                                  static_cast<double>(d_samples_per_symbol),//SAMPLING RATE
                                                  1.0,//Symbol rate
                                                  d_rolloff_alpha, // alpha
                                                  25.0 * d_samples_per_symbol  // number of taps
                                                  );
    d_rrc_tx = gr_make_interp_fir_filter_ccf(static_cast<unsigned int>(d_samples_per_symbol), d_rrc_tx_taps);

    //Connect them up
    this -> connect (this->basic_block(), 0, d_packed_to_unpacked, 0);
    this -> connect (d_packed_to_unpacked, 0, d_chunks_to_symbols, 0);
    this -> connect (d_chunks_to_symbols,  0, d_rrc_tx, 0);
    this -> connect (d_rrc_tx,  0, this->basic_block(), 0);
}





bpsk_demod_sptr make_bpsk_demod_sptr(demod_params* demod_p)
{
    bpsk_demod_params* bpsk_demod_p = reinterpret_cast<bpsk_demod_params*> (demod_p);
    return gnuradio::get_initial_sptr(new bpsk_demod(bpsk_demod_p->samples_per_symbol,
                                                     bpsk_demod_p->rolloff_alpha,
                                                     bpsk_demod_p->costas_alpha,
                                                     bpsk_demod_p->mm_gain_mu,
                                                     bpsk_demod_p->mm_omega_relative_limit));
}

bpsk_demod::bpsk_demod( unsigned int samples_per_symbol,
                             double       rolloff_alpha,
                             float        costas_alpha,
                             float        mm_gain_mu,
                             float        mm_omega_relative_limit):
             gr_hier_block2("bpsk_demodulation",
                            gr_make_io_signature(1, 1, sizeof(gr_complex)),
                            gr_make_io_signature2(2, 2, sizeof(unsigned char), sizeof(float))
                           ),
             d_samples_per_symbol(samples_per_symbol),
             d_rolloff_alpha(rolloff_alpha),
             d_costas_alpha(costas_alpha),
             d_costas_beta(0.25 * d_costas_alpha * d_costas_alpha),
             d_mm_gain_mu(mm_gain_mu),
             d_mm_omega(static_cast<float> (d_samples_per_symbol)),
             d_mm_gain_omega(0.25 * d_mm_gain_mu * d_mm_gain_mu),
             d_mm_omega_relative_limit(mm_omega_relative_limit)
{
    //Setup for DBPSK demod
    d_agc = gr_make_agc2_cc(0.6e-2, 1e-3, 1.22, 1, 100);
    d_rrc_rx_taps = gr_firdes::root_raised_cosine(1.0,//GAIN
                                                  static_cast<double>(d_samples_per_symbol),//SAMPLING RATE
                                                  1.0,//Symbol rate
                                                  d_rolloff_alpha, // alpha
                                                  11.0 * d_samples_per_symbol// number of taps
                                                 );
    d_rrc_rx = gr_make_interp_fir_filter_ccf(1, d_rrc_rx_taps);

    float mm_mu = 0.5;
    float fmin = -0.25;
    float fmax = 0.25;

    d_mpsk_receiver = gr_make_mpsk_receiver_cc(2,
                                               0,
                                               d_costas_alpha,
                                               d_costas_beta,
                                               fmin, fmax,
                                               mm_mu,
                                               d_mm_gain_mu,
                                               d_mm_omega,
                                               d_mm_gain_omega,
                                               d_mm_omega_relative_limit);

    d_soft_slicer = make_soft_slicer(NEG_MAPPING);

    //Connect'em up
    this -> connect (this->basic_block(), 0, d_agc, 0);
    this -> connect (d_agc, 0, d_rrc_rx, 0);
    this -> connect (d_rrc_rx,  0, d_mpsk_receiver, 0);
    this -> connect (d_mpsk_receiver,  0, d_soft_slicer, 0);
    this -> connect (d_soft_slicer, 0, this->basic_block(), 0);
    this -> connect (d_soft_slicer, 1, this->basic_block(), 1);
}


