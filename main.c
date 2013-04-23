#include "bot.h"

int main(int argc, char *argv[])
{
    int sckt;
    int ret;

    if (argc != 5)
    {
        printf("\nparameters: <server> <port> <nick> <channel>\n");
        printf("server: freenode server rotation is chat.freenode.net\n");
        printf("port: usually 6667\n");
        printf("Additionally, you'll need two textfiles:\n");
        printf("  messages.txt: general messages\n");
        printf("  personal.txt: messages the bot will use when addressed personally.\n");
        printf("NOTE: put a \\ before the # of the channelname\n\n");
        return -1;
    }

    do
    {
        sckt = connectirc(argv[1], atoi(argv[2]));

        if (sckt == -1)
        {
            ret = -1;
        }
        else
        {
            ret = init(sckt, argv[3], "guest", "Name", argv[4]);
        }

        if (ret == -1)
        {
            sleep(120); // wait 2 min until reconnect
        }

    } while (ret == -1);

    return 0;
}
