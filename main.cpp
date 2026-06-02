#include <bits/stdc++.h>
#include <regex>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <curl/curl.h> 
#include "src/URLFrontier.h"
#include "src/TextProcessor.h"
#include "src/Indexer.h"

// Thread-safe counter to limit the prototype run
atomic<int> pagesCrawled(0);
const int MAX_PAGES = 50;

// Mutex to prevent jumbled terminal output
mutex coutMutex; 

// ---------------------------------------------------------
// libcurl Callback
// ---------------------------------------------------------
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

// ---------------------------------------------------------
// HTTP Fetcher
// ---------------------------------------------------------
string fetchHTML(const string& url) {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); 
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); 
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bhavya_Crawler/1.0 (Student System Design Project)");

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            lock_guard<mutex> lock(coutMutex);
            cerr << "  -> curl failed on " << url << ": " << curl_easy_strerror(res) << "\n";
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

// ---------------------------------------------------------
// HTML Parser
// ---------------------------------------------------------
vector<string> extractLinks(const string& html) {
    vector<string> links;
    regex link_regex("<a\\s+[^>]*href=\"([^\"]*)\"");
    auto words_begin = sregex_iterator(html.begin(), html.end(), link_regex);
    auto words_end = sregex_iterator();

    for (sregex_iterator i = words_begin; i != words_end; ++i) {
        smatch match = *i;
        string extractedUrl = match[1].str();
        if (extractedUrl.find("http") == 0) {
            links.push_back(extractedUrl);
        }
    }
    return links;
}

// ---------------------------------------------------------
// Worker Thread Function
// ---------------------------------------------------------
void workerThread(int workerId, URLFrontier& frontier, Indexer& index) {
    while (pagesCrawled < MAX_PAGES) {
        std::string targetUrl = frontier.getNextURL();
        
        if (!targetUrl.empty()) {
            pagesCrawled++;
            
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "[Worker " << workerId << " | Page " << pagesCrawled << "] CRAWLING: " << targetUrl << "\n";
            }

            try {
                // 1. Fetch
                std::string html = fetchHTML(targetUrl);
                
                // 2. Extract Links
                std::vector<std::string> newLinks = extractLinks(html);

                // 3. Queue new links
                for (const std::string& link : newLinks) {
                    frontier.addURL(link);
                }

                // 4. Extract Text
                TextProcessor processor;
                std::vector<std::string> keywords = processor.processText(html);

                {
                    std::lock_guard<std::mutex> lock(coutMutex);
                    std::cout << "  -> Extracted " << keywords.size() << " valid keywords.\n";
                    if (!keywords.empty()) {
                        std::cout << "     Sample: " << keywords[0];
                        if (keywords.size() > 1) std::cout << ", " << keywords[1];
                        std::cout << "\n";
                        index.addDocument(targetUrl,keywords);
                    }
                }
            } 
            catch (const std::exception& e) {
                // If anything goes wrong, the thread survives!
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cerr << "  -> [ERROR] Worker " << workerId << " crashed on " << targetUrl 
                          << " | Reason: " << e.what() << "\n";
            }
            catch (...) {
                // Catch absolute worst-case scenario unknown errors
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cerr << "  -> [CRITICAL ERROR] Worker " << workerId << " encountered an unknown crash on " << targetUrl << "\n";
            }
            
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}

// ---------------------------------------------------------
// Main Function
// ---------------------------------------------------------
// ---------------------------------------------------------
// Main Function
// ---------------------------------------------------------
int main() {
    Indexer globalIndex;
    std::string indexFilename = "search_index.dat";

    std::cout << "==========================================\n";
    std::cout << "      DISTRIBUTED SEARCH ENGINE v1.0      \n";
    std::cout << "==========================================\n";
    std::cout << "1. Run Crawler (Build new index)\n";
    std::cout << "2. Load Index from Disk (Fast startup)\n";
    std::cout << "Select mode (1 or 2): ";
    
    int mode;
    std::cin >> mode;

    if (mode == 1) {
        URLFrontier frontier;
        frontier.addURL("https://en.wikipedia.org/wiki/History_of_Uttar_Pradesh");

        int numThreads = 4;
        std::vector<std::thread> threads;

        std::cout << "\nStarting crawler with " << numThreads << " concurrent workers...\n\n";

        for (int i = 1; i <= numThreads; ++i) {
            threads.emplace_back(workerThread, i, std::ref(frontier), std::ref(globalIndex));
        }

        for (auto& t : threads) {
            t.join();
        }

        std::cout << "\nCrawler stopped. Reached maximum limit of " << MAX_PAGES << " pages.\n";
        
        // Save the index to disk immediately after crawling finishes!
        globalIndex.saveToDisk(indexFilename);

    } else if (mode == 2) {
        std::cout << "\nLoading existing index from disk...\n";
        globalIndex.loadFromDisk(indexFilename);
    } else {
        std::cout << "Invalid mode selected. Exiting.\n";
        return 1;
    }

    // ---------------------------------------------------------
    // The Search Interface
    // ---------------------------------------------------------
    globalIndex.printStats();

    std::cout << "\n--- SEARCH ENGINE PROTOTYPE ---\n";
    
    // Clear the input buffer to prevent getline issues after reading 'mode'
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

    while (true) {
        std::string query;
        std::cout << "Enter a keyword or phrase to search (or type 'exit'): ";
        std::getline(std::cin, query); 

        if (query == "exit") break;
        if (query.empty()) continue;

        std::vector<std::string> results = globalIndex.search(query);
        
        std::cout << "Found " << results.size() << " results for '" << query << "':\n";
        for (size_t i = 0; i < std::min(results.size(), (size_t)10); ++i) {
            std::cout << "  " << i+1 << ". " << results[i] << "\n";
        }
    }

    return 0;
}