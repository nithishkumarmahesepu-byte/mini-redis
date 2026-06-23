#include "server.h"
#include <iostream>

int main() {
    // Port 6379 is the real Redis port
    Server server(6379);
    server.start();
    return 0;
}
