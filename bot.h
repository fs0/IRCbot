#ifndef __BOT__
#define __BOT__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "irc.h"
#include "util.h"

#define VERSION "1.1.2"

int mute;

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

/* scan serverline
 * parameter sckt: connected socket
 * parameter nick: nick used by the bot
 * parameter channel: current channel
 * parameter serverline: received string
 * returns: 0 if successful, 1 to break out of loop(), -1 else
 */
int react (int sckt, char *nick, char *channel, char *serverline);

/* get reply and store it in response
 * parameter sckt: connected socket
 * parameter serverline: received string
 * parameter dest: current channel
 * parameter nick: nick used by the bot
 * parameter response: string where response will be saved
 * returns: 0 (always...)
 */
int answer(int sckt, char *serverline, char *channel, char *nick, char *response);

/* send a message
 * parameter msg: message to send
 * parameter serverline: received string
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int privatemsg(char *msg, char *serverline, int sckt);

/* send a message
 * parameter msg: message to send
 * parameter channel: channelname
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int channelmsg(char *msg, char *channel, int sckt);

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
