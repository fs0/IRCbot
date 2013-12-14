#include "bot.h"

/*TODO regular expressions?*/
/*TODO correct error handling*/
/*TODO clean up string/message functions*/

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
    char privmsg[MAX];
    char tmp[MAX];
    char tmp2[MAX];
    char tome[MAX];
    int ret = 0;

    #ifdef DEBUG
    logprint("start loop()\n");
    #endif

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

    while (1==1) {

        memset(serverline, 0, MAX);

        if (readserver(serverline, sckt, 1) == -1) {
            //return -1;
            ret = -1;
        }
        
        if (strfind(serverline, "PING :") == 0) {
            #ifdef DEBUG
            logprint("strfind PING\n");
            #endif
            ret = sendpong(sckt, serverline);
        } else if (strfind(serverline, ":!os") == 0) {
            #ifdef DEBUG
            logprint("strfind !os\n");
            #endif
            memset(tmp, 0, MAX);
            osinfo(tmp);
            ret = privatemsg(tmp, channel, sckt);
        } else if (strfind(serverline, ":!info") == 0) {
            #ifdef DEBUG
            logprint("strfind !info\n");
            #endif
            ret = privatemsg("I'm a cybernetic organism. Living tissue over a metal endoskeleton.", channel, sckt);
        } else if (strfind(serverline, tome) == 0) {
            #ifdef DEBUG
            logprint("strfind tome\n");
            #endif
            ret = answer(sckt, serverline, channel, nick);
        } else if (strfind(serverline, ":!version") == 0) {
            #ifdef DEBUG
            logprint("strfind tome\n");
            #endif
            memset(tmp, 0, MAX);
            snprintf(tmp, MAX, "Version: %s", VERSION);
            ret = privatemsg(tmp, channel, sckt);
        } else if (strfind(serverline, privmsg) == 0) { /*private message*/

            #ifdef DEBUG
            logprint("strfind privmsg\n");
            #endif

            memset(tmp, 0, MAX);
            memset(tmp2, 0, MAX);

            if (strfind(serverline, ":!shutdown") == 0) { /*disconnect the bot*/
                #ifdef DEBUG
                logprint("strfind !shutdown\n");
                #endif

                if (checkPass(serverline) == 0) {
                    disconnectirc(sckt); // ignore return value
                    ret = 0;
                    break;
                } else if (usernamecount(serverline) != -1) {
                    strncpy(tmp2, serverline+1, (size_t)usernamecount(serverline));
                    ret = privatemsg("No!", tmp2, sckt);
                }
            } else if (strfind(serverline, ":!reconnect") == 0) { /*disconnect/reconnect*/
                #ifdef DEBUG
                logprint("strfind !reconnect\n");
                #endif

                if (checkPass(serverline) == 0) {
                    disconnectirc(sckt);
                    ret = -1;
                    break;
                } else if (usernamecount(serverline) != -1) {
                    strncpy(tmp2, serverline+1, (size_t)usernamecount(serverline));
                    ret = privatemsg("No!", tmp2, sckt);
                }
            } else if (strfind(serverline, ":!ip") == 0) {
                #ifdef DEBUG
                logprint("strfind !ip\n");
                #endif

                if (checkPass(serverline) == 0) {
                    if (getIP(tmp, nick, sckt) == -1) {
                        errprint("getIP()\n");
                        ret = -1;
                    } else if (usernamecount(serverline) != -1) { // TODO usernamecount called twice
                        strncpy(tmp2, serverline+1, (size_t)usernamecount(serverline));
                        // write the connection info string tmp to the user tmp2
                        ret = privatemsg(tmp, tmp2, sckt);
                    }
                } else if (usernamecount(serverline) != -1) {
                    strncpy(tmp2, serverline+1, (size_t)usernamecount(serverline));
                    ret = privatemsg("No!", tmp2, sckt);
                }
            } else {
                #ifdef DEBUG
                logprint("else block\n");
                #endif
                
                if (usernamecount(serverline) == -1) {
                    continue;
                }
                strncpy(tmp, serverline+1, usernamecount(serverline));
                ret = privatemsg("No!", tmp, sckt);
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
                ret = privatemsg(tmp, channel, sckt);
            }
        }
        
        if (ret == -1) {
            #ifdef DEBUG
            logprint("ret == -1 at end of loop()\n");
            #endif
            break;
        }
    }

    #ifdef DEBUG
    logprint("end loop()\n");
    #endif

    return ret;
}

int answer(int sckt, char *serverline, char *channel, char* nick)
{
    char tmp[MAX];
    char textfileline[MAX];
    int ret = 0;

    #ifdef DEBUG
    logprint("start answer()\n");
    #endif

    if (strfind(serverline, ": hello") == 0 || strfind(serverline, ": Hello") == 0 || strfind(serverline, ": hi") == 0 || strfind(serverline, ": Hi") == 0) {
        memset(tmp, 0, MAX);
        if (usernamecount(serverline) == -1) {
            // couldn't find username, continue
            return 0;
        }
        strncpy(tmp, serverline+1, (size_t)usernamecount(serverline));
        strncat(tmp, ": ", MAX-strnlen(tmp, MAX)-1);
        strncat(tmp, "Hello.", MAX-strnlen(tmp, MAX)-1);
        ret = privatemsg(tmp , channel, sckt);
    } else if (strend(serverline, "?") == 0 && yesnoq(serverline, nick) == 0) { /*yes no question?*/ 
        memset(tmp, 0, MAX);
        if (usernamecount(serverline) == -1) {
            // couldn't find username, continue
            return 0;
        }
        strncpy(tmp, serverline+1, (size_t)usernamecount(serverline));
        strncat(tmp, ": ", MAX-strnlen(tmp, MAX)-1);
        switch(getrand(3)) {
            case 0:    strncat(tmp, "Yes.", MAX-strnlen(tmp, MAX)-1); break;
            case 1:    strncat(tmp, "No.", MAX-strnlen(tmp, MAX)-1); break;
            case 2:    strncat(tmp, "Of course. I'm a terminator. ", MAX-strnlen(tmp, MAX)-1); break;
            default: break;
        }
        ret = privatemsg(tmp , channel, sckt);
    } else {
        memset(tmp, 0, MAX);
        memset(textfileline, 0, MAX);

        if (usernamecount(serverline) == -1) {
            // couldn't find username, continue
            return 0;
        }
        strncpy(tmp, serverline+1, (size_t)usernamecount(serverline));
        strncat(tmp, ": ", MAX-strnlen(tmp, MAX)-1);

        if (getLine(textfileline, "./personal.txt") == -1) {
            errprint("getmsg()\n");
        } else {
            strncat(tmp, textfileline, MAX-strnlen(tmp, MAX)-1);
            ret = privatemsg(tmp , channel, sckt);
        }
    }

    #ifdef DEBUG
    logprint("end answer()\n");
    #endif

    return ret;
}

int privatemsg(char *msg, char *dest, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start privatemsg()\n");
    #endif

    /*strcpy(tmp, "PRIVMSG ");*/
    snprintf(tmp, MAX, "%s", "PRIVMSG ");
    strncat(tmp, dest, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, " :", MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, msg, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt, 1) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end privatemsg()\n");
    #endif

    return 0;
}

// currently not in loop
int sendcommand(char *s, int sckt)
{
    char tmp[MAX];
    
    #ifdef DEBUG
    logprint("start sendcommand()\n");
    #endif

    /*strcpy(tmp, s);*/
    snprintf(tmp, MAX, "%s", s);
    if (getmsg(tmp, "!send ") == -1) {
        return -1;
    }
    if (writeserver(tmp, sckt, 1) == -1) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end sendcommand()\n");
    #endif

    return 0;
}

int getIP(char *ip, char *nick, int sckt)
{
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start getIP()\n");
    #endif

    memset(tmp, 0, MAX);

    snprintf(tmp, MAX, "%s", "WHOIS ");
    strncat(tmp, nick, MAX-strnlen(tmp, MAX)-1);
    strncat(tmp, "\r\n", MAX-strnlen(tmp, MAX)-1);

    if (writeserver(tmp, sckt, 1) == -1) {
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
