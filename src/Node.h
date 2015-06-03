#ifndef __NODE_H__
#define __NODE_H__ 

#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

class Node {
public:
    std::vector<pair<string, int>> leftStrs, rightStrs;
    Node* parent;
    bool sorted;
    char c;
    unordered_map<char, Node*> children;
    bool isLeaf;
    bool hasChild(char c);
    Node(Node* p,  char c);
    void addChild(Node* child);
    void insertEntity(string& el, string& er, const int num);
    void sort();
};

#endif