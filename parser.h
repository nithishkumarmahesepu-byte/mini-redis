#pragma once
#include <string>
#include <vector>

using namespace std;

struct Command {
    string name;           
    vector<string> args;  
};

class Parser {
public:
    Command parse(string input);
};
