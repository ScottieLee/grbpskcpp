#ifndef QUEUE_WATCHER_THREAD_H
#define QUEUE_WATCHER_THREAD_H

#include <QThread>
#include <string>
#include <gr_message.h>
#include <gr_msg_queue.h>
#include <packet_utils.h>
#include <time.h>
#include <sys/time.h>
#include "rx_callback.h"

// Todo Make it a template of callbacktype!

class queue_watcher_thread : public QThread
{
    Q_OBJECT
public:
    queue_watcher_thread(gr_msg_queue_sptr rcvd_pktq, callbacktype1 callback);

    void run()
    {
        while(!stop_flag)
        {
            struct timeval time_now;
            gr_message_sptr msg = d_rcvd_pktq-> delete_head();
            bool ok;
            std::string mpdu;
            packet_utils::ppdu2mpdu(msg->to_string(),mpdu, ok);
            gettimeofday(&time_now, NULL);
            d_callback(ok, mpdu,time_now); //FIXME!!!

         }
    }
private:
    volatile bool                 stop_flag;
    gr_msg_queue_sptr             d_rcvd_pktq;
    callbacktype1                 d_callback;
};

#endif // QUEUE_WATCHER_THREAD_H
