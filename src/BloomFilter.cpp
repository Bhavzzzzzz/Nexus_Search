#include "BloomFilter.h"

BloomFilter::BloomFilter(int size){
    this->size = size;
    bits.resize(size);
}

// Hash function 1
int BloomFilter::hash1(string s){
    int hash = 0;

    for(auto c : s){
        hash += (hash * 31 + c) % size;
    }

    return hash % size;
}

// Hash function 2
int BloomFilter::hash2(string s){
    int hash = 0;

    for(auto c : s){
        hash += (hash * 35 + c) % size;
    }

    return hash % size;
}

// Hash function 3
int BloomFilter::hash3(string s){
    int hash = 0;

    for(auto c : s){
        hash += (hash * 37 + c) % size;
    }

    return hash % size;
}

void BloomFilter::add(string s){
    bits[hash1(s)] = true;
    bits[hash2(s)] = true;
    bits[hash3(s)] = true;
}

bool BloomFilter::contains(string s){
    if(!bits[hash1(s)] || !bits[hash2(s)] || !bits[hash3(s)])
        return false;

    return true;
}

void BloomFilter::print(){
    for(bool b : bits)
        cout << b << " ";

    cout << endl;
}