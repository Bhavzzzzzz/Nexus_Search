#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include<bits/stdc++.h>
using namespace std;

class BloomFilter{

    private:
        int size;
        vector<bool> bits;

    public:
        BloomFilter(int size);

        int hash1(string s);
        int hash2(string s);
        int hash3(string s);

        void add(string s);

        bool contains(string s);

        void print();
};

#endif