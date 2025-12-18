#include <bits/stdc++.h>
using namespace std;

struct Result {
    string alg;
    int frames;
    int pageFaults;
    double faultRate;
};

vector<int> parseRefs(const string &s) {
    stringstream ss(s);
    vector<int> refs; 
    int x;
    while (ss >> x) refs.push_back(x);
    return refs;
}

// FIFO
int simulateFIFO(vector<int> refs, int frames) {
    queue<int> q;
    unordered_set<int> mem;
    int faults = 0;

    for (int p : refs) {
        if (mem.find(p) == mem.end()) { // miss
            faults++;
            if ((int)mem.size() < frames) {
                mem.insert(p);
                q.push(p);
            } else {
                int victim = q.front(); q.pop();
                mem.erase(victim);
                mem.insert(p);
                q.push(p);
            }
        }
    }
    return faults;
}

// LRU
int simulateLRU(vector<int> refs, int frames) {
    list<int> lru;
    unordered_set<int> mem;
    int faults = 0;

    for (int p : refs) {
        if (mem.find(p) == mem.end()) {
            faults++;
            if ((int)mem.size() < frames) {
                mem.insert(p);
                lru.push_front(p);
            } else {
                int victim = lru.back();
                lru.pop_back();
                mem.erase(victim);
                mem.insert(p);
                lru.push_front(p);
            }
        } else {
            // move to front
            lru.remove(p);
            lru.push_front(p);
        }
    }
    return faults;
}

// Optimal
int simulateOptimal(vector<int> refs, int frames) {
    vector<int> mem;
    int faults = 0;

    for (int i = 0; i < (int)refs.size(); i++) {
        int p = refs[i];
        if (find(mem.begin(), mem.end(), p) == mem.end()) {
            faults++;
            if ((int)mem.size() < frames) {
                mem.push_back(p);
            } else {
                int farthest = -1, idx = -1;
                for (int j = 0; j < frames; j++) {
                    int next = INT_MAX;
                    for (int k = i+1; k < (int)refs.size(); k++) {
                        if (refs[k] == mem[j]) { next = k; break; }
                    }
                    if (next > farthest) {
                        farthest = next;
                        idx = j;
                    }
                }
                mem[idx] = p;
            }
        }
    }
    return faults;
}

// LFU
int simulateLFU(vector<int> refs, int frames) {
    unordered_map<int,int> freq;
    vector<int> mem;
    int faults = 0;

    for (int p : refs) {
        if (find(mem.begin(), mem.end(), p) == mem.end()) {
            faults++;
            if ((int)mem.size() < frames) {
                mem.push_back(p);
                freq[p]++;
            } else {
                // find LFU, tie-break by order
                int minFreq = INT_MAX, idx = -1;
                for (int j = 0; j < frames; j++) {
                    if (freq[mem[j]] < minFreq) {
                        minFreq = freq[mem[j]];
                        idx = j;
                    }
                }
                freq[mem[idx]] = 0;
                mem[idx] = p;
                freq[p]++;
            }
        } else {
            freq[p]++;
        }
    }
    return faults;
}

// Clock (Second-Chance)
int simulateClock(vector<int> refs, int frames) {
    vector<int> mem(frames, -1);
    vector<int> use(frames, 0);
    int hand = 0, faults = 0;

    for (int p : refs) {
        bool found = false;
        for (int i = 0; i < frames; i++) {
            if (mem[i] == p) { use[i] = 1; found = true; break; }
        }
        if (!found) {
            faults++;
            while (true) {
                if (use[hand] == 0) {
                    mem[hand] = p;
                    use[hand] = 1;
                    hand = (hand + 1) % frames;
                    break;
                } else {
                    use[hand] = 0;
                    hand = (hand + 1) % frames;
                }
            }
        }
    }
    return faults;
}

int main() {
    string refStr;
    int frames;

    // User input
    cout << "Enter reference string (space-separated integers): ";
    getline(cin, refStr);
    cout << "Enter number of frames: ";
    cin >> frames;

    vector<int> refs = parseRefs(refStr);

    vector<Result> results;
    results.push_back({"FIFO", frames, simulateFIFO(refs, frames), 0});
    results.push_back({"LRU", frames, simulateLRU(refs, frames), 0});
    results.push_back({"Optimal", frames, simulateOptimal(refs, frames), 0});
    results.push_back({"LFU", frames, simulateLFU(refs, frames), 0});
    results.push_back({"Clock", frames, simulateClock(refs, frames), 0});

    cout << "\nDemand Paging Simulation\n";
    cout << "Reference String: " << refStr << "\n";
    cout << "Frames: " << frames << "\n\n";

    cout << setw(10) << "Alg" << setw(12) << "PageFaults" << setw(12) << "FaultRate\n";
    cout << "---------------------------------------\n";

    for (auto &r : results) {
        r.faultRate = (double)r.pageFaults / refs.size();
        cout << setw(10) << r.alg
             << setw(12) << r.pageFaults
             << setw(11) << fixed << setprecision(2) << r.faultRate*100 << "%\n";
    }
    return 0;
}
