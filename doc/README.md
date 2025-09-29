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