#include "Node.h"
#include <algorithm>
using namespace std;

Node::Node(Node* parent, char c) {
    isLeaf = false;
    sorted = false;
    this->parent = parent;
    this->c = c;
}

bool Node::hasChild(char c) {
    return children.find(c) != children.end();
}


void Node::addChild(Node* child) {
    children[child->c] = child;
}


void Node::insertEntity(string& el, string& er, const int num) {
    isLeaf = true;
    sorted = false;
    leftStrs.push_back(make_pair(el, num));
    rightStrs.push_back(make_pair(er, num));
}

void Node::sort() {
    if (sorted) return;
    sorted = true;
    std::sort(leftStrs.begin(), leftStrs.end());
    std::sort(rightStrs.begin(), rightStrs.end());
}
