#ifndef __NODE_H__
#define __NODE_H__ 

#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

const int ALPHABET = 26+10+1;

int alphaToNum(const char c);
char numToAlpha(const int num);

class Node {
public:
    std::vector<pair<string, int>> leftStrs, rightStrs;
    Node* next;
    bool sorted;
    int depth;
    // unordered_map<char, Node*> children;
    Node* children[ALPHABET];
    bool isLeaf;
    Node();
    void insertEntity(string& el, string& er, const int num);
    void sort();
};

#endif