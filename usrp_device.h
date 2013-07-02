#ifndef USRP_DEVICE_H
#define USRP_DEVICE_H
#include <string>
#include <usrp_sink_c.h>
#include <usrp_source_c.h>
#include <boost/make_shared.hpp>

/*--------------------------------------------------*/
// A Interface of USRP blocks



/* ------------------------------------------------------*/
/* A example of using usrp_device

    usrp_device_tx& utx = usrp_device_tx::create_utx("A"); // Just specify the board side.
    utx.set_interp(32); // Set interp rate
    utx.usrpdev_set_freq("2.43G"); // Set frequency
    usrp_sink_c_sptr utx_block_sptr = utx.get_sptr(); // We get the usrp block. Done.
    // Then use utx_block_sptr in the flow graph construction...
    ...
*/





/*----------------------------------------
 Abstract class of usrp device
 ----------------------------------------*/
class usrp_device
{
public:
    usrp_device(); //Default contor.
    usrp_device(int which_board,
                int fusb_block_size,
                int fusb_nblocks
                ); //Contor.

protected:
    virtual bool usrpdev_set_freq(double target_freq) = 0;
    virtual void set_mux(int mux) = 0;
    virtual void set_gain(float gain) = 0;
    virtual void display_info(void) = 0;
    double freq_input_handling(std::string freq_str); // E.g. Input "2.43G" ouput 2430000000.0
    usrp_subdev_spec select_subdev(std::string);

private:
    int d_which;
    int d_fusb_block_size;
    int d_fusb_nblocks;
};

inline double usrp_device::freq_input_handling(std::string freq_str)
{
    if (freq_str.empty()){
        fprintf(stderr, "Invalid Frequency input. Format is 2.4G or 200K or 300M ..");
        ::exit(1);
    }
    else if (freq_str[freq_str.length()-1] == 'G') {
        freq_str.resize(freq_str.length() - 1);
        double tmp_double   = (double)atof(freq_str.c_str());
        assert(tmp_double > 1);
        return tmp_double * 1000000000.0;
    }
    else if (freq_str[freq_str.length()-1] == 'M') {
        freq_str.resize(freq_str.length() - 1);
        double tmp_double   = (double)atof(freq_str.c_str());
        assert(tmp_double > 1);
        return tmp_double * 1000000.0;
    }
    else if (freq_str[freq_str.length()-1] == 'K') {
        freq_str.resize(freq_str.length() - 1);
        double tmp_double   = (double)atof(freq_str.c_str());
        assert(tmp_double > 1);
        return tmp_double * 1000.0;
    }
    else{
        double tmp_double   = (double)atof(freq_str.c_str());
        assert(tmp_double > 1);
        return tmp_double;
    }
}

/*----------------------------------------
  Class of usrp device tx It's a interface of usrp_sink_c
 ----------------------------------------*/

class usrp_device_tx: public usrp_device
{
private:
    usrp_device_tx(
                   const std::string side = "",
                   int which_board=0,
                   unsigned int interp_rate=32,
                   int fusb_block_size=0,
                   int fusb_nblocks=0,
                   int nchan=1,
                   int mux=-1,
                   const std::string fpga_filename="",
                   const std::string firmware_filename="");
public:

    static usrp_device_tx& create_utx(      const std::string side,
                                           int which_board,
                                           unsigned int interp_rate,
                                           int fusb_block_size,
                                           int fusb_nblocks,
                                           int nchan,
                                           int mux,
                                           const std::string fpga_filename,
                                           const std::string firmware_filename)
    {
        static usrp_device_tx utx_instance(side, which_board, interp_rate, fusb_block_size, nchan, mux, fpga_filename, firmware_filename);
        return utx_instance;
    }


    bool usrpdev_set_freq(double target_freq);
    void display(void);
    void set_interp( int interp);
    usrp_sink_c_sptr& get_sptr() { return d_utx; }


private:
    unsigned int      d_interp;
    int               d_mux;
    usrp_sink_c_sptr  d_utx;
    db_base_sptr      d_sub_device_tx;
    usrp_subdev_spec  d_spec_tx;
    usrp_tune_result  d_tune_result_tx;


};

inline void usrp_device_tx::display(void)
{
    std::cout << "---------------------------TX Settings--------------------------\n";
    printf ("      Actural frequecy = %f\n", (d_tune_result_tx.baseband_freq + d_tune_result_tx.dxc_freq));
    printf ("      Residual frequency = %f\n", d_tune_result_tx.residual_freq);
    printf ("      Baseband Freq = %f, DXC Freq = %f\n", d_tune_result_tx.baseband_freq, d_tune_result_tx.dxc_freq);
    printf      ("Subdevice name is %s\n", d_sub_device_tx -> name().c_str());
    printf      ("Subdevice freq range: (%g, %g)\n", d_sub_device_tx -> freq_min(), d_sub_device_tx->freq_max());
    std::cout << "Hardware Interpolation = " << d_interp << std::endl;
}


/*----------------------------------------
  Class of usrp device rx It's a interface of usrp_source_c
 ----------------------------------------*/


class usrp_device_rx: public usrp_device
{
private:
    usrp_device_rx(
                   const std::string side = "A",
                   int which_board=0,
                   unsigned int decim_rate=16,
                   int fusb_block_size = 0,
                   int fusb_nblocks = 0,
                   int nchan = 1,
                   int mux = -1,
                   int mode = 0,
                   const std::string fpga_filename = "",
                   const std::string firmware_filename = "");
public:
    static usrp_device_rx& create_urx(   const std::string side,
                                          int which_board,
                                          unsigned int decim_rate,
                                          int fusb_block_size,
                                          int fusb_nblocks,
                                          int nchan,
                                          int mux,
                                          int mode,
                                          const std::string fpga_filename,
                                          const std::string firmware_filename)
    {
        static usrp_device_rx urx_instance(side, which_board, decim_rate, fusb_block_size, fusb_nblocks, nchan, mux, mode, fpga_filename, firmware_filename);
        return urx_instance;
    }

    bool usrp_device_rx::usrpdev_set_freq(double target_freq);
    void usrp_device_rx::display(void);
    void set_decim( int decim);
    usrp_source_c_sptr& get_sptr() { return d_urx; }

private:
    unsigned int        d_decim;
    int                 d_mux;
    usrp_source_c_sptr  d_urx;
    db_base_sptr        d_sub_device_rx;
    usrp_subdev_spec    d_spec_rx;
    usrp_tune_result    d_tune_result_rx;


};

inline void usrp_device_rx::display(void)
{
    std::cout << "---------------------------USRP RX Settings--------------------------" << std::endl;
    std::cout << "      Actural frequecy = " << (d_tune_result_rx.baseband_freq + d_tune_result_rx.dxc_freq) << std::endl;
    std::cout << "      Residual frequency = " << d_tune_result_rx.residual_freq << std::endl;
    printf      ("Subdevice name is %s\n", d_sub_device_rx -> name().c_str());
    printf      ("Subdevice freq range: (%g, %g)\n", d_sub_device_rx -> freq_min(), d_sub_device_rx->freq_max());
    std::cout << "Hardware Decimation    = " << d_decim << std::endl;
}

#endif // USRP_DEVICE_H
