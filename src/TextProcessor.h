#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>

class TextProcessor {
private:
    // A basic set of English stop words
    std::unordered_set<std::string> stopWords = {
        "a", "an", "and", "are", "as", "at", "be", "but", "by",
        "for", "if", "in", "into", "is", "it", "no", "not", "of",
        "on", "or", "such", "that", "the", "their", "then", "there",
        "these", "they", "this", "to", "was", "will", "with"
    };

    // Very naive stemming just for demonstration
    // In a production engine, you would use a library like the "Porter Stemmer"
    std::string naiveStem(std::string word) {
        // Defensive check: don't try to stem tiny words or empty strings
        if (word.length() <= 3) {
            return word;
        }

        if (word.length() > 4 && word.substr(word.length() - 3) == "ing") {
            return word.substr(0, word.length() - 3);
        }
        if (word.length() > 3 && word.substr(word.length() - 2) == "ly") {
            return word.substr(0, word.length() - 2);
        }
        // Safely check the back of the string
        if (word.length() > 3 && word.back() == 's') {
            if (word[word.length()-2] != 's') {
                return word.substr(0, word.length() - 1); 
            }
        }
        return word;
    }

public:
    // 1. Strip HTML tags
    std::string stripHTML(const std::string& html) {
        std::string cleanText;
        bool inTag = false;
        bool inScript = false;
        bool inStyle = false;

        for (size_t i = 0; i < html.length(); ++i) {
            // Check for <script>
            if (!inTag && i + 7 < html.length() && html.substr(i, 7) == "<script") {
                inScript = true;
            }
            // Check for </script>
            if (inScript && i + 9 < html.length() && html.substr(i, 9) == "</script>") {
                inScript = false;
                i += 8; // Skip ahead
                continue;
            }
            
            // Check for <style>
            if (!inTag && i + 6 < html.length() && html.substr(i, 6) == "<style") {
                inStyle = true;
            }
            // Check for </style>
            if (inStyle && i + 8 < html.length() && html.substr(i, 8) == "</style>") {
                inStyle = false;
                i += 7; // Skip ahead
                continue;
            }

            if (inScript || inStyle) continue; // Ignore all text inside these blocks

            if (html[i] == '<') {
                inTag = true;
            } else if (html[i] == '>') {
                inTag = false;
                cleanText += ' '; 
            } else if (!inTag) {
                cleanText += html[i];
            }
        }
        return cleanText;
    }

    // Add this inside the TextProcessor class, right below stripHTML

    bool isLikelyEnglish(const std::string& cleanText) {
        if (cleanText.empty()) return false;

        int nonAsciiCount = 0;
        int totalChars = 0;

        for (char c : cleanText) {
            // We only count actual letters/text, ignoring spaces
            if (c != ' ') {
                totalChars++;
                // If the byte value is outside standard ASCII (0-127)
                if (static_cast<unsigned char>(c) > 127) {
                    nonAsciiCount++;
                }
            }
        }

        if (totalChars == 0) return false;

        // If more than 10% of the text is non-ASCII, it is not English
        double foreignRatio = (double)nonAsciiCount / totalChars;
        return foreignRatio < 0.10; 
    }
    
    // 2, 3 & 4. Tokenize, Lowercase, Remove Stop Words, and Stem
    std::vector<std::string> processText(const std::string& rawHtml) {
        std::string text = stripHTML(rawHtml);
        std::vector<std::string> tokens;
        std::string currentWord = "";

        // Iterate through the cleaned text character by character
        for (char c : text) {
            // If it's a letter, lowercase it and add it to our current word
            if (std::isalpha(c)) {
                currentWord += std::tolower(c);
            } 
            // If it's not a letter (space, punctuation, number) and we have a word built up
            else if (!currentWord.empty()) {
                // Check if it's a stop word
                if (stopWords.find(currentWord) == stopWords.end()) {
                    // Stem it and add to our final tokens list
                    tokens.push_back(naiveStem(currentWord));
                }
                currentWord = ""; // Reset for the next word
            }
        }
        
        // Catch the last word if the string ends with a letter
        if (!currentWord.empty() && stopWords.find(currentWord) == stopWords.end()) {
            tokens.push_back(naiveStem(currentWord));
        }

        return tokens;
    }
};

#endif // TEXT_PROCESSOR_H