#include "usrp_device.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <cassert>

usrp_subdev_spec usrp_device::select_subdev(const std::string& side)
{
    if (side == "A" || side == "a")
        return usrp_subdev_spec(0,0);
    else if (side == "B" || side == "b")
        return usrp_subdev_spec(1,0);
    else {
        fprintf(stderr, "USRP Device: Not valid USRP side, A or B.");
        ::exit(-1);
    }
}



usrp_device_tx::usrp_device_tx(
               const std::string side,
               int which_board,
               unsigned int interp_rate,
               int fusb_block_size,
               int fusb_nblocks,
               int nchan,
               int mux,
               const std::string fpga_filename,
               const std::string firmware_filename)
               :usrp_device(which_board, fusb_block_size, fusb_nblocks),
               d_interp(interp_rate),
               d_mux(mux)
{
    d_usrp_sink_sptr = usrp_make_sink_c(d_which, d_interp, nchan, mux, d_fusb_block_size, d_fusb_nblocks, fpga_filename, firmware_filename);
    if(!(d_usrp_sink_sptr -> set_interp_rate (d_interp)))
        fprintf(stderr, "USRP TX Dev: Failed to set interpolation rate.");
    if (side.empty()){
        d_spec_tx = d_usrp_sink_sptr -> pick_tx_subdevice();
    }
    else{
        d_spec_tx = select_subdev(side);
    }
    d_sub_device_tx = d_usrp_sink_sptr -> selected_subdev (d_spec_tx);
    d_usrp_sink_sptr -> set_mux (d_usrp_sink_sptr -> determine_tx_mux_value (d_spec_tx));
    d_sub_device_tx ->set_auto_tr(true);
    d_sub_device_tx ->set_gain((d_sub_device_tx ->gain_max() + d_sub_device_tx -> gain_min())/2);
}

inline void usrp_device_tx::set_gain(float gain)
{
    d_sub_device_tx -> set_gain(gain);
}

inline void usrp_device_tx::set_mux(int mux)
{
    d_sub_device_tx -> set_mux(mux);
}

inline void usrp_device_tx::set_interp_rate (int interp)
{
    d_interp = interp;
    d_usrp_sink_sptr-> set_interp_rate(interp);
}


bool usrp_device_tx::usrpdev_set_freq(std::string& freq_str)
{
    // Handling String to  double convertion
    double target_freq = freq_input_handling(freq_str);

    bool ok_tx = d_usrp_sink_sptr -> tune (0, d_sub_device_tx, target_freq, &d_tune_result_tx);
    if(!ok_tx){
        fprintf(stderr, "USRP TX Dev: Failed to set tx frequency");
        return false;
    }
    else
        return true;
}




usrp_device_rx::usrp_device_rx(
               const std::string side,
               int which_board,
               unsigned int decim_rate,
               int fusb_block_size,
               int fusb_nblocks,
               int nchan,
               int mux,
               int mode,
               const std::string fpga_filename,
               const std::string firmware_filename)
               :usrp_device(which_board, fusb_block_size, fusb_nblocks),
               d_decim(decim_rate),
               d_mux (mux)

{
    d_usrp_source_sptr = usrp_make_source_c(d_which, d_decim, nchan, mux, mode, d_fusb_block_size, d_fusb_nblocks, fpga_filename, firmware_filename);
    if (!(d_usrp_source_sptr -> set_decim_rate (d_decim)))
        fprintf(stderr, "USRP RX Dev: Failed to set decimation rate.");
    if (side.empty()){
        d_spec_rx = d_usrp_source_sptr -> pick_rx_subdevice();
    }
    else{
        d_spec_rx = select_subdev(side);
    }
    d_sub_device_rx = d_usrp_source_sptr -> selected_subdev (d_spec_rx);
    d_usrp_source_sptr -> set_mux (d_usrp_source_sptr -> determine_rx_mux_value (d_spec_rx));
    d_sub_device_rx ->set_auto_tr(true);
    d_sub_device_rx -> set_gain((d_sub_device_rx ->gain_max() + d_sub_device_rx -> gain_min())/2);
    d_sub_device_rx ->select_rx_antenna(0);
}

inline void usrp_device_rx::set_gain(float gain)
{
    d_sub_device_rx -> set_gain(gain);
}

inline void usrp_device_rx::set_mux(int mux)
{
    d_sub_device_rx -> set_gain(mux);
}

inline void usrp_device_rx::set_decim_rate(int decim)
{
    d_decim = decim;
    d_usrp_source_sptr -> set_decim_rate(decim);
}

bool usrp_device_rx::usrpdev_set_freq(double target_freq)
{
    // Handling String to  double convertion
    double target_freq = freq_input_handling(freq_str);

    bool ok_rx = d_usrp_source_sptr -> tune (0, d_sub_device_rx, target_freq, &d_tune_result_rx);
    if(!ok_rx){
        fprintf(stderr, "USRP RX Dev: Failed to set rx frequency");
        return false;
    }
    else
        return true;
}




