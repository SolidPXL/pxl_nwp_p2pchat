#ifndef CHAT_H
#define CHAT_H

#include "message.h"
#include <list>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <time.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

class Chat{
    private:
    std::list<struct Message> chatlog;
    public:
    struct Message processMessage(char* buf, int limit);
    struct MessageToSend prepareMessage(struct Message message);
    void addMessage(struct Message message);
};

#endif