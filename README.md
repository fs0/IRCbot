IRCbot
======

Simple IRC Bot (not fully functional)

Start the bot with:
  ```bash
  $ ./bot <server> <port> <nick> <channel>
  ```
where 
* server: address of the IRC server, e.g. irc.foonetic.net
* port: usually 6667
* nick: the name of the bot
* channel: the name of the channel, e.g. \#yourtestchannel (remember to put a \ before the #)

Additionally, you'll need two textfiles:
* messages.txt: general messages
* personal.txt: messages the bot will use when addressed personally
* passphrase: contains the passphrase

To shutdown the bot, use `/msg <nick> !shutdown`

To enable debug mode, use `make debug`
