#include "irc.h"

int connectirc(char *server, char *port)
{
    int sckt;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct timeval tv;

    #ifdef DEBUG
    logprint("start connectirc()\n");
    #endif

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // allow ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0; // any protocol

    if (getaddrinfo(server, port, &hints, &result) != 0) {
        errprint("getaddrinfo()\n");
        return -1;
    }

    #ifdef DEBUG
    logprint("start loop through address structures\n");
    #endif
    /* getaddrinfo() returns a list of address structures.
     * Try each address until we successfully connect.
     * If socket() (or connect()) fails, we (close the socket
     * and try the next address)
     */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if ((sckt = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
            #ifdef DEBUG
            logprint("socket() returned -1\n");
            #endif
            continue;
        }
        if ((connect(sckt, rp->ai_addr, rp->ai_addrlen)) != -1) {
            #ifdef DEBUG
            logprint("connect() successful\n");
            #endif
            break; // successfully connected
        }
        #ifdef DEBUG
        logprint("connect() returned -1\n");
        #endif
        close(sckt);
    }
    #ifdef DEBUG
    logprint("end loop through address structures\n");
    #endif

    // no address succeeded
    if (rp == NULL) {
        #ifdef DEBUG
        logprint("rp is NULL\n");
        #endif
        errprint("failed to connect\n");
        #ifdef DEBUG
        logprint("freeaddrinfo(result)\n");
        #endif
        freeaddrinfo(result);
        return -1;
    }

    #ifdef DEBUG
    logprint("freeaddrinfo(result)\n");
    #endif
    freeaddrinfo(result);

    tv.tv_sec = 180; // 3min timeout
    tv.tv_usec = 0;

    #ifdef DEBUG
    logprint("setsockopt()\n");
    #endif

    // set timeout
    if (setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, (socklen_t)sizeof(struct timeval)) == -1) {
        errprint("setsockopt()\n");
        close(sckt);
        return -1;
    }

    #ifdef DEBUG
    logprint("end connectirc()\n");
    #endif

    return sckt;
}

int disconnectirc(int sckt)
{
    int ret;

    #ifdef DEBUG
    logprint("start disconnectirc()\n");
    #endif

    ret = quit("Terminated...", sckt);
    logprint("disconnecting\n");
    sleep(1);

    #ifdef DEBUG
    logprint("trying to close socket\n");
    #endif
    if (close(sckt) == -1) {
        errprint("close(sckt)\n");
    }
    logprint("disconnected\n");

    #ifdef DEBUG
    logprint("end disconnectirc()\n");
    #endif

    return ret;
}

int loginpass(int sckt)
{
    #ifdef DEBUG
    logprint("start loginpass()\n");
    #endif

    char tmp[MAX];
    snprintf(tmp, MAX, "%s", "PASS drowssap\r\n");

    if (writeserver(tmp, sckt, 1) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end loginpass()\n");
    #endif
    return 0;
}

int setnick(char *nick, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start setnick()\n");
    #endif

    /*strcpy(tmp, "NICK ");*/
    snprintf(tmp, MAX, "%s", "NICK ");
    strncat(tmp, nick, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt, 1) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end setnick()\n");
    #endif
    
    return 0;
}

int setuser(char *username, char *realname, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start setuser()\n");
    #endif

    /*strcpy(tmp, "USER ");*/
    snprintf(tmp, MAX, "%s", "USER ");
    strncat(tmp, username, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, " 0 * :", MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, realname, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt, 1) == -1 ) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end setuser()\n");
    #endif

    return 0;
}

int waitForResponse(int sckt)
{
    char serverline[MAX];

    #ifdef DEBUG
    logprint("start waitForResponse()\n");
    #endif

    while (1==1) { 

        memset(serverline, 0, MAX);

        if (readserver(serverline, sckt, 1) == -1) {
            return -1;
        }

        if (strfind(serverline, ":End of /MOTD") == 0) {
            return 0;
        } else if (strfind(serverline, "PING :") == 0) {
            if (sendpong(sckt, serverline) == -1) {
                return -1;
            }
        }
    }
    #ifdef DEBUG
    logprint("end waitForResponse()\n");
    #endif

    return 0;
}

int joinchannel(char *channel, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start joinchannel()\n");
    #endif

    /*strcpy(tmp, "JOIN ");*/
    snprintf(tmp, MAX, "%s", "JOIN ");
    strncat(tmp, channel, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt, 1) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end joinchannel()\n");
    #endif

    return 0;
}

int sendpong(int sckt, char *s)
{
    char tmp[MAX];
    char tmp2[MAX];

    #ifdef DEBUG
    logprint("start sendpong()\n");
    #endif

    /*strcpy(tmp, s);*/
    snprintf(tmp, MAX, "%s", s);

    if (getmsg(tmp, "PING :") == 0) /*get message after "PING :"*/ {
        /*strcpy(tmp2, "PONG :");*/
        snprintf(tmp2, MAX, "%s", "PONG :");
        strncat(tmp2, tmp, MAX-strnlen(tmp2, MAX)-1);
    } else {
        /*strcpy(tmp2, "PONG");*/
        snprintf(tmp2, MAX, "%s", "PONG");
    }

    if (writeserver(tmp2, sckt, 0) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end sendpong()\n");
    #endif

    return 0;
}

int readserver(char *s, int sckt, int logging)
{
    char tmp[MAX];
    FILE *log;

    #ifdef DEBUG
    logprint("start readserver()\n");
    #endif

    log = fopen("./bot.log", "a");

    if (readline(sckt, tmp, sizeof(tmp)) == -1) {
        errprint("readserver()\n");
        return -1;
    }

    if (strfind(tmp, "Closing Link") == 0) {
        errprint("Closing Link");
        return -1;
    }

    // don't log pings
    if (strfind(tmp, "PING :") == 0) {
        logging = 0;
    }
    
    if (logging == 1) {
        fprintf(log, "%s", tmp);
    }

    /*strcpy(s, tmp);*/
    snprintf(s, MAX, "%s", tmp);
    fclose(log);

    #ifdef DEBUG
    logprint("readserver: ");
    logprint(s);
    #endif

    #ifdef DEBUG
    logprint("end readserver()\n");
    #endif

    return 0;
}

int writeserver(char *s, int sckt, int logging)
{
    FILE *log;

    #ifdef DEBUG
    logprint("start writeserver()\n");
    #endif

    log = fopen("./bot.log", "a");

    /*if (write(sckt, s, strlen(s)) == -1)*/
    if (write(sckt, s, strnlen(s, MAX)) == -1) {
        errprint("writeserver()\n");
        return -1;
    }
    if (logging == 1) {
        fprintf(log, "%s", s);
    }
    fclose(log);

    #ifdef DEBUG
    logprint("end writeserver()\n");
    #endif

    return 0;
}

int quit(char *msg, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start quit()\n");
    #endif

    /*strcpy(tmp, "QUIT ");*/
    snprintf(tmp, MAX, "%s", "QUIT ");
    strncat(tmp, msg, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt, 1) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end quit()\n");
    #endif

    return 0;
}
