#ifndef __IRC__
#define __IRC__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "util.h"

/* connect to irc server
 * parameter server: server ip address
 * parameter port: port, usually 6667
 * returns: connected socket
 */
int connectirc(char *server, char *port);

/* close socket
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int disconnectirc(int sckt);

/* send PASS command (sets connection password, must be sent before NICK/USER registration)
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int loginpass(int sckt);

/* set the nick name
 * parameter nick: nick name
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int setnick(char *nick, int sckt);

/* send USER command (specify username and realname)
 * parameter username: the username of the bot
 * parameter realname: the realname of the bot
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int setuser(char *username, char *realname, int sckt);

/* send MODE command (set mode)
 * parameter nick: the nick of the bot
 * parameter mode: usermode (e.g. +B to mark as bot on some servers)
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int setmode(char *nick, char *mode, int sckt);

/* wait for ":End of /MOTD"
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int waitForResponse(int sckt);

/* join a channel
 * parameter channel: channel name (without the #, it will be added automatically)
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int joinchannel(char *channel, int sckt);

/* reply to PING
 * parameter sckt: connected socket
 * parameter s: the serverline to get the ping message(if any)
 * returns 0 if successful, -1 else
 */
int sendpong(int sckt, char *s);

/* read from server
 * parameter s: string where the text of the server will be saved
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int readserver(char *s, int sckt);

/* send string to server
 * parameter s: command
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int writeserver(char *s, int sckt);

/* send QUIT command, disconnect from server
 * parameter msg: quit message
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int quit(char *msg, int sckt);

#endif
