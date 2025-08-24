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
