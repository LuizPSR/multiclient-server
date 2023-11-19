# MultiClient Server
Project using sockets to implement a server that can handle multiple clients using C socket libraries.

The server represent a blog where the clients may read and write topics. These topics behave similar to a chatroom, as the server doesn't need to save the messages, only direct the message written in a topic to all subscribers of that topic that are currently connected to the system.

There is no login or identification of the client, and clients and no information from the client is kept after the disconnection.

## Commands
- **publish [in/on/to] *topic***: publish a post on a existent topic.
- **list**: list all available topics.
- **subscribe [to/in/on] *topic***: subscribe in a topic. If topic not does not exist, create it.
- **exit**: end connection to server.
- **unsubscribe [to/in/to/from] *topic***: unsubscribe to a topic.

## Usage
./bin/server [v4/v6] *port*

./bin/client *IP adress* *port*
