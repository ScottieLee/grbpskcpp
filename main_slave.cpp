#include <QtCore/QCoreApplication>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include "queue_watcher_thread.h"
#include "top_block.h"
#include <gr_realtime.h>
#include <time.h>

#include "string_functions.h"
#include <time.h>
#include <sys/time.h>
#include <signal.h>


struct timeval start_time; //NOTICE global variable




unsigned int convert_time_to_time_stamp(timeval start_time, timeval time_in);
void send_pkt_at_time(top_block_sptr tb, std::string& pkt, bool eof, unsigned int trigger_time_stamp, timeval time_start ,timeval time_now);
void send_pkt_at_time(top_block_sptr tb, std::string& pkt, bool eof, timeval trigger_time, timeval time_now);


void microsleep(long us)
{
    struct timespec wait;
    wait.tv_sec = us / (1000 * 1000);
    wait.tv_nsec = (us % (1000 * 1000)) * 1000;
    nanosleep(&wait, NULL);
}


void callback(bool ok, std::string mpdu, timeval lt_timeval)
{
    std::cout << "We've received a mpdu " << std::endl;
    std::cout << string_to_hex(mpdu) <<std::endl;
    unsigned int gt_receive;
    unsigned int lt = convert_time_to_time_stamp(start_time, lt_timeval);
    std::string gt_receive_string = mpdu.substr(0,4);//NOTICE USAGE OF SUBSTR() !!
    conv_string_to_uint(gt_receive_string, gt_receive);
    std::cout << gt_receive << "\t" << lt << "\t" << lt - gt_receive << std::endl;
}
void send_pkt_at_time(top_block_sptr tb, std::string& pkt, bool eof, timeval trigger_time, timeval time_now)
{
    long long t = ((trigger_time.tv_sec * 1000000) + trigger_time.tv_usec) - ((time_now.tv_sec * 1000000) + time_now.tv_usec);
    microsleep(t);
    tb->send_pkt(pkt, eof);
}

void send_pkt_at_time(top_block_sptr tb, std::string& pkt, bool eof, unsigned int trigger_time_stamp, timeval time_start ,timeval time_now)
{
    int time_stamp_diff = trigger_time_stamp - convert_time_to_time_stamp(time_start, time_now);
    assert(time_stamp_diff > 0);
    microsleep((unsigned int)time_stamp_diff*10);
    tb->send_pkt(pkt, eof);
}


unsigned int convert_time_to_time_stamp(timeval start_time, timeval time_in)
{
    return (time_in.tv_sec * 100000 + time_in.tv_usec/10) - (start_time.tv_sec * 100000 + start_time.tv_usec/10);//FIXME ROLLOVER!
}




int main(int argc, char *argv[])
{

    char packet_array[100];
    for (int v = 0; v < 100; v++){
        packet_array[v] = 0xbb;
    }
    std::string packet = std::string((char*)packet_array, 100);

    top_block_sptr tb = make_top_block_sptr();
    gr_rt_status_t r = gr_enable_realtime_scheduling(); //FIXME
    // defines
    std::string mpdu;
    std::string gt;
    struct timeval time_now;
    unsigned int trigger_time_stamp = 100; // trigger first packet at 1ms
    //----------------------Start USRP----------------------//
    tb->start();
    gettimeofday(&start_time, NULL);
    tb->wait();

 //   QCoreApplication a(argc, argv);
 //   return a.exec();
}
