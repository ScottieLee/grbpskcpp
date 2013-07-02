#include "queue_watcher_thread.h"

queue_watcher_thread::queue_watcher_thread(gr_msg_queue_sptr rcvd_pktq, callbacktype1 callback) :
        stop_flag(false),d_rcvd_pktq(rcvd_pktq),d_callback(callback)
{
    this -> start();
}
