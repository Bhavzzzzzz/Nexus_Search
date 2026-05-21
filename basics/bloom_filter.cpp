#include<bits/stdc++.h>
using namespace std;

class BloomFilter{

    private:
        int size;
        vector<bool>bits;
    
    public:
        BloomFilter(int size){
            this->size = size;
            bits.resize(size);
        }

        // Hash function 1

        int hash1(string s){
            int hash = 0;

            for(auto c : s){
                hash += (hash * 31 + c)%size;
            }

            return hash;
        }

        // Hash function 2

        int hash2(string s){
            int hash = 0;
            for(auto c : s) hash += (hash * 35 + c)%size;
            return hash;
        }

        // Hash function 3

        int hash3(string s){
            int hash = 0;
            for(auto c : s) hash += (hash * 37 + c)%size;
            return hash;
        }

        void add(string s){
            bits[hash1(s)] = true;
            bits[hash2(s)] = true;
            bits[hash3(s)] = true;
        }

        bool contains(string s){
            if(!bits[hash1(s)] || !bits[hash2(s)] || !bits[hash3(s)]) return false;
            return true;
        }

        void print() {
            for(bool b : bits)
                cout << b << " ";

            cout << endl;
        }
};


int main(){

    BloomFilter bf(10);

    bf.add("pizza");
    bf.add("burger");

    bf.print();

    cout << bf.contains("pizza") << endl; // probably true
    cout << bf.contains("burger") << endl; // probably true
    cout << bf.contains("pasta") << endl; // maybe false

}