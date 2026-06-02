#ifndef INDEXER_H
#define INDEXER_H

#include <mutex>
#include<bits/stdc++.h>
#include "TextProcessor.h" // ADDED: So we can process user queries

using namespace std;

class Indexer {
private:
    unordered_map<string, unordered_set<string>> invertedIndex;
    mutex indexMutex;

public:
    void addDocument(const string& url, const vector<string>& words) {
        lock_guard<mutex> lock(indexMutex);
        for (const string& word : words) {
            invertedIndex[word].insert(url);
        }
    }

    // UPDATED: Multi-word search with Set Intersection
    vector<string> search(const string& rawQuery) {
        lock_guard<mutex> lock(indexMutex);
        
        // 1. Process the user's query exactly like we process scraped HTML
        TextProcessor processor;
        vector<string> queryWords = processor.processText(rawQuery);
        
        // If the query was empty or just stop-words (e.g. "the"), return nothing
        if (queryWords.empty()) return {};

        // 2. Start with the URL set of the FIRST word in the query
        string firstWord = queryWords[0];
        if (invertedIndex.find(firstWord) == invertedIndex.end()) {
            return {}; // If the first word isn't in the index, the intersection is empty
        }

        // Make a copy of the first word's URLs to start our intersection
        unordered_set<string> resultURLs = invertedIndex[firstWord];

        // 3. Intersect with the URL sets of all remaining words
        for (size_t i = 1; i < queryWords.size(); ++i) {
            const string& currentWord = queryWords[i];
            
            // If any word in the query doesn't exist, there are 0 matches for the full phrase
            if (invertedIndex.find(currentWord) == invertedIndex.end()) {
                return {}; 
            }

            const unordered_set<string>& wordURLs = invertedIndex[currentWord];
            unordered_set<string> intersectedURLs;

            // Only keep URLs that are in BOTH resultURLs and wordURLs
            for (const string& url : resultURLs) {
                if (wordURLs.find(url) != wordURLs.end()) {
                    intersectedURLs.insert(url);
                }
            }
            
            resultURLs = intersectedURLs;
            
            // Optimization: If at any point the intersection becomes empty, stop early
            if (resultURLs.empty()) return {};
        }

        // 4. Convert the final intersected set back to a vector for the console output
        vector<string> results;
        for (const string& url : resultURLs) {
            results.push_back(url);
        }
        return results;
    }

    void printStats() {
        lock_guard<mutex> lock(indexMutex);
        cout << "\n--- INDEX STATS ---\n";
        cout << "Total unique keywords indexed: " << invertedIndex.size() << "\n";
    }
};

#endif // INDEXER_H