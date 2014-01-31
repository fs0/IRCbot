#include "bot.h"

extern int mute;

int main(int argc, char *argv[])
{
    int sckt;
    int ret;

    #ifdef DEBUG
    logprint("start main()\n");
    #endif

    if (argc < 5) {
        printf("\n");
        printf("  parameters: <server> <port> <nick> <channel> [mute]\n");
        printf("\n");
        printf("  server:  freenode server rotation is chat.freenode.net\n");
        printf("  port:    usually 6667\n");
        printf("  nick:    nickname\n");
        printf("  channel: channelname (put a \\ before the #)\n");
        printf("  mute:    set this to non-zero to mute the bot (optional)\n");
        printf("\n");
        return 0;
    }

    if (argc == 6) {
        mute = atoi(argv[5]); // set mute state
    } else {
        mute = 0;
    }

    do {
        #ifdef DEBUG
        logprint("main(): connectirc()\n");
        #endif
        sckt = connectirc(argv[1], argv[2]);

        if (sckt == -1) {
            #ifdef DEBUG
            logprint("socket == -1\n");
            #endif
            ret = -1;
        } else {
            #ifdef DEBUG
            logprint("main(): init()\n");
            #endif
            ret = init(sckt, argv[3], "guest", "Name", argv[4]);
        }

        if (ret == -1) {
            #ifdef DEBUG
            logprint("ret == -1: sleep()\n");
            #endif
            sleep(120); // wait 2 min until reconnect
        }

    } while (ret == -1);

    #ifdef DEBUG
    logprint("end main()\n");
    #endif

    return 0;
}
