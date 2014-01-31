#include "bot.h"

extern int mute;

int init(int sckt, char *nick, char *username, char *realname, char *channel)
{
    int ret = 0;

    #ifdef DEBUG
    logprint("start init()\n");
    #endif

    if (loginpass(sckt) == -1) ret = -1;
    else if (setnick(nick, sckt) == -1) ret = -1;
    else if (setuser(username, realname, sckt) == -1) ret = -1;
    else if (waitForResponse(sckt) == -1) ret = -1;
    else if (joinchannel(channel, sckt) == -1) ret = -1;
    else if (loop(sckt, nick, channel) == -1) ret = -1;

    if (ret == -1) {
        #ifdef DEBUG
        logprint("trying to close socket\n");
        #endif
        if (close(sckt) == -1) {
            errprint("close(sckt)\n");
        }
        return -1;
    }
    
    #ifdef DEBUG
    logprint("end init()\n");
    #endif

    return 0;
}

int loop(int sckt, char *nick, char *channel)
{
    char serverline[MAX];
    int ret = 0;

    #ifdef DEBUG
    logprint("start loop()\n");
    #endif

    while (1==1) {

        memset(serverline, 0, MAX);

        if ( (ret = readserver(serverline, sckt)) != -1) {
            ret = react(sckt, nick, channel, serverline);
        }
        
        if (ret == -1) {
            #ifdef DEBUG
            logprint("ret == -1 at end of loop()\n");
            #endif
            break;
        } else if (ret == 1) {
            #ifdef DEBUG
            logprint("ret == 1 at end of loop");
            #endif
            break;
        }
    }

    #ifdef DEBUG
    logprint("end loop()\n");
    #endif

    return ret;
}

int react (int sckt, char *nick, char *channel, char *serverline)
{
    char privmsg[MAX];
    char tmp[MAX];
    int ret = 0;

    memset(privmsg, 0, MAX);
    memset(tmp, 0, MAX);

    /*private message to the bot*/
    snprintf(privmsg, MAX, "%s", "PRIVMSG ");
    strncat(privmsg, nick, MAX-strnlen(privmsg, MAX)-1);

    if (strfind(serverline, "PING :")) {
        #ifdef DEBUG
        logprint("strfind PING\n");
        #endif
        ret = sendpong(sckt, serverline);
    } else if (strfind(serverline, privmsg)) { /*private message*/

        #ifdef DEBUG
        logprint("strfind privmsg\n");
        #endif

        if (strfind(serverline, ":!disconnect")) { /*disconnect the bot*/
            #ifdef DEBUG
            logprint("strfind !disconnect\n");
            #endif

            if (checkPass(serverline)) {
                disconnectirc(sckt); // ignore return value
                ret = 1;
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, ":!reconnect")) { /*disconnect/reconnect*/
            #ifdef DEBUG
            logprint("strfind !reconnect\n");
            #endif

            if (checkPass(serverline)) {
                disconnectirc(sckt);
                ret = -1;
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, ":!ip")) {
            #ifdef DEBUG
            logprint("strfind !ip\n");
            #endif

            if (checkPass(serverline)) {
                if (getIP(tmp, nick, sckt) == -1) {
                    errprint("getIP()\n");
                    ret = -1;
                } else {
                    ret = privatemsg(tmp, serverline, sckt);
                }
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, "!mute")) {
            #ifdef DEBUG
            logprint("strfind !mute");
            #endif

            if (checkPass(serverline)) {
                mute = 1;
                ret = privatemsg("Muted.", serverline, sckt);
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, "!unmute")) {
            #ifdef DEBUG
            logprint("strfind !unmute");
            #endif

            if (checkPass(serverline)) {
                mute = 0;
                ret = privatemsg("Unmuted.", serverline, sckt);
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, ":!os")) {
            #ifdef DEBUG
            logprint("strfind !os\n");
            #endif
            memset(tmp, 0, MAX);
            osinfo(tmp);
            ret = privatemsg(tmp, serverline, sckt);
        } else if (strfind(serverline, ":!version")) {
            #ifdef DEBUG
            logprint("strfind !version\n");
            #endif
            memset(tmp, 0, MAX);
            snprintf(tmp, MAX, "Version: %s", VERSION);
            ret = privatemsg(tmp, serverline, sckt);
        } else {
            #ifdef DEBUG
            logprint("else block\n");
            #endif
            ret = privatemsg("No!", serverline, sckt);
        }
    } 
    
    return ret;
}

int privatemsg(char *msg, char *serverline, int sckt)
{
    char send[MAX];
    char user[MAX];
    memset(send, 0, MAX);
    memset(user, 0, MAX);

    #ifdef DEBUG
    logprint("start privatemsg()\n");
    #endif

    if (!mute) {
        strncpy(user, serverline+1, (size_t)usernamecount(serverline));
        #ifdef DEBUG
        logprint("username: ");
        logprint(user);
        logprint("\n");
        #endif
        snprintf(send, MAX, "%s", "PRIVMSG ");
        strncat(send, user, MAX-strnlen(send, MAX)-1);
        strncat(send, " :", MAX-strnlen(send, MAX)-1);
        strncat(send, msg, MAX-strnlen(send, MAX)-1);
        strncat(send, "\r\n", MAX-strnlen(send, MAX)-1);

        if (writeserver(send, sckt) == -1) {
            return -1;
        }
    } else {
        #ifdef DEBUG
        logprint("bot is muted\n");
        #endif
    }

    #ifdef DEBUG
    logprint("end privatemsg()\n");
    #endif

    return 0;
}

// currently not in react
int sendcommand(char *s, int sckt)
{
    char send[MAX];
    memset(send, 0, MAX);
    
    #ifdef DEBUG
    logprint("start sendcommand()\n");
    #endif

    snprintf(send, MAX, "%s", s);
    if (getmsg(send, "!send ") == -1) {
        return -1;
    }
    if (writeserver(send, sckt) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end sendcommand()\n");
    #endif

    return 0;
}

int getIP(char *ip, char *nick, int sckt)
{
    char send[MAX];
    memset(send, 0, MAX);

    #ifdef DEBUG
    logprint("start getIP()\n");
    #endif

    snprintf(send, MAX, "%s", "WHOIS ");
    strncat(send, nick, MAX-strnlen(send, MAX)-1);
    strncat(send, "\r\n", MAX-strnlen(send, MAX)-1);

    if (writeserver(send, sckt) == -1) {
        return -1;
    }

    while (1 == 1) {
        memset(ip, 0, MAX);
        if (readserver(ip, sckt) == -1) {
            return -1;
        }
        if (strfind(ip, "is connecting from")) {
            break;
        }
    }

    #ifdef DEBUG
    logprint("end getIP()\n");
    #endif
    
    return 0;
}
