IRCbot
======

Simple IRC Bot

Start the bot with:
  ```bash
  $ ./bot <server> <port> <nick> <realname> <channel> <mute> <log>
  ```
where 
* server: address of the IRC server, e.g. irc.foonetic.net
* port: usually 6667
* nick: the name of the bot
* realname: preferably your own nick (in case of problems with the bot)
* channel: the name of the channel, e.g. \#yourtestchannel (remember to put a \ before the #)
* mute: set this to non-zero to mute the bot
* log: set this to non-zero to log all messages

Additionally, you'll need the following textfile:
* passphrase: contains the passphrase (for at least a bit of security)

Controlling the bot:
* To disconnect the bot, use `/msg <nick> !disconnect <passphrase>`
* To reconnect the bot, use `/msg <nick> !reconnect <passphrase>` (note: Bot waits 2min before reconnecting (main.c))
* To turn on logging,  use `/msg <nick> !log <passphrase>`
* To turn off logging,  use `/msg <nick> !dontlog <passphrase>`
* To mute the bot, use `/msg <nick> !mute <passphrase>`
* To unmute the bot, use `/msg <nick> !unmute <passphrase>`
* To get the "connecting from" line, use `/msg <nick> !ip <passphrase>`
* To get OS information, use `/msg <nick> !os`
* To get the version, use `/msg <nick> !version`

To enable debug mode, use `make debug`
