IRCbot
======

Simple IRC Bot

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

Additionally, you'll need the following textfile:
* passphrase: contains the passphrase (for at least a bit of security)

Controlling the bot:
* To disconnect the bot, use `/msg <nick> !disconnect <passphrase>`
* To reconnect the bot, use `/msg <nick> !reconnect <passphrase>` (note: Bot waits 2min before reconnecting (main.c))
* To mute the bot, use `/msg <nick> !mute <passphrase>`
* To unmute the bot, use `/msg <nick> !unmute <passphrase>`
* To get the "connecting from" line, use `\msg <nick> !ip <passphrase>`
* To get OS information, use `/msg <nick> !os`
* To get the version, use `/msg <nick> !version`

To enable debug mode, use `make debug`
