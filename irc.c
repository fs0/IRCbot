#include "irc.h"

int connectirc(char *server, char *port)
{
    int sckt;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct timeval tv;

    #ifdef DEBUG
    logprint("start connectirc()");
    #endif

    memset(&hints, 0, sizeof(struct addrinfo));
    /*hints.ai_family = AF_UNSPEC;*/ /* allow ipv4 or ipv6 */
    hints.ai_family = AF_INET; /* allow ipv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0; /* any protocol */

    if (getaddrinfo(server, port, &hints, &result) != 0) {
        errprint("getaddrinfo()");
        return -1;
    }

    #ifdef DEBUG
    logprint("start loop through address structures");
    #endif
    /* getaddrinfo() returns a list of address structures.
     * Try each address until we successfully connect.
     * If socket() (or connect()) fails, we (close the socket
     * and try the next address)
     */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if ((sckt = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
            #ifdef DEBUG
            logprint("socket() returned -1");
            #endif
            continue;
        }
        if ((connect(sckt, rp->ai_addr, rp->ai_addrlen)) != -1) {
            #ifdef DEBUG
            logprint("connect() successful");
            #endif
            break; /* successfully connected */
        }
        #ifdef DEBUG
        logprint("connect() returned -1");
        #endif
        close(sckt);
    }
    #ifdef DEBUG
    logprint("end loop through address structures");
    #endif

    /* no address succeeded */
    if (rp == NULL) {
        #ifdef DEBUG
        logprint("rp is NULL");
        #endif
        errprint("failed to connect");
        #ifdef DEBUG
        logprint("freeaddrinfo(result)");
        #endif
        freeaddrinfo(result);
        return -1;
    }

    #ifdef DEBUG
    logprint("freeaddrinfo(result)");
    #endif
    freeaddrinfo(result);

    tv.tv_sec = 180; /* 3min timeout */
    tv.tv_usec = 0;

    #ifdef DEBUG
    logprint("setsockopt()");
    #endif

    /* set timeout */
    if (setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, (socklen_t)sizeof(struct timeval)) == -1) {
        errprint("setsockopt()");
        close(sckt);
        return -1;
    }

    #ifdef DEBUG
    logprint("end connectirc()");
    #endif

    return sckt;
}

int disconnectirc(int sckt)
{
    int ret;

    #ifdef DEBUG
    logprint("start disconnectirc()");
    #endif

    ret = quit("Terminated...", sckt);
    sleep(1);

    #ifdef DEBUG
    logprint("trying to close socket");
    #endif
    if (close(sckt) == -1) {
        errprint("close(sckt)");
    }

    #ifdef DEBUG
    logprint("end disconnectirc()");
    #endif

    return ret;
}

int loginpass(int sckt)
{
    #ifdef DEBUG
    logprint("start loginpass()");
    #endif

    char tmp[MAX];
    snprintf(tmp, MAX, "%s", "PASS drowssap\r\n");

    if (writeserver(tmp, sckt) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end loginpass()");
    #endif
    return 0;
}

int setnick(char *nick, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start setnick()");
    #endif

    /*strcpy(tmp, "NICK ");*/
    snprintf(tmp, MAX, "%s", "NICK ");
    strncat(tmp, nick, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end setnick()");
    #endif
    
    return 0;
}

int setuser(char *username, char *realname, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start setuser()");
    #endif

    /*strcpy(tmp, "USER ");*/
    snprintf(tmp, MAX, "%s", "USER ");
    strncat(tmp, username, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, " 0 * :", MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, realname, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt) == -1 ) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end setuser()");
    #endif

    return 0;
}

int setmode(char *nick, char *mode, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start setmode()");
    #endif

    snprintf(tmp, MAX, "%s", "MODE ");
    strncat(tmp, nick, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, " ", MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, mode, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, " ", MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt) == -1 ) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end setmode()");
    #endif

    return 0;
}

int waitForResponse(int sckt)
{
    char serverline[MAX];

    #ifdef DEBUG
    logprint("start waitForResponse()");
    #endif

    while (1==1) { 

        memset(serverline, 0, MAX);

        if (readserver(serverline, sckt) == -1) {
            return -1;
        }

        if (strfind(serverline, ":End of /MOTD")) {
            return 0;
        } else if (strfind(serverline, "PING :")) {
            if (sendpong(sckt, serverline) == -1) {
                return -1;
            }
        }
    }
    #ifdef DEBUG
    logprint("end waitForResponse()");
    #endif

    return 0;
}

int joinchannel(char *channel, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start joinchannel()");
    #endif

    /*strcpy(tmp, "JOIN ");*/
    snprintf(tmp, MAX, "%s", "JOIN ");
    strncat(tmp, channel, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end joinchannel()");
    #endif

    return 0;
}

int sendpong(int sckt, char *s)
{
    char tmp[MAX];
    char tmp2[MAX];

    #ifdef DEBUG
    logprint("start sendpong()");
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

    if (writeserver(tmp2, sckt) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end sendpong()");
    #endif

    return 0;
}

int readserver(char *s, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start readserver()");
    #endif

    if (readline(sckt, tmp, sizeof(tmp)) == -1) {
        errprint("readserver()");
        return -1;
    }

    if (strfind(tmp, "Closing Link")) {
        errprint("Closing Link");
        return -1;
    }

    snprintf(s, MAX, "%s", tmp);

    #ifdef DEBUG
    logprint("readserver: ");
    logprint(s);
    #endif

    #ifdef DEBUG
    logprint("end readserver()");
    #endif

    return 0;
}

int writeserver(char *s, int sckt)
{
    #ifdef DEBUG
    logprint("start writeserver()");
    #endif

    /*if (write(sckt, s, strlen(s)) == -1)*/
    if (write(sckt, s, strnlen(s, MAX)) == -1) {
        errprint("writeserver()");
        return -1;
    }

    #ifdef DEBUG
    logprint(s);
    logprint("end writeserver()");
    #endif

    return 0;
}

int quit(char *msg, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start quit()");
    #endif

    /*strcpy(tmp, "QUIT ");*/
    snprintf(tmp, MAX, "%s", "QUIT ");
    strncat(tmp, msg, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end quit()");
    #endif

    return 0;
}
