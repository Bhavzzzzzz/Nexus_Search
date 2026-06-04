#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "httplib.h"
#include "Indexer.h"
#include <iostream>
#include <string>

class WebServer {
private:
    Indexer& index; // Reference to your global index

public:
    // Constructor requires the index so the server knows how to search
    WebServer(Indexer& globalIndex) : index(globalIndex) {}

    void start(int port = 8081) {
        httplib::Server svr;

        // 1. Serve the Frontend HTML page
        svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
            std::string html = R"html(
            <!DOCTYPE html>
            <html>
            <head>
                <title>C++ Search Engine</title>
                <style>
                    body { font-family: Arial, sans-serif; background-color: #202124; color: white; display: flex; flex-direction: column; align-items: center; margin-top: 10%; }
                    h1 { font-size: 3rem; color: #4285F4; margin-bottom: 20px; }
                    .search-container { width: 100%; max-width: 600px; display: flex; flex-direction: column; align-items: center; }
                    input { width: 100%; padding: 15px; border-radius: 24px; border: 1px solid #5f6368; background-color: #202124; color: white; font-size: 1.1rem; outline: none; }
                    input:focus { background-color: #303134; }
                    button { margin-top: 20px; padding: 10px 20px; background-color: #303134; border: 1px solid #5f6368; color: white; border-radius: 4px; cursor: pointer; }
                    button:hover { background-color: #404144; }
                    #results { width: 100%; max-width: 600px; margin-top: 30px; text-align: left; }
                    .result-item { margin-bottom: 20px; }
                    .result-link { color: #8ab4f8; text-decoration: none; font-size: 1.2rem; }
                    .result-link:hover { text-decoration: underline; }
                    .result-score { color: #9aa0a6; font-size: 0.9rem; }
                </style>
            </head>
            <body>
                <h1>C++ Search</h1>
                <div class="search-container">
                    <input type="text" id="query" placeholder="Search the web..." onkeypress="if(event.key === 'Enter') performSearch()">
                    <button onclick="performSearch()">Search</button>
                </div>
                <div id="results"></div>

                <script>
                    async function performSearch() {
                        const query = document.getElementById('query').value;
                        if (!query) return;
                        
                        const resultsDiv = document.getElementById('results');
                        resultsDiv.innerHTML = "<i>Searching...</i>";

                        const response = await fetch(`/api/search?q=${encodeURIComponent(query)}`);
                        const data = await response.json();

                        resultsDiv.innerHTML = `<div>Found ${data.length} results</div><br>`;
                        
                        data.forEach(item => {
                            resultsDiv.innerHTML += `
                                <div class="result-item">
                                    <a class="result-link" href="${item.url}" target="_blank">${item.url}</a>
                                    <div class="result-score">TF-IDF Score: ${item.score.toFixed(4)}</div>
                                </div>`;
                        });
                    }
                </script>
            </body>
            </html>
            )html";
            
            res.set_content(html, "text/html");
        });

        // 2. The REST API Endpoint
        // We use a lambda capture [this] so we can access this->index inside the route
        svr.Get("/api/search", [this](const httplib::Request& req, httplib::Response& res) {
            if (req.has_param("q")) {
                std::string query = req.get_param_value("q");
                
                auto results = this->index.search(query);
                
                std::string json = "[";
                for (size_t i = 0; i < std::min(results.size(), (size_t)15); ++i) {
                    json += "{\"url\": \"" + results[i].first + "\", \"score\": " + std::to_string(results[i].second) + "}";
                    if (i < std::min(results.size(), (size_t)15) - 1) json += ",";
                }
                json += "]";

                res.set_content(json, "application/json");
            } else {
                res.set_content("[]", "application/json");
            }
        });

        std::cout << "\n[SERVER] Web server started successfully.\n";
        std::cout << "[SERVER] Open your browser and go to: http://localhost:" << port << "\n";
        std::cout << "[SERVER] Press Ctrl+C in this terminal to stop.\n";
        
        svr.listen("0.0.0.0", port);
    }
};

#endif // WEBSERVER_H