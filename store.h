#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include "expiry.h"

using namespace std;

class Store {
private:
    unordered_map<string, string> data;
    ExpiryManager expiry;
    string log_file = "appendonly.log";

    void appendToLog(string command);   
public:
    void set(string key, string value);
    optional<string> get(string key);
    bool del(string key);
    bool exists(string key);
    vector<string> keys();
    void flush();
    void setex(string key, int seconds, string value);
    string incr(string key);
    string decr(string key);

    void loadFromDisk();  
    int size();
};
