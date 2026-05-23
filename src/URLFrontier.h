#ifndef URL_FRONTIER_H
#define URL_FRONTIER_H

#include <bits/stdc++.h>
#include "BloomFilter.h"
#include "TokenBucket.h"
using namespace std;
class URLFrontier {
private:
    queue<string> urlQueue;
    BloomFilter visitedUrls;
    unordered_map<string, TokenBucket> domainRateLimiters;

    // Helper to parse domain from a full URL
    string extractDomain(const string& url);

public:
    URLFrontier();
    
    void addURL(const string& url);
    string getNextURL();
    bool shouldCrawl(const string& url);
    bool isEmpty() const;
};

#endif // URL_FRONTIER_H