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
        printf("NOTE: put a \\ before the # of the channelname\n\n");
        return -1;
    }

    sckt = connectirc(argv[1], atoi(argv[2]));
    if (sckt == -1)
    {
        return -1;
    }

    return init(sckt, argv[3], "guest", "Name", argv[4]);
}
