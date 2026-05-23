#include <bits/stdc++.h>
#include <regex>
#include <thread>
#include <chrono>
#include <curl/curl.h> // The HTTP library
#include "src/URLFrontier.h"
using namespace std;
// ---------------------------------------------------------
// libcurl Callback: Writes downloaded data into a string
// ---------------------------------------------------------
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

// ---------------------------------------------------------
// HTTP Fetcher: Downloads the raw HTML of a page
// ---------------------------------------------------------
std::string fetchHTML(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10-second timeout
        
        // ADDED: The User-Agent Header to bypass basic bot blocking
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bhavya_Crawler/1.0 (Student System Design Project)");

        // Perform the request
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "  -> curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

// ---------------------------------------------------------
// HTML Parser: Extracts all href links using Regex
// ---------------------------------------------------------
vector<string> extractLinks(const string& html) {
    vector<string> links;
    
    // Basic regex to find href="..."
    regex link_regex("<a\\s+[^>]*href=\"([^\"]*)\"");
    auto words_begin = sregex_iterator(html.begin(), html.end(), link_regex);
    auto words_end = sregex_iterator();

    for (sregex_iterator i = words_begin; i != words_end; ++i) {
        smatch match = *i;
        string extractedUrl = match[1].str();
        
        // Filter to only grab absolute http/https links for now
        // (Ignoring relative paths like "/wiki/Main_Page")
        if (extractedUrl.find("http") == 0) {
            links.push_back(extractedUrl);
        }
    }
    return links;
}

// ---------------------------------------------------------
// Main Crawler Loop
// ---------------------------------------------------------
int main() {
    URLFrontier frontier;

    // Seed the frontier with your Wikipedia link
    frontier.addURL("https://en.wikipedia.org/wiki/History_of_Uttar_Pradesh");

    cout << "Starting crawler loop..." << endl;

    // Increased iterations so you can watch it bounce between pages
    int iterations = 0;
    while (!frontier.isEmpty() && iterations < 50) {
        string targetUrl = frontier.getNextURL();
        
        if (!targetUrl.empty()) {
            cout << "\n[" << iterations + 1 << "] CRAWLING: " << targetUrl << endl;
            
            // 1. Fetch the HTML
            string html = fetchHTML(targetUrl);
            cout << "  -> Downloaded " << html.length() << " bytes of HTML.\n";

            // 2. Extract the links
            vector<string> newLinks = extractLinks(html);
            cout << "  -> Extracted " << newLinks.size() << " absolute http(s) links.\n";

            // 3. Feed links back into the Frontier (Bloom Filter handles duplicates)
            for (const string& link : newLinks) {
                frontier.addURL(link);
            }
            
        } else {
            cout << "[WAITING] Rate limited. Sleeping for 500ms..." << endl;
            this_thread::sleep_for(chrono::milliseconds(500));
        }
        iterations++;
    }

    cout << "\nCrawler stopped (Hit iteration limit or queue empty)." << endl;
    return 0;
}