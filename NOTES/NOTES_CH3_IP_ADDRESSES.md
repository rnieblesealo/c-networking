## 3.1 IPv4 and 6

- IPv4 = 32-bit address space; 2^32 available addresses
    - But there is a bunch of shit that needs an IP and that's not enough
    - Fuck parking meters 
    - Four 1-byte chunks
        - e.g. `192.168.10.33` has first byte with decimal value `192` 
        - (4 bytes) * (8 bits per byte) = 32 bits! 
- IPv6 = 128-bit address space; 2^128 addresses, that'll do
    - Eight 2-byte chunks
        - (2 bytes) * (8 bits per byte) * (8 chunks) = 128 bits!
- Side note on hex:
    - Binary base is 2 
        - That means we need log 2 (2) = 1 bits to represent a single binary digit
    - Hex base is 16 
        - That means we need log 2 (16) = 4 bits to represent a hex digit
    - Each byte has 8 bits, meaning 2 pairs of 4 bits 
    - As such, **each byte is 2 hex digits!**

### 3.1.1 Subnets

- IP split between network/host part; we can extract each using netmask; 
- The netmask gives you the network addr. WITHOUT HOST
    - e.g. If `255.255.255.0` is netmask and original IPv4 is `192.168.10.33` host addr. is `192.168.10.33 & 255.255.255.0 = 192.168.10.0`
    - The host is the remainder; here, host would be `0.0.0.12` 
- We specify with `/` the bit at which network and host are separated
    - For above example, last byte (8 bits) are host, so we'd denote as `192.168.10.33/24` since bit `24` is where host starts
    - Same idea for v6 

### 3.1.2 Port Numbers

- Specific location on host at which specific connection is handled 
    - Does each port have a socket?
        - This seems messy so TCP's safety forbids it
        - But UDP = fuck it! This does let you do that
        - Still, it's important to distinguish ports from sockets; the port can simply be considered a more specific part of the address

- System usually has OS-reserved ports up until 1024

### 3.2 Endianness

- Big endian: MSB leftmost, network-byte order too!
- Little endian: MSB rightmost 
- Host-byte order can be either depending on computer but we always want to convert into big endian assuming host byte order is wrong one
- Use `hton, etc.` function family to do these conversions
    - Long and short just distinguish bitness; if we're working with 64-bit integers vs. 32
    - Either way, IPv4 is 4 bytes and IPv6 is 16 bytes; they'll fit into either one!

### 3.3 C Implementation/Structs

- `struct addrinfo`: Socket address structure, stores protocol, socket type, actual address as `struct sockaddr`, etc. 
- `struct sockaddr`: Stores all address data in bytes (including port and whatnot) and address family (Is it INET4, INET6...?) 
    - This is fucking shit since doing this by hand sucks, so there is...
    - `struct sockaddr_in/sockaddr_in6`: The above but handier for IPv4 or v6 respectively! 
        - `struct in_addr/in6_addr`: Simply stores the IP itself, NOT all network data in bytes; this is literally the `192.168.10.33` thing
- `struct sockaddr_storage`: Generic store for address data; we typically want to cast to this when passing address around APIs 
- `getaddrinfo()`: Resolves a hostname into a linked list of addrinfo's which are all its possible representations

Don't forget to convert to network-byte order!

### 3.4 C Implementation/Structs II

- `inet_pton()`: Converts an address string (e.g. `"192.168.10.33"` into a network byte order value)
    - Writes everything to `sockaddr_in6.sin_addr` we pass via address
- `inet_ntop()`: does the opposite as above :)

### 3.4.1 Private Networks

- NAT translates our static IP to an ISP-level one 
    - That means our entire local network is associated to one IP
    - That local network has its own IP ecosystem! We have 2^32-1 possible addresses
    - This is done by the firewall to save on addresses
    - IPv6 has so many addresses possible that NAT should no longer be necessary 
