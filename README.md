IRCbot
======

Simple IRC Bot (not fully functional)

Start the bot with:
  ```bash
  $ ./bot <server> <port> <nick> <channel> [mute]
  ```
where 
* server: address of the IRC server, e.g. irc.foonetic.net
* port: usually 6667
* nick: the name of the bot
* channel: the name of the channel, e.g. \#yourtestchannel (remember to put a \ before the #)
* mute: set this to non-zero to mute the bot (optional)

Additionally, you'll need the following textfiles:
* messages.txt: general messages
* personal.txt: messages the bot will use when addressed personally
* passphrase: contains the passphrase (for at least a bit of security)

To shutdown the bot, use `/msg <nick> !shutdown <passphrase>`     
To reconnect the bot, use `/msg <nick> !reconnect <passphrase>` (note: Bot waits 2min before reconnecting (main.c))

To enable debug mode, use `make debug`
