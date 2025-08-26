### 6.1 Client Server Intro

- Multiple incoming connections are typically handled by separate threads
    - `fork()`!

#### Big Program Syscalls

- `waitpid()`: Suspend execution of calling process until signal received or child process reports back after terminating via `stat_loc`.
    - If we pass `-1` PID, it waits for any child process to finish; otherwise, it waits for the PID-specific one.
    - `NULL` as second arg isn't really relevant in this case
    - `WNOHANG` = don't wait if no processes want to give status report 
> Essentially, it waits for a child process to finish to resume parent process execution; i.e., ***wait for PID!***
