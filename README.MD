#  C++ Distributed Search Engine

A high-performance, concurrent web crawler and search engine built entirely from scratch in modern C++17. This project implements a full Information Retrieval pipeline, from scraping HTML across distributed worker threads to ranking results using mathematically rigorous TF-IDF algorithms via a built-in REST API.

## Key Features

* **Concurrent Web Crawler:** Utilizes a custom C++ thread pool to asynchronously scrape multiple webpages simultaneously.
* **Politeness & Rate Limiting:** Implements a Token Bucket algorithm to respect server limits and avoid IP bans.
* **O(1) Deduplication:** Uses a custom Bloom Filter to guarantee links are never crawled twice, saving bandwidth and CPU cycles.
* **Text Processing Pipeline:** Strips HTML, tokenizes strings, filters stop-words, and removes non-English pages via ASCII ratio thresholding.
* **TF-IDF Ranking Engine:** Mathematically scores and sorts search results based on Term Frequency-Inverse Document Frequency.
* **Disk Serialization:** Saves the massive Inverted Index to disk using custom C++ file I/O for instant startup times.
* **Embedded Web Server:** Bypasses the terminal with a single-header HTTP server (`cpp-httplib`), exposing a REST API to a local Google-style web interface.

## Technology Stack

* **Language:** C++17
* **Build System:** CMake
* **Networking:** `libcurl` (Web Scraping)
* **Web Server:** `cpp-httplib` (REST API)
* **Concurrency:** `<thread>`, `<mutex>`, `<atomic>`

##  Prerequisites

To build and run this project, you need the following installed on your system (Linux/WSL):
* `g++` (Compiler with C++17 support)
* `cmake` (Build system)
* `libcurl4-openssl-dev` (For HTTP requests)

##  Build Instructions

This project uses CMake for out-of-source builds to keep the directory clean.

```bash
# 1. Clone the repository
git clone [https://github.com/Bhavzzzzzz/Nexus_Search.git](https://github.com/Bhavzzzzzz/Nexus_Search.git)
cd Nexus_Search.git

# 2. Create a build directory
mkdir build
cd build

# 3. Generate Makefiles and compile
cmake ..
make
```

## Usage 
After building, run the executable:
```bash
./crawler_prototype
```

You will be greeted with a startup menu offering three modes:

Mode 1: Run Crawler (Build Index)
Prompts you for a seed URL (e.g., https://en.wikipedia.org/wiki/Linux). The engine will spin up multiple worker threads, crawl the web, process the text, and save the resulting TF-IDF inverted index to a .dat file on your hard drive.

Mode 2: CLI Search (Terminal Mode)
Instantly loads your saved index from the hard drive and drops you into a terminal-based search prompt. Supports multi-word phrase intersection.

Mode 3: Start Web Server (Browser Mode)
Loads the index and starts a local web server on port 8081.
Open your web browser and navigate to http://localhost:8081 to use the visual search UI!

Architecture Overview
main.cpp: Entry point, thread pool management, and Web Server routing.

URLFrontier.h/cpp: Manages the URL queue, Token Bucket, and Bloom Filter.

TextProcessor.h: HTML stripping, regex tokenization, and language detection.

Indexer.h: Thread-safe Inverted Index, TF-IDF mathematics, and Disk I/O.

WebServer.h: Embedded HTTP server and HTML/JS frontend.

Author
[Bhavzzzzzz] - Backend & Systems Engineer