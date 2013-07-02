#ifndef RECEPTION_STATISTICS_H
#define RECEPTION_STATISTICS_H
#include <vector>
class reception_statistics
{
private:
    explicit reception_statistics(unsigned int slave_amount);  // private contor.
    unsigned int              d_total_rev;
    unsigned int              d_last_rev_pktno;
    std::vector<unsigned int> d_slave_rev_num;
    std::vector<unsigned int> d_slave_last_rev_pktno;

public:
    static reception_statistics& make_reception_statistics(unsigned int slave_amount);  //Singleton Creator

    // which slave counts from zero
    void pkt_received(unsigned int rev_pktno, unsigned int which_slave)
    {
        d_slave_rev_num[which_slave]++;
        d_slave_last_rev_pktno[which_slave] = rev_pktno;
    }
    unsigned int get_total_rev_num() {return d_total_rev;}
    double       get_total_plr() {return static_cast<double>(d_last_rev_pktno + 1 - d_total_rev)/ static_cast<double>(d_total_rev);}
    unsigned int get_slave_rev_num(unsigned int which_slave)  // how many pkts has this slave received
    {
        return d_slave_rev_num[which_slave];
    }


};

#endif // RECEPTION_STATISTICS_H
