### Messages Especifications
All messages need to be at format json, which should contains: user color, user id, username.

# Rules for commands
Get a text with '/' or '/a' at the beginning, will be treated as a command, otherwise will be a normal message.

## Server commands (all start with '/')
1- time         -->    Displays the local time of the server
2- stime        -->    Displays the server uptime
3- utime        -->    Displays the user time since he connected to the server

## Client commands (all start only with '/a')
1- connect      -->    Connects the user to the chat server
2- disconnect   -->    Disconnects the user from the server
3- isup         -->    Checks if the server is up