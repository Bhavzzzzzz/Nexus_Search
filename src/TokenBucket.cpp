#include "TokenBucket.h"

TokenBucket::TokenBucket(){
    this->refillrate = 0;
    this->capacity = 0;
    this->tokens = 0;

    lastRefill = chrono::steady_clock::now();
}

TokenBucket::TokenBucket(int cap,double rate){
    this->refillrate = rate;
    this->capacity = cap;
    this->tokens = cap;

    lastRefill = chrono::steady_clock::now();
}

void TokenBucket::refill(){
    auto now = chrono::steady_clock::now();

    double seconds = chrono::duration_cast<chrono::milliseconds>(now - lastRefill).count() / 1000.0;

    double newTokens = seconds*refillrate;
    tokens = min((double)capacity, tokens+newTokens);

    lastRefill = now;
}

bool TokenBucket::consumeToken(){

    refill();

    if(tokens >= 1){
        tokens-=1;
        return true;
    }

    return false;
}

double TokenBucket::getTokens(){
    return tokens;
}
