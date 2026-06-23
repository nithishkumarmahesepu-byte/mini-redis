#pragma once
#include <unordered_map>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

class ExpiryManager {
private:
    unordered_map<string, time_point<steady_clock>> expiry_map;

public:
    void setExpiry(string key, int seconds);
    bool isExpired(string key);
    void removeExpiry(string key);
    bool hasExpiry(string key);
};
