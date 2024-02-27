# Items that still need to be done
* Refactor Client to use multithreading instead of creating a new process
* Refactor Service to use multithreading instead of creating a new process
    * Send process stdout to connected clients
    * Allow multiple connections to socket using multiple threads for each socket
    * Run the socket when not in daemon mode, in addition to stdin/stdout outputs
* Refactor everything (especially main)
    * Don't mix c++ string and c string
    * Maybe use vectors for char buffers
    * Don't use shared pointer in static variables, if the pointers are the lifetime of the program it doesn't matter if the memory is never freed. Look into making static classes using factories. Re-assess what needs static variables
    * Put error handling in functions, less copy/pasting
* Add a way to schedule tasks that interact with stdout, ie. backing up the minecraft world only if >0 players are online
    * Also keep stateful record of when players are online, in case a player leaves between backups
    * This may require a separate executable compiled with an extra library (minecraft-webproc)
* Create HTTP interface
* Integrate with systemd
    * handle SIGTERM instead of/in addition to SIGINT
* Logger should use format strings
* Provide a user/group to which the daemon reduces its privileges
    * Make sure the future HTTP server will be able to use port 80/443
* Restarting the process?
* Create better documentation
