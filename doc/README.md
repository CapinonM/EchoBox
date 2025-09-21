### Command Reference

All messages must be in JSON format and include: user color, user ID, and username.

# Command Rules

Messages starting with '/' are treated as commands. All other messages are sent as normal chat.

## Server Commands

| Command         | Description                        |
|-----------------|------------------------------------|
| `/server-time`  | Show the server's local time       |
| `/server-uptime`| Show how long the server is running|
| `/my-uptime`    | Show how long you've been connected|

## Client Commands

| Command         | Description                       |
|-----------------|-----------------------------------|
| `/join`         | Connect to the chat server        |
| `/leave`        | Disconnect from the server        |
| `/ping`         | Check if the server is online     |
<!--
Command Reference
All messages need to be at format json, which should contains: user color, user id, username.

Rules for commands
Get a text with '/' at the beginning, will be treated as a command, otherwise will be a normal message.

Server commands (all start with '/')
1- time         ->    Displays the local time of the server
2- stime        ->    Displays the server uptime
3- utime        ->    Displays the user time since he connected to the server

## Client commands (all start with '/')
1- connect      ->    Connects the user to the chat server
2- disconnect   ->    Disconnects the user from the server
3- isserverup   ->    Checks if the server is up
-->