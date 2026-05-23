#include "URLFrontier.h"

// Initialize the Bloom Filter size in the constructor's initializer list
URLFrontier::URLFrontier() : visitedUrls(100000) {}

string URLFrontier::extractDomain(const string& url) {
    size_t pos = url.find("://");
    if (pos == string::npos) return ""; 
    pos += 3;

    size_t endPos = url.find('/', pos);
    if (endPos == string::npos) {
        return url.substr(pos);
    }
    return url.substr(pos, endPos - pos);
}

void URLFrontier::addURL(const string& url) {
    if (visitedUrls.contains(url)) {
        return; // Discard duplicate
    }
    visitedUrls.add(url);
    urlQueue.push(url);
}

bool URLFrontier::shouldCrawl(const string& url) {
    string domain = extractDomain(url);
    if (domain.empty()) return false;

    // Create a bucket for a new domain: 1 token every 2 seconds (0.5 tokens/sec)
    if (domainRateLimiters.find(domain) == domainRateLimiters.end()) {
        domainRateLimiters[domain] = TokenBucket(1.0, 0.5); 
    }

    return domainRateLimiters[domain].consumeToken(); 
}

string URLFrontier::getNextURL() {
    if (urlQueue.empty()) {
        return "";
    }

    string nextUrl = urlQueue.front();
    urlQueue.pop();

    if (shouldCrawl(nextUrl)) {
        return nextUrl; 
    } else {
        urlQueue.push(nextUrl); // Push to back if rate limited
        return ""; 
    }
}

bool URLFrontier::isEmpty() const {
    return urlQueue.empty();
}