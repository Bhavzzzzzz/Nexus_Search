#ifndef TOKENBUCKET_H
#define TOKENBUCKET_H

#include <bits/stdc++.h>
using namespace std;

class TokenBucket{

    private:
    double refillrate;
    int capacity;
    double tokens;
    chrono::steady_clock::time_point lastRefill;

    public:

    TokenBucket();

    TokenBucket(int cap,double rate);

    void refill();

    bool consumeToken();

    double getTokens();
};
#endif