# libsync-server

The context when I wrote this project : [libfrp](https://zhaochengche.me/blog-tech/libfrp/)

## Demo

[demo video](https://github.com/libsync/demo.mov)

## Features

## APIs

Control: the get connection sends request to this server, obtain response
Update: the receive connection receives updates from this server

## TODO & BUGS

Remove the update fds and control fds after the connection is closed or the chid is reset.