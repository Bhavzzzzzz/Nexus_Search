#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <iostream>

class Indexer {
private:
    // The core data structure: Maps a Keyword -> Set of URLs
    std::unordered_map<std::string, std::unordered_set<std::string>> invertedIndex;
    
    // Mutex to protect the index when multiple workers write to it simultaneously
    std::mutex indexMutex;

public:
    // Add words to the index
    void addDocument(const std::string& url, const std::vector<std::string>& words) {
        std::lock_guard<std::mutex> lock(indexMutex);
        for (const std::string& word : words) {
            invertedIndex[word].insert(url);
        }
    }

    // Search the index
    std::vector<std::string> search(const std::string& query) {
        std::lock_guard<std::mutex> lock(indexMutex);
        
        // Convert the query to lowercase just in case
        std::string lowerQuery = "";
        for(char c : query) {
            lowerQuery += std::tolower(c);
        }

        if (invertedIndex.find(lowerQuery) != invertedIndex.end()) {
            // Found it! Convert the set back to a vector for easy returning
            std::vector<std::string> results;
            for (const std::string& url : invertedIndex[lowerQuery]) {
                results.push_back(url);
            }
            return results;
        } 
        
        return {}; // Return empty vector if word not found
    }

    // Print a quick summary of the index size
    void printStats() {
        std::lock_guard<std::mutex> lock(indexMutex);
        std::cout << "\n--- INDEX STATS ---\n";
        std::cout << "Total unique keywords indexed: " << invertedIndex.size() << "\n";
    }
};

#endif // INDEXER_H