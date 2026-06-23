#pragma once
#include <string>
#include <vector>

using namespace std;

struct Command {
    string name;           // SET, GET, DEL, etc. (always uppercase)
    vector<string> args;   // remaining tokens
};

class Parser {
public:
    Command parse(string input);
};
