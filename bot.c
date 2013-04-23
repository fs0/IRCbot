#include "bot.h"

/*TODO regular expressions?*/
/*TODO reconnect if disconnected, use ret*/
/*TODO correct error handling*/
/*TODO clean up string/message functions*/

int init(int sckt, char *nick, char *username, char *realname, char *channel)
{
    int ret;

    if (loginpass(sckt) == -1) return -1;
    if (setnick(nick, sckt) == -1) return -1;
    if (setuser(username, realname, sckt) == -1) return -1;
    if (waitForResponse(sckt) == -1) return -1;
    if (joinchannel(channel, sckt) == -1) return -1;
    return loop(sckt, nick, channel);
}

int waitForResponse(int sckt)
{
    char serverline[MAX];

    while (1==1)
    { 
        memset(serverline, 0, MAX);
        readserver(serverline, sckt);
        if (strfind(serverline, ":End of /MOTD"))
        {
            return 0;
        }
        else if (strfind(serverline, "PING :") == 1)
        {
            if (sendpong(sckt, serverline) == -1) 
            {
                return -1;
            }
        }
    }
}

int loop(int sckt, char *nick, char *channel)
{
    char serverline[MAX];
    char privmsg[MAX];
    char tmp[MAX];
    char tome[MAX];
    int ret;

    /*private message to the bot*/
    /*strcpy(privmsg, "PRIVMSG ");*/
    snprintf(privmsg, MAX, "%s", "PRIVMSG ");
    strncat(privmsg, nick, MAX-strnlen(privmsg, MAX)-1);

    /*message to the bot*/
    /*strcpy(tome, "PRIVMSG ");*/
    snprintf(tome, MAX, "%s", "PRIVMSG ");
    strncat(tome, channel, MAX-strnlen(tome, MAX)-1);
    strncat(tome, " :", MAX-strnlen(tome, MAX)-1);
    strncat(tome, nick, MAX-strnlen(tome, MAX)-1);

    while (1==1)
    {
        memset(serverline, 0, MAX);
        readserver(serverline, sckt);
        
        if (strfind(serverline, "PING :") == 1)
        {
            ret = sendpong(sckt, serverline);
        }
        else if (strfind(serverline, ":!os") == 1)
        {
            memset(tmp, 0, MAX);
            osinfo(tmp);
            ret = privatemsg(tmp, channel, sckt);
        }
        else if (strfind(serverline, ":!info") == 1)
        {
            ret = privatemsg("I'm a cybernetic organism. Living tissue over a metal endoskeleton.", channel, sckt);
        }
        else if (strfind(serverline, tome) == 1)
        {
            ret = answer(sckt, serverline, channel, nick);
        }
        else if (strfind(serverline, privmsg) == 1) /*private message*/
        {
            if (strfind (serverline, ":!shutdown") == 1) /*disconnect the bot*/
            {
                ret = 0;
                break;
            }
            else if (strfind(serverline, "!send "))
            {
                if (sendcommand(serverline, sckt) == -1)
                    errprint("sendcommand()");
            }
            else
            {
                /*
                memset(tmp, 0, MAX);
                if (usernamecount(serverline) == -1)
                    continue;
                strncpy(tmp, serverline+1, usernamecount(serverline));
                ret = privatemsg("Please don't write me messages.", tmp, sckt);
                */
            }
        }
        else if ((getrand(20) == 0) && (strend(serverline, "?") == 0))
        {
            memset(tmp, 0, MAX);
            if (getLine(tmp, "./messages.txt") == -1)
                errprint("getmsg()");
            else
                ret = privatemsg(tmp, channel, sckt);

        }
        
        if (ret == -1)
        {
            break;
        }
    }

    ret = privatemsg("I'll be back.", channel, sckt);
    disconnectirc(sckt);

    return ret;
}


int answer(int sckt, char *serverline, char *channel, char* nick)
{
    char tmp[MAX];
    char textfileline[MAX];
    int ret;

    if (strfind(serverline, "hello") == 1 || strfind(serverline, "Hello") == 1)
    {
        memset(tmp, 0, MAX);
        if (usernamecount(serverline) == -1)
        {
            return 0;
        }
        strncpy(tmp, serverline+1, usernamecount(serverline));
        strcat(tmp, ": ");
        strcat(tmp, "Hello.");
        ret = privatemsg(tmp , channel, sckt);
    }
    else if (strend(serverline, "?") == 1 && yesnoq(serverline, nick) == 1) /*yes no question?*/
    {
        memset(tmp, 0, MAX);
        if (usernamecount(serverline) == -1)
        {
            return 0;
        }
        strncpy(tmp, serverline+1, usernamecount(serverline));
        strcat(tmp, ": ");
        switch(getrand(3))
        {
            case 0:    strcat(tmp, "Yes."); break;
            case 1:    strcat(tmp, "No."); break;
            case 2: strcat(tmp, "Of course. I'm a terminator. "); break;
            default: break;
        }
        ret = privatemsg(tmp , channel, sckt);
    }
    else 
    {
        memset(tmp, 0, MAX);
        memset(textfileline, 0, MAX);

        if (usernamecount(serverline) == -1)
        {
            return 0;
        }
        strncpy(tmp, serverline+1, usernamecount(serverline));
        strcat(tmp, ": ");

        if (getLine(textfileline, "./personal.txt") == -1)
        {
            errprint("getmsg()");
        }
        else
        {
            strcat(tmp, textfileline);
            ret = privatemsg(tmp , channel, sckt);
        }
    }
    return ret;
}


int connectirc(char *server, int port)
{
    int sckt;
    const int y = 1;
    struct sockaddr_in address;
    struct in_addr inaddr;
    struct hostent *host;
    FILE *log;

    if ((sckt = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        errprint("socket()");
        return -1;
    }
    //printf("socket created\n");
    log = fopen("./bot.log", "a");
    fprintf(log, "socket created\n");

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_aton(server, &inaddr) != 0) /*nonzero if valid*/
    {
        host = gethostbyaddr((const void *) &inaddr, sizeof(inaddr), AF_INET);
    }
    else
    {
        host = gethostbyname(server);
    }
    if (host == NULL)
    {
        errprint("host not found");
        if (close(sckt) == -1) 
        {
            //printf("error close(sckt)\n");
            fprintf(log, "error close(sckt)\n");
        }
        return -1;
    }
    memcpy(&address.sin_addr, host->h_addr_list[0], sizeof(address.sin_addr));

    if (connect(sckt, (struct sockaddr *) &address, sizeof(address)) == -1)
    {
        errprint("connect()");
        close(sckt);
        return -1;
    }
    //printf("connected to %s\n", inet_ntoa(address.sin_addr));
    fprintf(log, "connected to %s\n", inet_ntoa(address.sin_addr));

    fclose(log);

    return sckt;
}

int disconnectirc(int sckt)
{
    FILE *log;
    log = fopen("./bot.log", "a");
    quit("Terminated...", sckt);
    fprintf(log, "disconnecting...\n");
    sleep(1);
    if (close(sckt) == -1)
        return -1;
    fprintf(log, "disconnected\n");
    fclose(log);
    return 0;
}

int loginpass(int sckt)
{
    char tmp[MAX];
    /*strcpy(tmp, "PASS drowssap\r\n");*/
    snprintf(tmp, MAX, "%s", "PASS drowssap\r\n");
    if (writeserver(tmp, sckt) == -1)
    {
        return -1;
    }
    return 0;
}

int privatemsg(char *msg, char *dest, int sckt)
{
    char tmp[MAX];
    /*strcpy(tmp, "PRIVMSG ");*/
    snprintf(tmp, MAX, "%s", "PRIVMSG ");
    strncat(tmp, dest, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, " :", MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, msg, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);
    if (writeserver(tmp, sckt) == -1)
    {
        return -1;
    }
    return 0;
}

int setuser(char *username, char *realname, int sckt)
{
    char tmp[MAX];
    /*strcpy(tmp, "USER ");*/
    snprintf(tmp, MAX, "%s", "USER ");
    strncat(tmp, username, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, " 0 * :", MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, realname, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);
    if (writeserver(tmp, sckt) == -1 )
    {
        return -1;
    }
    return 0;
}

int joinchannel(char *channel, int sckt)
{
    char tmp[MAX];
    /*strcpy(tmp, "JOIN ");*/
    snprintf(tmp, MAX, "%s", "JOIN ");
    strncat(tmp, channel, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);
    if (writeserver(tmp, sckt) == -1)
    {
        return -1;
    }
    return 0;
}

int setnick(char *nick, int sckt)
{
    char tmp[MAX];
    /*strcpy(tmp, "NICK ");*/
    snprintf(tmp, MAX, "%s", "NICK ");
    strncat(tmp, nick, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);
    if (writeserver(tmp, sckt) == -1)
    {
        return -1;
    }
    return 0;
}

int sendcommand(char *s, int sckt)
{
    char tmp[MAX];

    /*strcpy(tmp, s);*/
    snprintf(tmp, MAX, "%s", s);
    if (getmsg(tmp, "!send ") == -1)
    {
        return -1;
    }
    if (writeserver(tmp, sckt) == -1)
    {
        return -1;
    }
    return 0;
}

int quit(char *msg, int sckt)
{
    char tmp[MAX];
    /*strcpy(tmp, "QUIT ");*/
    snprintf(tmp, MAX, "%s", "QUIT ");
    strncat(tmp, msg, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);
    if (writeserver(tmp, sckt) == -1)
    {
        return -1;
    }
    return 0;
}

int sendpong(int sckt, char *s)
{
    char tmp[MAX];
    char tmp2[MAX];

    /*strcpy(tmp, s);*/
    snprintf(tmp, MAX, "%s", s);
    if (getmsg(tmp, "PING :") == 0) /*get message after "PING :"*/
    {
        /*strcpy(tmp2, "PONG :");*/
        snprintf(tmp2, MAX, "%s", "PONG :");
        strncat(tmp2, tmp, MAX-strnlen(tmp2, MAX)-1);
    }
    /*if (strfind(s, "PING :") == 1)
    {
        strcpy(tmp, s);
        getmsg(tmp, "PING");
        strcpy(tmp2, "PONG :");
        strcat(tmp2, tmp);
    }*/
    else
    {
        /*strcpy(tmp2, "PONG");*/
        snprintf(tmp2, MAX, "%s", "PONG");
    }
    if (writeserver(tmp2, sckt) == -1)
    {
        return -1;
    }
    return 0;
}

int yesnoq(char *s, char *nick)
{
    char tmp[MAX];
    char tmp2[MAX];
    /*strcpy(tmp, nick);*/
    snprintf(tmp, MAX, "%s", nick);
    strncat(tmp, ": ", MAX-strnlen(tmp, MAX)-1);

    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "Why", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "why", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "What", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "what", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "Who", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "who", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }

    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "When", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "when", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "Where", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "where", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "How", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "how", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 1)
    {
        return 0;
    }

    return 1;
}


int readserver(char *s, int sckt)
{
    char tmp[MAX];
    int bytesread;
    FILE *log;

    log = fopen("./bot.log", "a");

    if (readline(sckt, tmp, sizeof(tmp)) == -1)
    {
        errprint("writeserver()");
        return -1;
    }
    fprintf(log, "%s", tmp);
    /*strcpy(s, tmp);*/
    snprintf(s, MAX, "%s", tmp);
    fclose(log);
    return 0;
}

int writeserver(char *s, int sckt)
{
    FILE *log;

    log = fopen("./bot.log", "a");

    /*if (write(sckt, s, strlen(s)) == -1)*/
    if (write(sckt, s, strnlen(s, MAX)) == -1)
    {
        errprint("writeserver()");
        return -1;
    }
    fprintf(log, "%s", s);
    fclose(log);
    return 0;
}

int osinfo(char *info)
{
    struct utsname name;

    if (uname(&name) == -1)
    {
        errprint("uname()");
        return -1;
    }
    /*strcpy(info, "System: ");*/
    snprintf(info, MAX, "%s", "System: ");
    strncat(info, name.sysname, MAX-strnlen(info, MAX)-1);
    strncat(info, ", release: ", MAX-strnlen(info, MAX)-1);
    strncat(info, name.release, MAX-strnlen(info, MAX)-1);
    strncat(info, ", architecture: ", MAX-strnlen(info, MAX)-1);
    strncat(info, name.machine, MAX-strnlen(info, MAX)-1);
    return 0;
}

int strfind(char *s1, char *s2)
{
    int i;
    /*for (i=0; i<strlen(s1); i++)*/
    for (i=0; i<strnlen(s1, MAX); i++)
    {
        /*if (strncmp(s1+i, s2, strlen(s2)) == 0)*/
        if (strncmp(s1+i, s2, strnlen(s2, MAX)) == 0)
            return 1;
    }
    return 0;
}

int usernamecount(char *s)
{
    int i;
    /*for (i=2; i<strlen(s); i++)*/
    for (i=2; i<strnlen(s, MAX); i++)
    {
        /*if (strncmp(s+i, "!", strlen("!")) == 0)*/
        if (strncmp(s+i, "!", strnlen("!", MAX)) == 0)
            return i-1;
    }
    return -1;
}

int strend(char *s0, char *s1)
{
    /*if (strncmp(s0+strlen(s0)-3, s1, strlen(s1)) == 0)*/
    if (strncmp(s0+strnlen(s0, MAX)-3, s1, strnlen(s1, MAX)) == 0)
        return 1;
    return 0;
}

int getmsg(char *s, char *key)
{
    int i;
    char tmp[MAX];
    /*strcpy(tmp, key);*/
    snprintf(tmp, MAX, "%s", key);
    /*strcat(tmp, " :");*/

    /*for (i=0; i<strlen(s); i++)*/
    for (i=0; i<strnlen(s, MAX); i++)
    {
        /*if (strncmp(s+i, tmp, strlen(tmp)) == 0)*/
        if (strncmp(s+i, tmp, strnlen(tmp, MAX)) == 0)
        {
            /*strcpy(s, s+i+strlen(tmp));*/
            snprintf(s, MAX, "%s", s+i+strnlen(tmp, MAX));
            return 0;
        }
    }
    return -1;
}

void errprint(char *s)
{
    FILE *log;
    log = fopen("./bot.log", "a");
    //fprintf(stderr, "ERROR %s\n", s);
    fprintf(log, "ERROR %s\n", s);
}

int getrand(int a)
{
    int fd;
    int random;
    FILE *log;

    log = fopen("./bot.log", "a");

    fd = open("/dev/urandom", O_RDONLY);
    read(fd, &random, sizeof(int));
    close(fd);
    random = abs(random) % a;
    
    /*
    // compile with -lrt
    int i, random;
    struct timespec t;

    clock_gettime(CLOCK_REALTIME, &t);
    random = t.tv_nsec % a;
    */

    fprintf(log, "random number generated: %d\n", random);
    fclose(log);

    return random;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;

    for (n=1; n<maxlen; n++)
    {
        if((rc=read(fd, &c, 1)) == 1)
        {
            *ptr++ = c;

            if (c == '\n')
            {
                break;
            }

        }
        else if (rc == 0)
        {
            if (n == 1)
                return (0);
            else
                break;

        }
        else
        {
            if (errno != EINTR)
            {
                return -1;
            }
        }
    }
    *ptr = '\0';
    return n;
}

int getLine(char *line, char *textfile)
{
    FILE *fd;
    int numLines = 0;
    int currentLine = 0;
    int randnum;
    char c;
    int i = 0;

    // open file
    fd = fopen(textfile, "r");
    if (fd == NULL)
        return -1;
    // count lines
    while ((c = getc(fd)) != EOF)
    {
        if (c == '\n')
            numLines++;
    }
    fseek(fd, 0, SEEK_SET);

    // generate random number
    randnum = getrand(numLines);
    // get the <randnum>th line
    while ((c = getc(fd)) != EOF)
    {
        if (randnum > currentLine)
        {
            if (c == '\n')
                currentLine++;
        }
        else
        {
            if (c == '\n')
            {
                break;
            }
            else
            {
                line[i] = c;
                i++;
            }
        }
        
    }

    // close file
    fclose(fd);
}
