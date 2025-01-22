# Simple Client-Server Communication in C

This repository contains a basic implementation of a client-server communication system in C using sockets. The client connects to the server, sends messages, and receives responses. It demonstrates fundamental socket programming techniques.

## Features
- Client-server communication over TCP.
- Configurable port and server IP.
- Multi-client support via forking on the server side.
- Graceful handling of connection termination.

## Files in the Repository
- **client.c**: Source code for the client application.
- **server.c**: Source code for the server application.
- **Makefile**: Build script for compiling the client and server programs.
