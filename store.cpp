#include "store.h"
#include <iostream>
#include <sstream>


void Store::appendToLog(string command) {
    ofstream file(log_file, ios::app);  
    if (file.is_open()) {
        file << command << "\n";
        file.close();
    }
}

// ─────────────────────────────────────────────────────────
// SET key value
// Stores the value and logs the command
// ─────────────────────────────────────────────────────────
void Store::set(string key, string value) {
    data[key] = value;
    expiry.removeExpiry(key);           
    appendToLog("SET " + key + " " + value);
}

// ─────────────────────────────────────────────────────────
// GET key
// Returns value if key exists and not expired
// ─────────────────────────────────────────────────────────
optional<string> Store::get(string key) {
    if (data.find(key) == data.end())
        return nullopt;

    if (expiry.isExpired(key)) {
        data.erase(key);
        expiry.removeExpiry(key);
        appendToLog("DEL " + key);
        return nullopt;
    }

    return data[key];
}

// ─────────────────────────────────────────────────────────
// DEL key
// Deletes a key. Returns true if it existed, false if not
// ─────────────────────────────────────────────────────────
bool Store::del(string key) {
    if (data.find(key) == data.end())
        return false;
    data.erase(key);
    expiry.removeExpiry(key);
    appendToLog("DEL " + key);
    return true;
}

// ─────────────────────────────────────────────────────────
// EXISTS key
// Returns true if key exists and is not expired
// ─────────────────────────────────────────────────────────
bool Store::exists(string key) {
    if (data.find(key) == data.end())
        return false;
    if (expiry.isExpired(key)) {
        data.erase(key);
        expiry.removeExpiry(key);
        appendToLog("DEL " + key);
        return false;
    }
    return true;
}

// ─────────────────────────────────────────────────────────
// KEYS
// Returns all non-expired keys
// ─────────────────────────────────────────────────────────
vector<string> Store::keys() {
    vector<string> result;
    vector<string> expired_keys;

    for (auto& pair : data) {
        if (expiry.isExpired(pair.first)) {
            expired_keys.push_back(pair.first);
        } else {
            result.push_back(pair.first);
        }
    }

    // Clean up expired keys
    for (auto& k : expired_keys) {
        data.erase(k);
        expiry.removeExpiry(k);
        appendToLog("DEL " + k);
    }

    return result;
}

// ─────────────────────────────────────────────────────────
// FLUSH
// Deletes everything including the log file
// ─────────────────────────────────────────────────────────
void Store::flush() {
    data.clear();
    // Wipe the log file completely
    ofstream file(log_file, ios::trunc);
    file.close();
}

// ─────────────────────────────────────────────────────────
// SETEX key seconds value
// Store with TTL — key auto-deletes after N seconds
// ─────────────────────────────────────────────────────────
void Store::setex(string key, int seconds, string value) {
    data[key] = value;
    expiry.setExpiry(key, seconds);
    appendToLog("SET " + key + " " + value);
    
}

// ─────────────────────────────────────────────────────────
// INCR key
// Increment integer value by 1
// ─────────────────────────────────────────────────────────
string Store::incr(string key) {
    if (data.find(key) == data.end())
        data[key] = "0";
    try {
        int val = stoi(data[key]);
        data[key] = to_string(val + 1);
        appendToLog("SET " + key + " " + data[key]);
        return data[key];
    } catch (...) {
        return "ERROR: value is not an integer";
    }
}

// ─────────────────────────────────────────────────────────
// DECR key
// Decrement integer value by 1
// ─────────────────────────────────────────────────────────
string Store::decr(string key) {
    if (data.find(key) == data.end())
        data[key] = "0";
    try {
        int val = stoi(data[key]);
        data[key] = to_string(val - 1);
        appendToLog("SET " + key + " " + data[key]);
        return data[key];
    } catch (...) {
        return "ERROR: value is not an integer";
    }
}

// ─────────────────────────────────────────────────────────
// SIZE
// Returns number of keys currently in store
// ─────────────────────────────────────────────────────────
int Store::size() {
    return (int)data.size();
}

// ─────────────────────────────────────────────────────────
// LOAD FROM DISK
// Called once when server starts
// Reads appendonly.log and replays every command
// ─────────────────────────────────────────────────────────
void Store::loadFromDisk() {
    ifstream file(log_file);

    if (!file.is_open()) {
        cout << "[INFO] No existing log file. Starting fresh." << endl;
        return;
    }

    string line;
    int count = 0;

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "SET") {
            string key, value;
            ss >> key >> value;
            data[key] = value;         
        }
        else if (cmd == "DEL") {
            string key;
            ss >> key;
            data.erase(key);
        }

        count++;
    }

    file.close();
    cout << "[INFO] Restored " << data.size()
         << " keys from disk ("
         << count << " log entries replayed)" << endl;
}
