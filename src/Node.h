#ifndef __NODE_H__
#define __NODE_H__ 

#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

class Node {
public:
    std::vector<pair<string, int>> leftStrs, rightStrs;
    Node* next;
    Node* parent;
    bool sorted;
    char c;
    int depth;
    unordered_map<char, Node*> children;
    // Node* children[]
    bool isLeaf;
    bool hasChild(char c);
    Node(Node* p,  char c);
    void addChild(Node* child);
    void insertEntity(string& el, string& er, const int num);
    void sort();
};

#endif