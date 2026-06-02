#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <iostream>
#include <fstream>  // ADDED: For file writing/reading
#include <sstream>  // ADDED: For string splitting
#include "TextProcessor.h"

class Indexer {
private:
    std::unordered_map<std::string, std::unordered_set<std::string>> invertedIndex;
    std::mutex indexMutex;

public:
    void addDocument(const std::string& url, const std::vector<std::string>& words) {
        std::lock_guard<std::mutex> lock(indexMutex);
        for (const std::string& word : words) {
            invertedIndex[word].insert(url);
        }
    }

    std::vector<std::string> search(const std::string& rawQuery) {
        std::lock_guard<std::mutex> lock(indexMutex);
        TextProcessor processor;
        std::vector<std::string> queryWords = processor.processText(rawQuery);
        
        if (queryWords.empty()) return {};

        std::string firstWord = queryWords[0];
        if (invertedIndex.find(firstWord) == invertedIndex.end()) {
            return {}; 
        }

        std::unordered_set<std::string> resultURLs = invertedIndex[firstWord];

        for (size_t i = 1; i < queryWords.size(); ++i) {
            const std::string& currentWord = queryWords[i];
            if (invertedIndex.find(currentWord) == invertedIndex.end()) return {}; 

            const std::unordered_set<std::string>& wordURLs = invertedIndex[currentWord];
            std::unordered_set<std::string> intersectedURLs;

            for (const std::string& url : resultURLs) {
                if (wordURLs.find(url) != wordURLs.end()) {
                    intersectedURLs.insert(url);
                }
            }
            
            resultURLs = intersectedURLs;
            if (resultURLs.empty()) return {};
        }

        std::vector<std::string> results;
        for (const std::string& url : resultURLs) {
            results.push_back(url);
        }
        return results;
    }

    void printStats() {
        std::lock_guard<std::mutex> lock(indexMutex);
        std::cout << "\n--- INDEX STATS ---\n";
        std::cout << "Total unique keywords indexed: " << invertedIndex.size() << "\n";
    }

    // ---------------------------------------------------------
    // NEW: Save the index to a text file
    // ---------------------------------------------------------
    void saveToDisk(const std::string& filename) {
        std::lock_guard<std::mutex> lock(indexMutex);
        std::ofstream outFile(filename);
        
        if (!outFile.is_open()) {
            std::cerr << "[ERROR] Could not open " << filename << " for writing.\n";
            return;
        }

        for (const auto& pair : invertedIndex) {
            outFile << pair.first; // Write the keyword
            for (const std::string& url : pair.second) {
                outFile << " " << url; // Write each URL separated by a space
            }
            outFile << "\n"; // New line for the next keyword
        }
        
        outFile.close();
        std::cout << "[SUCCESS] Index saved to " << filename << " (" << invertedIndex.size() << " keywords).\n";
    }

    // ---------------------------------------------------------
    // NEW: Load the index from a text file
    // ---------------------------------------------------------
    void loadFromDisk(const std::string& filename) {
        std::lock_guard<std::mutex> lock(indexMutex);
        std::ifstream inFile(filename);
        
        if (!inFile.is_open()) {
            std::cerr << "[ERROR] Could not open " << filename << " for reading.\n";
            return;
        }

        invertedIndex.clear(); // Clear any existing data in RAM
        std::string line;
        
        while (std::getline(inFile, line)) {
            std::istringstream stream(line);
            std::string word;
            
            // The first token on the line is the keyword
            if (stream >> word) {
                std::string url;
                // Every subsequent token is a URL
                while (stream >> url) {
                    invertedIndex[word].insert(url);
                }
            }
        }
        
        inFile.close();
        std::cout << "[SUCCESS] Index loaded from " << filename << " (" << invertedIndex.size() << " keywords).\n";
    }
};

#endif // INDEXER_H