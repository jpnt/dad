# dad - Daemon as demanded

dad is a stupid simple program that allows you to create daemon groups and then
activate them all at once to run a program that needs it.

### Idea

Minimize number of daemons on the system; manually activate them when needed.
No need to run services when a program is not using it.

### Configuration

Configuration is done inside the single `config.h` header file.

### Installation

`sudo make clean install`
