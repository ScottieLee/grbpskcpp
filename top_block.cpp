#include "top_block.h"

#include <gr_io_signature.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include "string_functions.h"

#define ACCESS_CODE_LEN 128

top_block_sptr make_top_block_sptr()
{
    return gnuradio::get_initial_sptr(new top_block());
}

const unsigned int top_block::SAMPLES_PER_SYMBOL = 4;
const unsigned int top_block::DAC_RATE = 128000000;
const unsigned int top_block::BITRATE = 500000;
const double       top_block::TX_FREQ = 2430000000.0;
const float        top_block::TX_AMPL = 0.015;
const unsigned int top_block::BITS_PER_SYMBOL = 1;

const unsigned int top_block::ADC_RATE = 64000000;
const double       top_block::RX_FREQ = 2430000000.0;
const float        top_block::RX_GAIN = 45.0;
const float        top_block::MAX_AMPLITUDE = 32767;


top_block::top_block():gr_top_block("turbo_bpsk_phy_top_block"
                                   ),
                       d_spec_tx(static_spec),
                       d_spec_rx(static_spec)
{
    //----------------------------------Transmit Path------------------------------------

    //setup for gr_message_source
    d_message_source = gr_make_message_source(sizeof(char), 4);
    //setup for BPSK mod
    d_packed_to_unpacked = gr_make_packed_to_unpacked_bb(BITS_PER_SYMBOL, GR_MSB_FIRST);
    gr_complex bit0(1,0);
    gr_complex bit1(-1,0);
    std::vector<gr_complex> symbol_table;
    symbol_table.push_back(bit0);
    symbol_table.push_back(bit1);
    d_chunks_to_symbols = gr_make_chunks_to_symbols_bc(symbol_table);
    d_rrc_tx_taps = gr_firdes::root_raised_cosine((double)SAMPLES_PER_SYMBOL,//GAIN
                                                  (double)SAMPLES_PER_SYMBOL,//SAMPLING RATE
                                                  1.0,//Symbol rate
                                                  0.35, // alpha
                                                  25.0 * SAMPLES_PER_SYMBOL  // number of taps
                                                  );
    d_rrc_tx = gr_make_interp_fir_filter_ccf((unsigned int )SAMPLES_PER_SYMBOL, d_rrc_tx_taps);
    // setup for d_power_control
    d_power_control = gr_make_multiply_const_cc(std::complex<float>(TX_AMPL*MAX_AMPLITUDE,TX_AMPL*MAX_AMPLITUDE));

    // setup for usrp_sink
    d_usrp_sink_sptr = usrp_make_sink_c(0, 32, 1, -1, 1024, 16, "", "");
    if(!(d_usrp_sink_sptr -> set_interp_rate ((unsigned int)(DAC_RATE/BITRATE/SAMPLES_PER_SYMBOL))))
        fprintf(stderr, "Failed to set interpolation rate.");
    d_sub_device_tx = d_usrp_sink_sptr -> selected_subdev (d_spec_tx);
    d_usrp_sink_sptr -> set_mux (d_usrp_sink_sptr -> determine_tx_mux_value (d_spec_tx));
    d_sub_device_tx ->set_auto_tr(true);
    d_sub_device_tx ->set_gain((d_sub_device_tx ->gain_max()+d_sub_device_tx ->gain_min())/2);
    usrp_tune_result tune_result_tx;
    bool ok_tx = d_usrp_sink_sptr -> tune (0, d_sub_device_tx, TX_FREQ, &tune_result_tx);
    if(!ok_tx){
       fprintf(stderr," Failed to set tx frequency." );
    }
    std::cout << "---------------------------TX Settings--------------------------\n";
    printf ("Tune: Target  frequecy = %f\n", TX_FREQ);
    printf ("      Actural frequecy = %f\n", (tune_result_tx.baseband_freq + tune_result_tx.dxc_freq));
    printf ("      Residual frequency = %f\n", tune_result_tx.residual_freq);
    printf ("      Baseband Freq = %f, DXC Freq = %f\n", tune_result_tx.baseband_freq, tune_result_tx.dxc_freq);
    printf      ("Subdevice name is %s\n", d_sub_device_tx -> name().c_str());
    printf      ("Subdevice freq range: (%g, %g)\n", d_sub_device_tx -> freq_min(), d_sub_device_tx->freq_max());
    std::cout << "Hardware Interpolation = " << (unsigned int)(DAC_RATE/BITRATE/SAMPLES_PER_SYMBOL) << std::endl;

    this->connect (d_message_source, 0, d_packed_to_unpacked, 0);
    this->connect (d_packed_to_unpacked, 0, d_chunks_to_symbols, 0);
    this->connect (d_chunks_to_symbols, 0, d_rrc_tx, 0);
    this->connect (d_rrc_tx, 0, d_power_control, 0);
    this->connect (d_power_control, 0, d_usrp_sink_sptr, 0);

    //----------------------------------Receive Path------------------------------------

    // Setup for usrp source
    d_usrp_source_sptr = usrp_make_source_c(0, 16, 1, -1, 0, 1024, 16, "", "");
    if (!(d_usrp_source_sptr -> set_decim_rate ((unsigned int)(ADC_RATE/BITRATE/SAMPLES_PER_SYMBOL))))
        fprintf(stderr, "Failed to set decimation rate.");
    d_sub_device_rx = d_usrp_source_sptr -> selected_subdev (d_spec_rx);
    d_usrp_source_sptr -> set_mux (d_usrp_source_sptr -> determine_rx_mux_value (d_spec_rx));
    d_sub_device_rx ->set_auto_tr(true);
    d_sub_device_rx -> set_gain(RX_GAIN);
    d_sub_device_rx ->select_rx_antenna(0);
    usrp_tune_result tune_result_rx;
    bool ok_rx = d_usrp_source_sptr -> tune (0, d_sub_device_rx, RX_FREQ, &tune_result_rx);
    if(!ok_rx){
       fprintf(stderr," Failed to set rx frequency." );
    }
    std::cout << "---------------------------RX Settings--------------------------\n";
    std::cout << "Tune: Target  frequecy = " << RX_FREQ << std::endl;
    std::cout << "      Actural frequecy = " << (tune_result_rx.baseband_freq + tune_result_rx.dxc_freq) << std::endl;
    std::cout << "      Residual frequency = " << tune_result_rx.residual_freq << std::endl;
    printf      ("Subdevice name is %s\n", d_sub_device_rx -> name().c_str());
    printf      ("Subdevice freq range: (%g, %g)\n", d_sub_device_rx -> freq_min(), d_sub_device_rx->freq_max());
    std::cout << "Hardware Decimation    = " << (unsigned int)(ADC_RATE/BITRATE/SAMPLES_PER_SYMBOL) << std::endl;

    // Setup for channel filter
    d_channel_filter_taps = gr_firdes::low_pass(
                                              1.0, //gain
                                              SAMPLES_PER_SYMBOL, // sampling rate
                                              1.0, // midpoint of trans. band
                                              0.5, // width of trans. band
                                              gr_firdes::WIN_BLACKMAN // window
                                              );
    d_channel_filter = gr_make_fir_filter_ccf(1, // no decimation                //FIXME!!!
                                              d_channel_filter_taps
                                             );

    //Setup for DBPSK demod
    d_agc = gr_make_agc2_cc(0.6e-2, 1e-3, 1.22, 1, 100);
    d_rrc_rx_taps = gr_firdes::root_raised_cosine(1.0,//GAIN
                                                  (double)SAMPLES_PER_SYMBOL,//SAMPLING RATE
                                                  1.0,//Symbol rate
                                                  0.35, // alpha
                                                  11.0 * SAMPLES_PER_SYMBOL// number of taps
                                                 );
    d_rrc_rx = gr_make_interp_fir_filter_ccf(1, d_rrc_rx_taps);
    float mm_mu = 0.5;
    float mm_omega = (float)SAMPLES_PER_SYMBOL;
    float mm_gain_mu = 0.1;
    float mm_gain_omega  = 0.25 * mm_gain_mu * mm_gain_mu;
    float costas_alpha = 0.02;
    float costas_beta = 0.25 * costas_alpha * costas_alpha;
    float fmin = -0.25;
    float fmax = 0.25;
    float mm_omega_relative_limit= 0.005;
    d_mpsk_receiver = gr_make_mpsk_receiver_cc(2,
                                               0,
                                               costas_alpha,
                                               costas_beta,
                                               fmin, fmax,
                                               mm_mu,
                                               mm_gain_mu,
                                               mm_omega,
                                               mm_gain_omega,
                                               mm_omega_relative_limit);
    printf("Costas Loop alpha = %.2e\n", costas_alpha);
    printf("Costas Loop beta  = %.2e\n", costas_beta);
    printf("M&M Mu            = %.2f\n", mm_mu);
    printf("M&M Mu Gain       = %.2e\n", mm_gain_mu);
    printf("M&M Omega         = %.2f\n", mm_omega);
    printf("M&M Omega Gain    = %.2e\n", mm_gain_omega);
    printf("M&M Omega Limit   = %.2e\n", mm_omega_relative_limit);

    std::vector<unsigned char> bits;
    bits.push_back(0);
    bits.push_back(1);
    d_slicer = make_soft_slicer(SLICE_MODE.NEG_MAPPING);

    //Setup for correlator and frame sink
    d_rcvd_pktq = gr_make_msg_queue();
    std::string access_code_1_0_string("1010110011011101101001001110001011110010100011000010000011111100");
    std::bitset<ACCESS_CODE_LEN> access_code_bitset(access_code_1_0_string);
    d_correlator = make_correlate_sync_word(access_code_bitset, ACCESS_CODE_LEN, 15); //threashold
    d_framer = make_soft_frame_contor(d_rcvd_pktq);

    this->connect (d_usrp_source_sptr, 0, d_channel_filter, 0);
    this->connect (d_channel_filter, 0, d_agc, 0);
    this->connect (d_agc, 0, d_rrc_rx, 0);
    this->connect (d_rrc_rx, 0, d_mpsk_receiver, 0);
    this->connect (d_mpsk_receiver, 0, d_slicer, 0);
    this->connect (d_slicer, 0, d_correlator, 0);
    this->connect (d_correlator, 0, d_framer_sink, 0);
    d_queue_watcher = new queue_watcher_thread(d_rcvd_pktq, &callback);




}

bool top_block::send_pkt(std::string mpdu, bool eof)
{
    gr_message_sptr msg;
    if (eof)
        msg = gr_make_message(1);
    else{
        std::string ppdu;
        packet_utils::make_packet(mpdu, SAMPLES_PER_SYMBOL, BITS_PER_SYMBOL,ppdu);// mpdu should be already whitened
        msg = gr_make_message_from_string(ppdu);
//        std::cout << "PPDULEN = " << ppdu.size() << "Send need time " << ppdu.size()*8.0/500 << " ms" << std::endl;
    }
    d_message_source->msgq()->insert_tail(msg);
    return true;
}
