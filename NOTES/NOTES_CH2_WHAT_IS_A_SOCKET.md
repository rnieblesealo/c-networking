## 2.1 Sockets

- Socket = Endpoint where connections can read/write data to/from
    - Has a file descriptor associated to it because Unix!
- Diff types 
    - `SOCK_STREAM` = Socket with TCP, reliable, slower
    - `SOCK_DGRAM` = Socket with UDP, less reliable, WAY faster
        - UDP usually has other protocol atop it to do some kind of safety work, e.g. `tftp`
 
## 2.2 Packet Layering

- Onion layering atop data
    - Each layer corresponds to a protocol; the protocol encapsulates what's below it such that it can work with it
