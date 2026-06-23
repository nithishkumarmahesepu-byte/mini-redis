# Mini-Redis — In-Memory Key-Value Store

A lightweight in-memory key-value store built in C++, inspired by Redis.
Supports TCP client connections, TTL-based expiry, AOF persistence, and common Redis-like commands.

## Features

- O(1) GET / SET using `unordered_map`
- TCP server using POSIX sockets — connect from any terminal
- TTL-based key expiry with lazy deletion (`SETEX`)
- AOF Persistence — data survives server restarts via `appendonly.log`
- Integer operations: `INCR`, `DECR`
- Clean modular architecture: Store, Parser, Server, ExpiryManager

## Tech Stack

- C++17
- POSIX Sockets (TCP networking)
- STL: unordered_map, chrono, fstream

## Project Structure

```
mini-redis/
├── main.cpp          # Entry point — starts the server
├── server.h/cpp      # TCP socket server + command routing
├── store.h/cpp       # Key-value storage + persistence
├── parser.h/cpp      # Parses raw command strings
├── expiry.h/cpp      # TTL / expiry management
├── Makefile          # Build automation
└── appendonly.log    # Auto-generated persistence file
```

## How to Build and Run

```bash
# Build
make

# Run
./mini-redis

# OR one command
make run
```

## How to Connect

Open a second terminal:

```bash
nc localhost 6379
```

## Commands

| Command                  | Description                        | Example                    |
|--------------------------|------------------------------------|----------------------------|
| `SET key value`          | Store a key-value pair             | `SET name Nithish`           |
| `GET key`                | Retrieve a value                   | `GET name`                 |
| `DEL key`                | Delete a key                       | `DEL name`                 |
| `EXISTS key`             | Check if key exists (1/0)          | `EXISTS name`              |
| `KEYS`                   | List all keys                      | `KEYS`                     |
| `FLUSH`                  | Delete all keys                    | `FLUSH`                    |
| `SETEX key secs value`   | Store with TTL expiry              | `SETEX token 30 abc123`    |
| `INCR key`               | Increment integer value by 1       | `INCR counter`             |
| `DECR key`               | Decrement integer value by 1       | `DECR counter`             |
| `DBSIZE`                 | Number of keys in store            | `DBSIZE`                   |
| `PING`                   | Test connection                    | `PING`                     |
| `HELP`                   | Show all commands                  | `HELP`                     |
| `QUIT`                   | Disconnect from server             | `QUIT`                     |

## Example Session

```
> SET name Nithish
OK
> GET name
Nithish
> SET age 20
OK
> INCR age
21
> KEYS
1) name
2) age
> SETEX session 10 active
OK
> GET session
active
(after 10 seconds)
> GET session
(nil)
> DBSIZE
2
> DEL name
1
> FLUSH
OK
> KEYS
(empty)
```

## Persistence Demo

```bash
# Start server
./mini-redis

# Connect and store data
SET name Nithish
SET city Hyderabad

# Stop server with Ctrl+C

# Check the log file
cat appendonly.log
# SET name Nithish
# SET city Hyderabad

# Restart server — data is restored automatically
./mini-redis
# [INFO] Restored 2 keys from disk (2 log entries replayed)
```

## Resume Points

```
Mini-Redis — In-Memory Key-Value Store  (C++, POSIX Sockets)

- Built a TCP server handling client connections via POSIX sockets on port 6379
- Implemented O(1) key-value storage using unordered_map
- Added TTL-based key expiry using lazy deletion with std::chrono
- Implemented AOF (Append-Only File) persistence — replays log on restart to restore data
- Supported Redis-like commands: SET, GET, DEL, EXISTS, SETEX, INCR, DECR, FLUSH
- Designed modular architecture: Store, Parser, Server, ExpiryManager components
```
