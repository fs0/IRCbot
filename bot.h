#ifndef __BOT__
#define __BOT__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* max length of message */
#define MAX 1024

/* initialization
 * parameter sckt: socket
 * parameter nick: nick used by the bot
 * parameter username: username of the bot
 * parameter realname: realname of the bot
 * parameter channel: channel to connect to
 * returns: 0 if successful, -1 else
 */
int init(int sckt, char *nick, char *username, char *realname, char *channel);

/* wait for ":End of /MOTD"
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int waitForResponse(int sckt);

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

/* send a message
 * parameter msg: message to send
 * parameter dest: destination, e.g. channelname or username
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int privatemsg(char *msg, char *dest, int sckt);

/* send USER command (specify username and realname)
 * parameter username: the username of the bot
 * parameter realname: the realname of the bot
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int setuser(char *username, char *realname, int sckt);

/* join a channel
 * parameter channel: channel name (without the #, it will be added automatically)
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int joinchannel(char *channel, int sckt);

/* set the nick name
 * parameter nick: nick name
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int setnick(char *nick, int sckt);

/* send commandline to server 
 * parameter s: serverline
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int sendcommand(char *s, int sckt);

/* send QUIT command, disconnect from server
 * parameter msg: quit message
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int quit(char *msg, int sckt);

/* reply to PING
 * parameter sckt: connected socket
 * parameter s: the serverline to get the ping message(if any)
 * returns 0 if successful, -1 else
 */
int sendpong(int sckt, char *s);

/* can the message be answered with yes/no?
 * parameter s: line reveived from server
 * parameter nick: nickname
 * returns: 1 if the message can be answered with yes or no, 0 if not
 */
int yesnoq(char *s, char *nick);

/* read from server
 * parameter s: string where the text of the server will be saved
 * parameter sckt: connected socket
 * parameter logging: only write to log if logging==1
 * returns: 0 if successful, -1 else
 */
int readserver(char *s, int sckt, int logging);

/* send command to server
 * parameter s: command
 * parameter sckt: connected socket
 * parameter logging: only write to log if logging==1
 * returns: 0 if successful, -1 else
 */
int writeserver(char *s, int sckt, int logging);

/* get operating system info
 * parameter info: info string
 * returns: 0 if successful, -1 else
 */
int osinfo(char *info);

/* get IP of the bot
 * parameter ip: string where the ip will be saved
 * parameter nick: nick of bot
 * parameter sckt: connected socket
 * returns: 0 if successful, -1 else
 */
int getIP(char *ip, char *nick, int sckt);

/* find s2 in s1
 * parameter s1: string to be searched in
 * parameter s2: string to be searched after
 * returns: 1 if s2 is in s1, 0 if s2 is not in s1
 */
int strfind(char *s1, char *s2);

/* counts the characters of the username that sent a private message
 * parameter s: string with username
 * returns: number of characters of the username, if error: -1
 */
int usernamecount(char *s);

/* does s0 end with s1?
 * parameter s0: string to be searched in
 * parameter s1: string to be searched after
 * returns: 1 if s0 ends with s1, 0 if not
 */
int strend(char *s0, char *s1);

/* filters the message behind the keyword from s and saves it to s
 * parameter s: serverline
 * parameter key: the keyword
 * returns: 0 if successful, -1 else
 */
int getmsg(char *s, char *key);

/* prints error message to log
 * parameter s: error string
 */
void errprint(char *s);

/* print to logfile
 * parameter s: string to print to log file
 */
void logprint(char *s);

/* get a random number between 0 and a (excluding)
 * parameter a: limit 
 * returns: random number
 */
int getrand(int a);

/* read line from fd
 * parameter fd: descriptor
 * parameter vptr: buffer
 * parameter maxlen: max length to read
 * returns: characters read
 */
ssize_t readline(int fd, void *vptr, size_t maxlen);

/* get a random line from a text file
 * parameter line: pointer to char array to store line
 * returns 0 if successful, -1 else
 */
int getLine(char *line, char *textfile);

#endif
