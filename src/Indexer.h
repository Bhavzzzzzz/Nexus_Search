#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>     // ADDED: For log()
#include <algorithm> // ADDED: For sort()
#include "TextProcessor.h"

class Indexer {
private:
    // Core Map: Keyword -> (URL -> Frequency Count)
    std::unordered_map<std::string, std::unordered_map<std::string, int>> invertedIndex;
    
    // Track all unique URLs seen to calculate Total Documents (N) for IDF
    std::unordered_set<std::string> allDocuments;
    std::mutex indexMutex;

public:
    void addDocument(const std::string& url, const std::vector<std::string>& words) {
        std::lock_guard<std::mutex> lock(indexMutex);
        allDocuments.insert(url); // Track total universe of URLs
        
        for (const std::string& word : words) {
            invertedIndex[word][url]++; // Increment frequency!
        }
    }

    // CHANGED: Now returns a pair of <URL, Score>
    std::vector<std::pair<std::string, double>> search(const std::string& rawQuery) {
        std::lock_guard<std::mutex> lock(indexMutex);
        TextProcessor processor;
        std::vector<std::string> queryWords = processor.processText(rawQuery);
        
        if (queryWords.empty()) return {};

        // 1. Set Intersection (Find URLs containing ALL words)
        std::string firstWord = queryWords[0];
        if (invertedIndex.find(firstWord) == invertedIndex.end()) return {}; 

        std::unordered_set<std::string> validURLs;
        for (const auto& pair : invertedIndex[firstWord]) {
            validURLs.insert(pair.first); // pair.first is the URL
        }

        for (size_t i = 1; i < queryWords.size(); ++i) {
            const std::string& currentWord = queryWords[i];
            if (invertedIndex.find(currentWord) == invertedIndex.end()) return {}; 

            std::unordered_set<std::string> intersectedURLs;
            for (const auto& pair : invertedIndex[currentWord]) {
                if (validURLs.find(pair.first) != validURLs.end()) {
                    intersectedURLs.insert(pair.first);
                }
            }
            validURLs = intersectedURLs;
            if (validURLs.empty()) return {};
        }

        // 2. TF-IDF Scoring
        int N = allDocuments.size(); // Total documents in corpus
        std::vector<std::pair<std::string, double>> rankedResults;

        for (const std::string& url : validURLs) {
            double totalScore = 0.0;
            
            for (const std::string& word : queryWords) {
                // TF: How many times this word is on this specific URL
                double tf = invertedIndex[word][url];
                
                // IDF: log( Total Docs / Docs containing this word )
                double df = invertedIndex[word].size();
                double idf = std::log( (double)N / (1.0 + df) ); // +1.0 prevents division by zero

                totalScore += (tf * idf);
            }
            rankedResults.push_back({url, totalScore});
        }

        // 3. Sort Results by Score (Descending)
        std::sort(rankedResults.begin(), rankedResults.end(), 
            [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
                return a.second > b.second; 
            });

        return rankedResults;
    }

    void printStats() {
        std::lock_guard<std::mutex> lock(indexMutex);
        std::cout << "\n--- INDEX STATS ---\n";
        std::cout << "Total unique keywords indexed: " << invertedIndex.size() << "\n";
        std::cout << "Total documents processed: " << allDocuments.size() << "\n";
    }

    // UPDATED: Save formatting is now -> keyword url1 count1 url2 count2...
    void saveToDisk(const std::string& filename) {
        std::lock_guard<std::mutex> lock(indexMutex);
        std::ofstream outFile(filename);
        if (!outFile.is_open()) return;

        for (const auto& pair : invertedIndex) {
            outFile << pair.first; 
            for (const auto& urlData : pair.second) {
                outFile << " " << urlData.first << " " << urlData.second; 
            }
            outFile << "\n"; 
        }
        outFile.close();
        std::cout << "[SUCCESS] Index saved to " << filename << "\n";
    }

    void loadFromDisk(const std::string& filename) {
        std::lock_guard<std::mutex> lock(indexMutex);
        std::ifstream inFile(filename);
        if (!inFile.is_open()) return;

        invertedIndex.clear(); 
        allDocuments.clear();
        std::string line;
        
        while (std::getline(inFile, line)) {
            std::istringstream stream(line);
            std::string word, url;
            int count;
            
            if (stream >> word) {
                while (stream >> url >> count) {
                    invertedIndex[word][url] = count;
                    allDocuments.insert(url);
                }
            }
        }
        inFile.close();
        std::cout << "[SUCCESS] Index loaded from " << filename << "\n";
    }
};

#endif // INDEXER_H