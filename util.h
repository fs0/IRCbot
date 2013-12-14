#ifndef __UTIL__
#define __UTIL__

#include <sys/utsname.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

/* max length of message */
#define MAX 1024

/* print to logfile
 * parameter s: string to print to log file
 */
void logprint(char *s);

/* prints error message to log
 * parameter s: error string
 */
void errprint(char *s);

/* find s2 in s1
 * parameter s1: string to be searched in
 * parameter s2: string to be searched after
 * returns: 0 if s2 is in s1, -1 if s2 is not in s1
 */
int strfind(char *s1, char *s2);

/* does s0 end with s1?
 * parameter s0: string to be searched in
 * parameter s1: string to be searched after
 * returns: 0 if s0 ends with s1, -1 if not
 */
int strend(char *s0, char *s1);

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
 * returns: 0 if passphrase correct, -1 else
 */
int checkPass(char *serverline);

/* can the message be answered with yes/no?
 * parameter s: line reveived from server
 * parameter nick: nickname
 * returns: 0 if the message can be answered with yes or no, -1 if not
 */
int yesnoq(char *s, char *nick);

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

/* get a random line from a text file
 * parameter line: pointer to char array to store line
 * parameter textfile: name of file
 * returns 0 if successful, -1 else
 */
int getLine(char *line, char *textfile);

#endif
