# libsync-server

The context when I wrote this project : [libfrp](https://zhaochengche.me/blog-tech/libfrp/)

## Demo

[demo video](http://zhaochengche.me/share/demo.mp4)

## Features

## APIs

### Control

Control: the get connection sends request to this server, obtain response

```
chnlcreate c1  # create a channel named c1
chnl c1        # attach to channel c1
hget k1 f1     # get the value at key=k1, field=f1
hset k1 f1 v1  # set the value at key=k1, field=f1 to be v1
quit           # close connection
```

### Update

Update: the receive connection receives updates from this server

```
chnl c1        # attach to channel c1
quit           # close connection
```

## TODO & BUGS

Reset to default channel with `chnl`.
Send the delayed messages when update_fd is first hooked.