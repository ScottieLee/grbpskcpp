#include "reception_statistics.h"

reception_statistics::reception_statistics(unsigned int slave_amount):
        d_total_rev(0), d_last_rev_pktno(0),
        d_slave_rev_num(slave_amount, 0), // slave_amount of zeros
        d_slave_last_rev_pktno(slave_amount, 0) // slave_amount of zeros
{

}

reception_statistics& reception_statistics::make_reception_statistics(unsigned int slave_amount)
{
    static reception_statistics r_s(slave_amount);
    return r_s;
}
