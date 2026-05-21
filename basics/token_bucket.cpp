#include<bits/stdc++.h>
using namespace std;

class TokenBucket{

    private:
    double refillrate;
    int capacity;
    double tokens;
    chrono::steady_clock::time_point lastRefill;

    public:

    TokenBucket(int cap,double rate){
        this->refillrate = rate;
        this->capacity = cap;
        this->tokens = cap;

        lastRefill = chrono::steady_clock::now();
    }

    void refill(){
        auto now = chrono::steady_clock::now();

        double seconds = chrono::duration_cast<chrono::milliseconds>(now - lastRefill).count() / 1000.0;

        double newTokens = seconds*refillrate;
        tokens = min((double)capacity, tokens+newTokens);

        lastRefill = now;
    }

    bool allowRequest(){

        refill();

        if(tokens >= 1){
            tokens-=1;
            return true;
        }

        return false;
    }

    double getTokens(){
        return tokens;
    }
};

int main() {

    TokenBucket bucket(5, 1); // capacity=5, refill=1 token/sec

    for(int i = 1; i <= 10; i++) {

        if(bucket.allowRequest())
            cout << "Request " << i << " allowed\n";
        else
            cout << "Request " << i << " blocked\n";
    }

    cout << "\nWaiting 3 seconds...\n";

    this_thread::sleep_for(chrono::seconds(3));

    for(int i = 11; i <= 15; i++) {

        if(bucket.allowRequest())
            cout << "Request " << i << " allowed\n";
        else
            cout << "Request " << i << " blocked\n";
    }
}
