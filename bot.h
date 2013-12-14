#ifndef __BOT__
#define __BOT__

#include <stdio.h>
#include <string.h>
#include "irc.h"
#include "util.h"

/* initialization
 * parameter sckt: socket
 * parameter nick: nick used by the bot
 * parameter username: username of the bot
 * parameter realname: realname of the bot
 * parameter channel: channel to connect to
 * returns: 0 if successful, -1 else
 */
int init(int sckt, char *nick, char *username, char *realname, char *channel);

/* main loop
 * parameter sckt: connected socket
 * parameter nick: nick used by the bot
 * parameter channel: current channel
 * returns: 0 if successful, -1 else
 */
int loop(int sckt, char *nick, char *channel);

/* reply to message
 * parameter sckt: connected socket
 * parameter serverline: received string
 * parameter channel: current channel
 * parameter nick: nick used by the bot
 * returns: 0 if successful (even if no username found), -1 else
 */
int answer(int sckt, char *serverline, char *channel, char *nick);

/* send a message
 * parameter msg: message to send
 * parameter dest: destination, e.g. channelname or username
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int privatemsg(char *msg, char *dest, int sckt);

/* send IRC-command to server 
 * parameter s: serverline
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int sendcommand(char *s, int sckt);

/* get IP of the bot from server
 * parameter ip: string where the ip will be saved
 * parameter nick: nick of bot
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int getIP(char *ip, char *nick, int sckt);

#endif
