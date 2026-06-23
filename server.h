#pragma once
#include "store.h"
#include "parser.h"
#include <string>

using namespace std;

class Server {
private:
    int port;
    Store store;
    Parser parser;

    string handleCommand(string input);  

public:
    Server(int port);
    void start();
};
