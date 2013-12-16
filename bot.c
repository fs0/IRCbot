#include "bot.h"

/*TODO regular expressions?*/
/*TODO correct error handling*/
/*TODO clean up string/message functions*/

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

        if ( (ret = readserver(serverline, sckt, 1)) != -1) {
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
    char tome[MAX];
    int ret = 0;

    memset(privmsg, 0, MAX);
    memset(tmp, 0, MAX);
    memset(tome, 0, MAX);

    /*private message to the bot*/
    snprintf(privmsg, MAX, "%s", "PRIVMSG ");
    strncat(privmsg, nick, MAX-strnlen(privmsg, MAX)-1);

    /*message to the bot*/
    snprintf(tome, MAX, "%s", "PRIVMSG ");
    strncat(tome, channel, MAX-strnlen(tome, MAX)-1);
    strncat(tome, " :", MAX-strnlen(tome, MAX)-1);
    strncat(tome, nick, MAX-strnlen(tome, MAX)-1);

    if (strfind(serverline, "PING :") == 0) {
        #ifdef DEBUG
        logprint("strfind PING\n");
        #endif
        ret = sendpong(sckt, serverline);
    } else if (strfind(serverline, ":!info") == 0) {
        #ifdef DEBUG
        logprint("strfind !info\n");
        #endif
        ret = channelmsg("I'm a cybernetic organism. Living tissue over a metal endoskeleton.", channel, sckt);
    } else if (strfind(serverline, tome) == 0) {
        #ifdef DEBUG
        logprint("strfind tome\n");
        #endif
        answer(sckt, serverline, channel, nick, tmp); // screw return
        ret = channelmsg(tmp, channel, sckt);
    } else if (strfind(serverline, privmsg) == 0) { /*private message*/

        #ifdef DEBUG
        logprint("strfind privmsg\n");
        #endif

        if (strfind(serverline, ":!shutdown") == 0) { /*disconnect the bot*/
            #ifdef DEBUG
            logprint("strfind !shutdown\n");
            #endif

            if (checkPass(serverline) == 0) {
                disconnectirc(sckt); // ignore return value
                ret = 1;
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, ":!reconnect") == 0) { /*disconnect/reconnect*/
            #ifdef DEBUG
            logprint("strfind !reconnect\n");
            #endif

            if (checkPass(serverline) == 0) {
                disconnectirc(sckt);
                ret = -1;
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, ":!ip") == 0) {
            #ifdef DEBUG
            logprint("strfind !ip\n");
            #endif

            if (checkPass(serverline) == 0) {
                if (getIP(tmp, nick, sckt) == -1) {
                    errprint("getIP()\n");
                    ret = -1;
                } else {
                    ret = privatemsg(tmp, serverline, sckt);
                }
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, "!mute") == 0) {
            #ifdef DEBUG
            logprint("strfind !mute");
            #endif

            if (checkPass(serverline) == 0) {
                mute = 1;
                ret = privatemsg("Muted.", serverline, sckt);
            } else {
                ret = privatemsg("You mute!", serverline, sckt);
            }
        } else if (strfind(serverline, "!unmute") == 0) {
            #ifdef DEBUG
            logprint("strfind !unmute");
            #endif

            if (checkPass(serverline) == 0) {
                mute = 0;
                ret = privatemsg("Unmuted.", serverline, sckt);
            } else {
                ret = privatemsg("No!", serverline, sckt);
            }
        } else if (strfind(serverline, ":!os") == 0) {
            #ifdef DEBUG
            logprint("strfind !os\n");
            #endif
            memset(tmp, 0, MAX);
            osinfo(tmp);
            ret = privatemsg(tmp, serverline, sckt);
        } else if (strfind(serverline, ":!version") == 0) {
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
            answer(sckt, serverline, channel, nick, tmp);
            ret = privatemsg(tmp, serverline, sckt);
        }
    } else if ((getrand(20) == 0) && (strend(serverline, "?") == -1)) {
        #ifdef DEBUG
        logprint("rand == 0 && strend not ?\n");
        #endif
        memset(tmp, 0, MAX);
        if (getLine(tmp, "./messages.txt") == -1) {
            errprint("getmsg()\n");
            ret = -1;
        } else {
            ret = channelmsg(tmp, channel, sckt);
        }
    }
    
    return ret;
}

// TODO errorhandling?
int answer(int sckt, char *serverline, char *channel, char* nick, char *response)
{
    char textfileline[MAX];
    int ret = 0;

    #ifdef DEBUG
    logprint("start answer()\n");
    #endif

    memset(response, 0, MAX);
    memset(textfileline, 0, MAX);

    if (strfind(serverline, ": hello") == 0 || strfind(serverline, ": Hello") == 0 || strfind(serverline, ": hi") == 0 || strfind(serverline, ": Hi") == 0) {
        if (usernamecount(serverline) == -1) {
            // couldn't find username, continue
            return 0;
        }
        strncpy(response, serverline+1, (size_t)usernamecount(serverline));
        strncat(response, ": ", MAX-strnlen(response, MAX)-1);
        strncat(response, "Hello.", MAX-strnlen(response, MAX)-1);
    } else if (strend(serverline, "?") == 0 && yesnoq(serverline, nick) == 0) { /*yes no question?*/ 
        if (usernamecount(serverline) == -1) {
            // couldn't find username, continue
            return 0;
        }
        strncpy(response, serverline+1, (size_t)usernamecount(serverline));
        strncat(response, ": ", MAX-strnlen(response, MAX)-1);
        switch(getrand(3)) {
            case 0:     strncat(response, "Yes.", MAX-strnlen(response, MAX)-1); break;
            case 1:     strncat(response, "No.", MAX-strnlen(response, MAX)-1); break;
            case 2:     strncat(response, "Of course. I'm a terminator.", MAX-strnlen(response, MAX)-1); break;
            default:    break;
        }
    } else {
        if (usernamecount(serverline) == -1) {
            // couldn't find username, continue
            return 0;
        }
        strncpy(response, serverline+1, (size_t)usernamecount(serverline));
        strncat(response, ": ", MAX-strnlen(response, MAX)-1);
        if (getLine(textfileline, "./personal.txt") == -1) {
            errprint("getmsg()\n");
        } else {
            strncat(response, textfileline, MAX-strnlen(response, MAX)-1);
        }
    }

    #ifdef DEBUG
    logprint("end answer()\n");
    #endif

    return 0;
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

    strncpy(user, serverline+1, (size_t)usernamecount(serverline));
    logprint("username: ");
    logprint(user);
    logprint("\n");
    snprintf(send, MAX, "%s", "PRIVMSG ");
    strncat(send, user, MAX-strnlen(send, MAX)-1);
    strncat(send, " :", MAX-strnlen(send, MAX)-1);
    strncat(send, msg, MAX-strnlen(send, MAX)-1);
    strncat(send, "\r\n", MAX-strnlen(send, MAX)-1);

    if (writeserver(send, sckt, 1) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end privatemsg()\n");
    #endif

    return 0;
}

int channelmsg(char *msg, char *channel, int sckt)
{
    char send[MAX];
    memset(send, 0, MAX);

    #ifdef DEBUG
    logprint("start privatemsg()\n");
    #endif

    if (!mute) {
        #ifdef DEBUG
        logprint("not muted\n");
        #endif
        snprintf(send, MAX, "%s", "PRIVMSG ");
        strncat(send, channel, MAX-strnlen(send, MAX)-1);
        strncat(send, " :", MAX-strnlen(send, MAX)-1);
        strncat(send, msg, MAX-strnlen(send, MAX)-1);
        strncat(send, "\r\n", MAX-strnlen(send, MAX)-1);

        if (writeserver(send, sckt, 1) == -1) {
            return -1;
        }
    } else {
        #ifdef DEBUG
        logprint("muted\n");
        #endif
    }

    #ifdef DEBUG
    logprint("end privatemsg()\n");
    #endif

    return 0;
}

// currently not in loop
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
    if (writeserver(send, sckt, 1) == -1) {
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

    if (writeserver(send, sckt, 1) == -1) {
        return -1;
    }

    while (1 == 1) {
        memset(ip, 0, MAX);
        if (readserver(ip, sckt, 1) == -1) {
            return -1;
        }
        if (strfind(ip, "is connecting from") == 0) {
            break;
        }
    }

    #ifdef DEBUG
    logprint("end getIP()\n");
    #endif
    
    return 0;
}
