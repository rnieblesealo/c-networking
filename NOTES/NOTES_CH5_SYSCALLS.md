### 5.1 getaddrinfo()

#### Parameters

- Node param = hostname (DNS or IP)
- Service param = port/service name (can be straight up port number or IANA port alias, e.g. `http` for the port where HTTP requests are handled by default)
    - If we leave blank (I think!) it assumes DNS lookup port
- Hints = Pre-filled addrinfo with facts about host we already know 
- Addrinfo = Dest object to write to

#### Notes

- IANA port list and `/etc/services` list aliases for port numbers
- `gai_strerror()` turns error code spit out by `getaddrinfo()` or `getnameinfo()` into string
- We must `freeaddrinfo()` wherever we wrote results to (since there's linked list!)
- You saw `nslookup` command and `getaddrinfo()` spit different IPs for a given hostname
    - This was because you didn't handle converting to printable IP correctly
        - Make sure you have a check for `ai_family` and handle both cases (v4 and v6) accordingly
        - See `getaddrinfo.c`

### 5.2 socket()

Gets you a file descriptor to a socket for use in later syscalls; *not a connection!*

#### Parameters 

Allow you to define kind of socket

- Domain (type) = `PF_INET`/`PF_INET6`
    - Asking the type of our domain name
- Type = Socket type (Stream or datagram?)
- Protocol = Leave to 0 to auto-select based on type
    - We can alternatively specify the protocol ourselves using `getprotobyname()`, but **probably won't need to do this**
    
#### Notes

- `PF_INET` and `AF_INET` have the same value but there's a PF variant for historical reasons
    - It was thought address family might support different protocols, but no 
    - So PF = protocol, AF = address family
    - For semantics, use PF in `socket()` call and AF for anything else (`sockaddr_in`) 

### 5.3 bind()

Bind this socket to a specific port in our machine to handle connections!

Port is used to match packet to a process' socket descriptor 

> We don't need to do this if we're the client because we're sending outgoing stuff; the server should be the one caring since it needs to handle the incoming request!

#### Parameters

- SockFD = The socket descriptor
- MyAddr = The struct containing our desired port and IP to bind the socket descriptor to
- AddrLen = The size of the address object

#### Notes

- We might only really bind to a local IP address; this stuff belongs to us!
- Lots of old code might use old way, where we package address manually
- **Non-closed socket might linger, giving address already in use**
    - Fix using `setsockopt(sockfd, SOL_SOCKET, SO_REUSE_ADDR, &yes, sizeof(yes))`

### 5.4 connect()

- Connects to remote host, uses socket for communication 
> Port 80 is good to test `connect()`; it takes in HTTP connections which are sent from pretty much every client that ever tries to access it

### 5.5 listen()

#### Notes

- IP `0.0.0.0` is a wildcard for all our host machine's network interfaces; if this is the IP we are listening on, we are listening on all our available network interfaces
- This isn't blocking! It basically only sets up listening; but if we're looking to `accept()` then *that* will require blocking

### 5.6 accept()

This will return a new socket file descriptor responsible for the accepted connection; the original socket descriptor is still listening for new connections  

Pretty simple

#### Notes

- For some reason, if we have an `accept()`, any prints must have a newline at the end or else they'll be held back
> I have no fucking idea why this happens what the fuck
- Most systems typically cap the backlog size to about 20 by default

### 5.7 send() and recv()

#### Notes

- `send()` payload caps out at about 1K, be careful with this!
- If `recv()` ever returns 0 it *probably* means something's gone wrong!

### 5.8 sendto() and recvfrom()

DATAGRAM style sending and received (for unconnected datagram sockets)

> **Note:** Deferred for now; will get into these later

### 5.9 close() and shutdown()

- `close()` nukes the FD so neither side can use it anymore
- `shutdown()` gives control; can close one end only or both, like `close()`
    - We specify this via `how` param
    - `0`: Disable receiving via this FD 
    - `1`: Disable sending via this FD
    - `2`: Disable both; same as `close()`
    - **Warning:** This function, regardless of `how`, does NOT close the file descriptor; we must use `close()` for this strictly

### 5.10 getpeername()

Tells us who's at the other end of a TCP socket
> If we established the connection in the first place, shouldn't we already know this? Re: It might be useful when we want the IP as opposed to the DNS name, for example
