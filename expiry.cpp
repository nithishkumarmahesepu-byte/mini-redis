#include "expiry.h"

void ExpiryManager::setExpiry(string key, int seconds) {
    expiry_map[key] = steady_clock::now() + chrono::seconds(seconds);
}

bool ExpiryManager::isExpired(string key) {
    if (expiry_map.find(key) == expiry_map.end())
        return false; // no expiry set = lives forever
    return steady_clock::now() > expiry_map[key];
}

void ExpiryManager::removeExpiry(string key) {
    expiry_map.erase(key);
}

bool ExpiryManager::hasExpiry(string key) {
    return expiry_map.find(key) != expiry_map.end();
}
