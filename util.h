#ifndef __UTIL__
#define __UTIL__

#include <sys/utsname.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* max length of message */
#define MAX 1024
#define LOG "./bot.log"
#define MSGLOG "./msg.log"

/* print to logfile
 * parameter s: string to print to logfile
 */
void logprint(char *s);

/* prints error message to log
 * parameter s: error string
 */
void errprint(char *s);

/* print messages to message-logfile
 * parameter s: string to print to logfile
 */
void msglogprint(char *s);

/* find s2 in s1
 * parameter s1: string to be searched in
 * parameter s2: string to be searched after
 * returns: 1 if s2 is in s1, 0 if s2 is not in s1
 */
int strfind(char *s1, char *s2);

/* filters the message behind the keyword from s and saves it to s
 * parameter s: serverline
 * parameter key: the keyword
 * returns: 0 if successful, -1 else
 */
int getmsg(char *s, char *key);

/* counts the characters of the username that sent a private message
 * parameter s: string with username
 * returns: number of characters of the username, if error: -1
 */
int usernamecount(char *s);

/* check if correct passphrase in serverline
 * parameter serverline: string received
 * returns: 1 if passphrase correct, 0 else
 */
int checkPass(char *serverline);

/* get operating system info
 * parameter info: info string
 * returns: 0 if successful, -1 else
 */
int osinfo(char *info);

/* get a random number between 0 and a [0,a)
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

#endif
