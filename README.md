# BookClub
A cross-platform Book Club Management System built with C++ and Qt Framework. This application features a robust backend powered by the C++ Standard Template Library (STL)

## Architecture: Server + Client (two separate executables)

This repository builds **two independent executables** from one CMake project,
communicating over TCP sockets:

- **BookClubServer** (`Server.exe`) — the core of the system. It owns the
  database, listens on a TCP port (default `5555`), and handles every
  request coming from clients. It must be started first and must stay
  running for as long as clients need to use the system.
- **BookClubClient** (`Client.exe`) — the app run by end users (regular
  user, publisher, or system admin). It connects to a running server over
  a socket using the server's IP + port, and cannot do anything if the
  server isn't reachable.

### How to run

1. Build the project (CMake + Qt6, `qt_add_executable` generates both
   `BookClubServer` and `BookClubClient`).
2. Run `Server.exe` first. It starts listening on port `5555` (or a custom
   port passed as `Server.exe <port>`) and shows a dashboard with logs,
   connected-client count, and start/stop control.
3. Run one or more `Client.exe` instances. Each one asks for the server's
   IP and port, then connects over a socket.
4. While the server is running, all connected clients can use the system
   at the same time.
5. If the server is closed:
   - New clients can no longer connect (`establishConnection` fails and
     the client shows an error).
   - Already-connected clients get a "disconnected from server" message
     when their socket drops.

Shared domain/model classes (`Book`, `User`, `Publisher`, `NetworkMessage`,
etc.) are compiled into both executables; server-only classes
(`DatabaseManager`, `ServerCore`, `RequestProcessor`, ...) and client-only
classes (`LoginWindow`, `DashboardController`, `ClientNetworkManager`, ...)
are each compiled only into their respective target.
