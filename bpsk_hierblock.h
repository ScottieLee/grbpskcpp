#ifndef BPSK_HIERBLOCK_H
#define BPSK_HIERBLOCK_H

#include <gr_hier_block2.h> //This is a hierachical block
#include <gr_multiply_const_cc.h>
#include <gr_packed_to_unpacked_bb.h>
#include <gr_diff_encoder_bb.h>
#include <gr_chunks_to_symbols_bc.h>
#include <gr_interp_fir_filter_ccf.h>
#include <gr_firdes.h>
#include <gr_fir_filter_ccf.h>
#include <gr_agc2_cc.h>
#include <gr_mpsk_receiver_cc.h>
#include "soft_slicer.h"
#include <cstdio> // for display

// Struct of Bpsk modulation parameters
struct bpsk_mod_params {
    unsigned int samples_per_symbol;
    double       rolloff_alpha;
};

// Struct of Bpsk demodulation parameters
struct bpsk_demod_params {
    unsigned int samples_per_symbol;
    double       rolloff_alpha;
    float        costas_alpha;
    float        mm_gain_mu;
    float        mm_omega_relative_limit;
};

class bpsk_mod;
typedef boost::shared_ptr<bpsk_mod> bpsk_mod_sptr; // define a sptr for handling


class bpsk_demod;
typedef boost::shared_ptr<bpsk_demod> bpsk_demod_sptr; // define a sptr for handling

class mod_params;
bpsk_mod_sptr make_bpsk_mod_sptr (mod_params*);

class bpsk_mod: public gr_hier_block2 {
private:
    bpsk_mod(unsigned int samples_per_symbol, double rolloff_alpha); // Private Contor.
    friend bpsk_mod_sptr make_bpsk_mod_sptr (mod_params*); // Sptr handles construction and controlling of the block instance.

    // declaration of block sptrs:
    gr_packed_to_unpacked_bb_sptr            d_packed_to_unpacked;
    gr_chunks_to_symbols_bc_sptr             d_chunks_to_symbols;
    std::vector<float>                       d_rrc_tx_taps;
    gr_interp_fir_filter_ccf_sptr            d_rrc_tx;

    // data members
    unsigned int d_samples_per_symbol;
    double       d_rolloff_alpha;

public:
    // accessors:
    unsigned int get_samples_per_symbol () { return d_samples_per_symbol; }
    double       get_rolloff_alpha()       { return d_rolloff_alpha; }
    void         display();
};

inline void bpsk_mod::display()
{
    fprintf(stderr, "Modulation:     BPSK");
    fprintf(stderr, "Rolloff Factor  %.2f\n", d_rolloff_alpha);
    fprintf(stderr, "Samples per Sym %2d\n",  d_samples_per_symbol);
}


class demod_params;
bpsk_demod_sptr make_bpsk_demod_sptr (demod_params*); // make_demod_sptr IS AN OVERLOAD FUNCTION!

class bpsk_demod: public gr_hier_block2 {
private:
    bpsk_demod(unsigned int samples_per_symbol,
               double       rolloff_alpha,
               float        costas_alpha,
               float        mm_gain_mu,
               float        mm_omega_relative_limit); //Private Contor.

    friend bpsk_demod_sptr make_bpsk_demod_sptr (demod_params*); // Sptr for handling
    // declaration of block sptrs:
    gr_agc2_cc_sptr                          d_agc;
    std::vector<float>                       d_rrc_rx_taps;
    gr_interp_fir_filter_ccf_sptr            d_rrc_rx;
    gr_mpsk_receiver_cc_sptr                 d_mpsk_receiver;
    soft_slicer_sptr                         d_soft_slicer;

    // data members:
    unsigned int d_samples_per_symbol;
    double       d_rolloff_alpha;
    float        d_costas_alpha;
    float        d_costas_beta;
    float        d_mm_gain_mu;
    float        d_mm_omega;
    float        d_mm_gain_omega;
    float        d_mm_omega_relative_limit;

public:
    // accessors:
    unsigned int get_samples_per_symbol () { return d_samples_per_symbol; }
    double       get_rolloff_alpha()       { return d_rolloff_alpha; }
    void         display();

};

inline void bpsk_demod::display()
{
    fprintf(stderr, "Demodulation:   BPSK");
    fprintf(stderr, "Rolloff Factor  %.2f\n", d_rolloff_alpha);
    fprintf(stderr, "Samples per Sym %2d\n",  d_samples_per_symbol);
    fprintf(stderr, "Costas alpha    %8f\n",  d_costas_alpha);
    fprintf(stderr, "Costas beta     %8f\n",  d_costas_beta);
    fprintf(stderr, "MM gain mu      %8f\n",  d_mm_gain_mu);
    fprintf(stderr, "MM gain omega relative limit %8f", d_mm_omega_relative_limit);
}

#endif // BPSK_HIERBLOCK_H
