#include "AEE.h"
#include <fstream>
#include <string>
#include <climits>
#include <iostream>

using namespace std;

AEE::AEE() {
}

AEE::~AEE() {
}

int AEE::createIndex(const char *entity_file_name) {
    Lmin = INT_MAX;
    Lmax = -1;
    entities.clear();
    ifstream fin(entity_file_name);
    string s;
    while (getline(fin, s)) {
        int len = (int)s.length();
        if (len < Lmin)
            Lmin = len;
        if (len > Lmax)
            Lmax = len;
        entities.push_back(s);
    }
    return SUCCESS;
}

Node* AEE::insert(Node* parent, string& s, int pos) {
    if (pos == (int)s.length())
        return parent;
    if (parent->hasChild(s[pos]))
        return insert(parent->children[s[pos]], s, pos+1);
    Node* child = new Node(parent, s[pos]);
    parent->addChild(child);
    return insert(child, s, pos+1);
}

void AEE::printTree(Node* p, int depth) {
    if (p->children.empty()) {
        p->sort();
        for (auto& left : p->leftStrs)
            cout << "(" << left.first << ","  << left.second << ") ";
        cout << "; ";
        for (auto& right : p->rightStrs)
            cout << "(" << right.first << ","  << right.second << ") ";
        cout << endl;
    }
    bool first = true;
    for (auto& child : p->children) {
        for (int i = 0; first == false && i < depth; i++)
            cout << "   ";
        cout << "->" << child.first;
        printTree(child.second, depth+1);
        first = false;
    }
}

void AEE::buildTrie(const int threshold) {
    root = new Node(NULL, ' ');
    for (int lineNum = 0; lineNum < (int)entities.size(); lineNum++) {
        string& s = entities[lineNum];
        int len = (int)s.size();
        int shortLen = len / (threshold + 1);
        int longLen = shortLen + 1;
        int k = len - shortLen * (threshold + 1);
        // cout << s << ": ";
        // cout << s << endl;
        for (int i = 0; i < k; i++) {
            string em = s.substr(i * longLen, longLen);
            string el = s.substr(0, i * longLen);
            string er = s.substr((i+1) * longLen, len - (i+1) * longLen);
            // cout << " " << el <<  em << er <<endl;
            Node* loc = insert(root, em, 0);
        }
        for (int i = 0; i < threshold + 1 - k; i++) {
            int start = k * longLen + i * shortLen;
            string em = s.substr(start, shortLen);
            string el = s.substr(0, start);
            string er = s.substr(start + shortLen, len - (start + shortLen));
            // cout << " " << el <<  em << er <<endl;
            Node* loc = insert(root, em, 0);
            loc->insertEntity(el, er, lineNum);
            // cout << em << " ";
        }
        // cout << endl;
    }
    // printTree(root, 0);
}

int AEE::aeeJaccard(const char *document, double threshold, vector<JaccardExtractResult> &result) {
    result.clear();
    return SUCCESS;
}

int AEE::aeeED(const char *document, unsigned threshold, vector<EDExtractResult> &result) {
    result.clear();
    buildTrie((int)threshold);
    
    return SUCCESS;
}
