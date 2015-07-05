#include "Node.h"
#include <algorithm>
#include <iostream>
using namespace std;

int alphaToNum(const char c) {
    if (c == ' ')
        return 36;
    if (c == '.')
        return 37;
    if (c == ',')
        return 38;
    if (unsigned(c - '0') < 10)
        return c - '0' + 26;
    if (unsigned(c - 'a' < 26))
        // cout << c << endl;
        return c - 'a';
    // cout << "lala" << endl;
    return -1;

}

char numToAlpha(const int num) {
    if (num == 36)
        return ' ';
    if (num == 37)
        return '.';
    if (num == 38)
        return '.';
    if (num < 26)
        return num + 'a';
    return num - 26 + '0';
}

Node::Node() {
    isLeaf = false;
    sorted = false;
    next = NULL;
    for (int i = 0; i < ALPHABET; i++)
        children[i] = NULL;
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


