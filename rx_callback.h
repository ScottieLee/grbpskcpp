#ifndef RX_CALLBACK_H
#define RX_CALLBACK_H
#include <string>
#include <time.h>
#include <sys/time.h>


//Put the rx callback functions typedef and declarance here
typedef void (*callbacktype1) (bool, std::string, timeval);
void callback1(bool ok, std::string mpdu, timeval lt_timeval);




#endif // RX_CALLBACK_H
