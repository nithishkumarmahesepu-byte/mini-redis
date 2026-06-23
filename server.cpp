#include "server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>

using namespace std;

Server::Server(int port) : port(port) {}

string Server::handleCommand(string input) {
    Command cmd = parser.parse(input);

    if (cmd.name.empty())
        return "ERROR: Empty command\n";

    // ── SET key value ─────────────────────────────────────
    if (cmd.name == "SET") {
        if ((int)cmd.args.size() < 2)
            return "ERROR: Usage: SET <key> <value>\n";
        store.set(cmd.args[0], cmd.args[1]);
        return "OK\n";
    }

    // ── GET key ───────────────────────────────────────────
    if (cmd.name == "GET") {
        if ((int)cmd.args.size() < 1)
            return "ERROR: Usage: GET <key>\n";
        auto val = store.get(cmd.args[0]);
        if (val.has_value())
            return val.value() + "\n";
        return "(nil)\n";
    }

    // ── DEL key ───────────────────────────────────────────
    if (cmd.name == "DEL") {
        if ((int)cmd.args.size() < 1)
            return "ERROR: Usage: DEL <key>\n";
        bool deleted = store.del(cmd.args[0]);
        return deleted ? "1\n" : "0\n";
    }

    // ── EXISTS key ────────────────────────────────────────
    if (cmd.name == "EXISTS") {
        if ((int)cmd.args.size() < 1)
            return "ERROR: Usage: EXISTS <key>\n";
        return store.exists(cmd.args[0]) ? "1\n" : "0\n";
    }

    // ── KEYS ──────────────────────────────────────────────
    if (cmd.name == "KEYS") {
        auto k = store.keys();
        if (k.empty()) return "(empty)\n";
        string result = "";
        for (int i = 0; i < (int)k.size(); i++)
            result += to_string(i + 1) + ") " + k[i] + "\n";
        return result;
    }

    // ── FLUSH ─────────────────────────────────────────────
    if (cmd.name == "FLUSH") {
        store.flush();
        return "OK\n";
    }

    // ── SETEX key seconds value ───────────────────────────
    if (cmd.name == "SETEX") {
        if ((int)cmd.args.size() < 3)
            return "ERROR: Usage: SETEX <key> <seconds> <value>\n";
        try {
            int seconds = stoi(cmd.args[1]);
            if (seconds <= 0)
                return "ERROR: seconds must be a positive integer\n";
            store.setex(cmd.args[0], seconds, cmd.args[2]);
            return "OK\n";
        } catch (...) {
            return "ERROR: seconds must be a valid integer\n";
        }
    }

    // ── INCR key ──────────────────────────────────────────
    if (cmd.name == "INCR") {
        if ((int)cmd.args.size() < 1)
            return "ERROR: Usage: INCR <key>\n";
        return store.incr(cmd.args[0]) + "\n";
    }

    // ── DECR key ──────────────────────────────────────────
    if (cmd.name == "DECR") {
        if ((int)cmd.args.size() < 1)
            return "ERROR: Usage: DECR <key>\n";
        return store.decr(cmd.args[0]) + "\n";
    }

    // ── DBSIZE ────────────────────────────────────────────
    if (cmd.name == "DBSIZE") {
        return to_string(store.size()) + "\n";
    }

    // ── PING ──────────────────────────────────────────────
    if (cmd.name == "PING") {
        return "PONG\n";
    }

    // ── HELP ──────────────────────────────────────────────
    if (cmd.name == "HELP") {
        return
            "Commands:\n"
            "  SET <key> <value>          - Store a value\n"
            "  GET <key>                  - Get a value\n"
            "  DEL <key>                  - Delete a key\n"
            "  EXISTS <key>               - Check if key exists (1/0)\n"
            "  KEYS                       - List all keys\n"
            "  FLUSH                      - Delete all keys\n"
            "  SETEX <key> <secs> <val>   - Store with expiry\n"
            "  INCR <key>                 - Increment integer\n"
            "  DECR <key>                 - Decrement integer\n"
            "  DBSIZE                     - Number of keys\n"
            "  PING                       - Test connection\n"
            "  QUIT                       - Disconnect\n"
            "  HELP                       - Show this message\n";
    }

    // ── QUIT ──────────────────────────────────────────────
    if (cmd.name == "QUIT") {
        return "QUIT\n";  // server.cpp handles disconnect on QUIT
    }

    return "ERROR: Unknown command '" + cmd.name + "'. Type HELP for list.\n";
}


void Server::start() {

    // ── Step 1: Load existing data from disk ──────────────
    store.loadFromDisk();

    // ── Step 2: Create socket ─────────────────────────────
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cout << "[ERROR] Could not create socket" << endl;
        return;
    }

    // Allow port reuse (avoids "Address already in use" error)
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // ── Step 3: Bind to port ──────────────────────────────
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // accept from any IP
    address.sin_port        = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cout << "[ERROR] Could not bind to port " << port << endl;
        return;
    }

    // ── Step 4: Listen for connections ────────────────────
    listen(server_fd, 10);  // queue up to 10 pending connections

    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║       Mini-Redis Server          ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;
    cout << "[INFO] Server running on port " << port << endl;
    cout << "[INFO] Connect using: nc localhost " << port << endl;
    cout << "[INFO] Type HELP after connecting for commands" << endl;
    cout << "[INFO] Press Ctrl+C to stop server" << endl;
    cout << "──────────────────────────────────────" << endl;

    // ── Step 5: Accept clients in a loop ──────────────────
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(
            server_fd,
            (struct sockaddr*)&client_addr,
            &client_len
        );

        if (client_fd < 0) continue;

        cout << "[INFO] Client connected" << endl;

        // Send welcome message
        string welcome =
            "Welcome to Mini-Redis!\n"
            "Type HELP to see all commands.\n"
            "> ";
        write(client_fd, welcome.c_str(), welcome.size());

        // ── Step 6: Read commands from this client ────────
        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes = read(client_fd, buffer, sizeof(buffer) - 1);

            if (bytes <= 0) {
                cout << "[INFO] Client disconnected" << endl;
                break;
            }

            string input(buffer);

            // Remove \r and \n characters from input
            input.erase(
                remove(input.begin(), input.end(), '\n'),
                input.end()
            );
            input.erase(
                remove(input.begin(), input.end(), '\r'),
                input.end()
            );

            if (input.empty()) {
                write(client_fd, "> ", 2);
                continue;
            }

            cout << "[CMD] " << input << endl;

            // Handle QUIT command — disconnect client
            string upper = input;
            transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
            if (upper == "QUIT") {
                string bye = "Goodbye!\n";
                write(client_fd, bye.c_str(), bye.size());
                break;
            }

            // Process command and send response
            string response = handleCommand(input);
            response += "> ";  // show prompt after response
            write(client_fd, response.c_str(), response.size());
        }

        close(client_fd);
    }

    close(server_fd);
}
