#include "parser.h"
#include <sstream>
#include <algorithm>

Command Parser::parse(string input) {
    Command cmd;

    if (input.empty()) return cmd;

    stringstream ss(input);
    string word;
    vector<string> tokens;

    while (ss >> word)
        tokens.push_back(word);

    if (tokens.empty()) return cmd;

    cmd.name = tokens[0];
    transform(
        cmd.name.begin(),
        cmd.name.end(),
        cmd.name.begin(),
        ::toupper
    );

    for (int i = 1; i < (int)tokens.size(); i++)
        cmd.args.push_back(tokens[i]);

    return cmd;
}
