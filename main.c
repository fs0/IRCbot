#include "bot.h"

int main(int argc, char *argv[])
{
    int sckt, ret;

    #ifdef DEBUG
    logprint("start main()");
    #endif

    if (argc < 6) {
        printf("\n");
        printf("  parameters: server port nick realname channel [mute]\n");
        printf("\n");
        printf("  server:   freenode server rotation is chat.freenode.net\n");
        printf("  port:     usually 6667\n");
        printf("  nick:     nickname\n");
        printf("  realname: preferably your own nick (in case of problems with the bot)\n");
        printf("  channel:  channelname (put a \\ before the #)\n");
        printf("  mute:     set this to non-zero to mute the bot (optional)\n");
        printf("\n");
        return 0;
    }

    if (argc >= 7) {
        mute = atoi(argv[6]); /* set mute state; mute declared in bot.h */
    } else {
        mute = 0; /* default, not muted; mute declared in bot.h */
    }

    logFlag = 0; /* default, don't log messages; logFlag declared in bot.h */

    do {
        #ifdef DEBUG
        logprint("main(): connectirc()");
        #endif
        sckt = connectirc(argv[1], argv[2]);

        if (sckt == -1) {
            #ifdef DEBUG
            logprint("socket == -1");
            #endif
            ret = -1;
        } else {
            #ifdef DEBUG
            logprint("main(): init()");
            #endif

            if (logFlag) {
                msglogprint("--- Log opened ---\n");
            }

            ret = init(sckt, argv[3], argv[3], argv[4], argv[5]);
        }

        if (ret == -1) {
            #ifdef DEBUG
            logprint("ret == -1: sleep()");
            #endif
            sleep(120); /* wait 2 min until reconnect */
        }

    } while (ret == -1);

    #ifdef DEBUG
    logprint("end main()");
    #endif

    return 0;
}
