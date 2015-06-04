#include "AEE.h"
#include <fstream>
#include <string>
#include <climits>
#include <iostream>
#include <algorithm>

using namespace std;

AEE::AEE() {
}

AEE::~AEE() {
}

const int maxED = 16;

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
    M = new int* [Lmax];
    for(int i = 0; i < Lmax; i++)
        M[i] = new int[maxED];
    return SUCCESS;
}

Node* AEE::insert(string& s) {
    Node* p = root;
    for (int i = 0; i < (int)s.length(); i++) {
        if (p->hasChild(s[i])) {
            p = p->children[s[i]];
            continue;
        }
        Node* child = new Node(p, s[i]);
        child->depth = i+1;
        p->addChild(child);
        p = child;
    }
    return p;
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
            cout << "     ";
        cout << "->" << child.first;
        if (child.second->next != NULL)
            cout << child.second->next->depth << child.second->next->c;
        else
            cout << "  ";
        printTree(child.second, depth+1);
        first = false;
    }
}


// Build KMP next
void AEE::buildNext(Node* t, Node* j) {
    if (t->hasChild(j->c)) {
        t = t->children[j->c];
        j->next = t;
        for (auto& child : j->children) {
            buildNext(t, child.second);
        }
    } else if (t == root) {
        j->next = root;
        for (auto& child : j->children) {
            buildNext(root, child.second);
        }
    } else {
        buildNext(t->next, j);
    }
}
// Build KMP next
void AEE::buildNext() {
    root->next = root;
    for (auto& child : root->children) {
        child.second->next = root;
    }
    for (auto& child : root->children)
        for (auto& grandchild : child.second->children) {
            buildNext(root, grandchild.second);
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
        cout << s << endl;
        for (int i = 0; i < k; i++) {
            string em = s.substr(i * longLen, longLen);
            string el = s.substr(0, i * longLen);
            string er = s.substr((i+1) * longLen, len - (i+1) * longLen);
            // cout << " " << el <<  em << er <<endl;
            Node* loc = insert(em);
            loc->insertEntity(el, er, lineNum);
        }
        for (int i = 0; i < threshold + 1 - k; i++) {
            int start = k * longLen + i * shortLen;
            string em = s.substr(start, shortLen);
            string el = s.substr(0, start);
            string er = s.substr(start + shortLen, len - (start + shortLen));
            // cout << " " << el <<  em << er <<endl;
            Node* loc = insert(em);
            loc->insertEntity(el, er, lineNum);
            // cout << em << " ";
        }
        // cout << endl;
    }
}

// Node* AEE::search(string& str) {
//     Node* p = root;
//     for (int i = 0; i < (int)str.length(); i++) {
//         if (p->isLeaf && !p->hasChild)
//         p = p->children[str[i]];
//     }
//     return p;
// }

int AEE::aeeJaccard(const char *document, double threshold, vector<JaccardExtractResult> &result) {
    result.clear();
    return SUCCESS;
}

inline int min(int x, int y, int z) {
    return min(min(x, y), z);
}

int AEE::maxCommonPrefixLen(std::vector<pair<string, int>>& rightStrs, int cur, int pre, int bound) {
    int i = 0;
    while (i < bound && pre >= 0) {
        if (rightStrs[cur].first[i] == rightStrs[pre].first[i])
            i ++;
        else
            break;
    }
    return i;
}

int AEE::maxCommonSuffixLen(std::vector<pair<string, int>>& leftStrs, int cur, int pre, int bound) {
    int i = 0;
    int len1 = (int)leftStrs[cur].first.length();
    int len2 = (int)leftStrs[pre].first.length();
    while (i < bound && pre >= 0) {
        if (leftStrs[cur].first[len1 - 1 - i] == leftStrs[pre].first[len2 - 1 - i])
            i ++;
        else
            break;
    }
    return i;

}
void AEE::extension(Node* p, string& D, int right, int left, int threshold, vector<EDExtractResult> &result) {
    vector<pair<string, int>>& leftStrs = p->leftStrs;
    vector<pair<string, int>>& rightStrs = p->rightStrs;
    for (int k = 0; k <= 2*threshold; k++) {
        M[0][k] = k - threshold > 0 ? k - threshold : threshold - k;
    }
    p->sort();
    int j = 0;
    unordered_map<int, vector<pair<int, int>>> entity_EDend;
    for (int i = 0; i < (int)rightStrs.size(); i++) {
        int minEd = threshold + 1;
        int erLen = (int)rightStrs[i].first.length();
        j = maxCommonPrefixLen(rightStrs, i, i-1, j);
        for (; j < erLen; j++) {
            // |v| = j + 1
            for (int k = 0; k <= 2*threshold; k++) {
                int c = 1;
                int index = left + j - threshold + k;
                if (index >= left && index < (int)D.length()) {
                    char v = rightStrs[i].first[j];
                    char d = D[index];
                    c = (v == d) ? 0 : 1;
                }
                if (k == 0) {
                    M[j+1][0] = min(M[j][0] + c, M[j][1] + 1);
                } else if (k == 2 * threshold) {
                    M[j+1][2 * threshold] = min(M[j][2 * threshold] + c, M[j+1][2 * threshold -1] + 1);
                } else {
                    M[j+1][k] = min(M[j][k] + c, M[j][k+1] + 1, M[j+1][k-1] + 1);
                }
                if (M[j+1][k] < minEd)
                    minEd = M[j+1][k];
            }
            if (minEd > threshold)
                break;
        }
        if (j == erLen && minEd <= threshold) {
            vector<pair<int, int>> ED_end;
            for (int k = 0; k <= 2 * threshold; k++) {
                int ED = M[erLen][k];
                if (ED <= threshold) {
                    int end = left - 1 + erLen - threshold + k;
                    if (end < (int)D.length())
                        ED_end.push_back(make_pair(ED, end));
                }
            }
            entity_EDend[rightStrs[i].second] = ED_end;
        }
    }
    int last = -1;
    j = 0;
    for (int i = 0; i < (int)leftStrs.size(); i++) {
        if (entity_EDend.find(leftStrs[i].second) == entity_EDend.end())
            continue;
        vector<pair<int, int>>& ED_end = entity_EDend[leftStrs[i].second];
        std::sort(ED_end.begin(), ED_end.end());
        int threshold2 = threshold - ED_end[0].first;
        int minEd = threshold2 + 1;
        j = maxCommonSuffixLen(leftStrs, i, last, j);
        last = i;
        int elLen = (int)leftStrs[i].first.length();
        for (; j < elLen; j++) {
            // |v| = j + 1
            for (int k = 0; k <= 2 * threshold2; k++) {
                int c = 1;
                int index = right - j + threshold2 - k;
                if (index <= right && index >= 0) {
                    char v = leftStrs[i].first[elLen - 1 - j];
                    char d = D[index];
                    c = (v == d) ? 0 : 1;
                }
                if (k == 0) {
                    M[j+1][0] = min(M[j][0] + c, M[j][1] + 1);
                } else if (k == 2 * threshold2) {
                    M[j+1][2 * threshold2] = min(M[j][2 * threshold2] + c, M[j+1][2 * threshold2 -1] + 1);
                } else {
                    M[j+1][k] = min(M[j][k] + c, M[j][k+1] + 1, M[j+1][k-1] + 1);
                }
                if (M[j+1][k] < minEd)
                    minEd = M[j+1][k];
            }
            if (minEd > threshold2)
                break;
        }
        if (j == elLen && minEd <= threshold2) {
            for (int k = 0; k <= 2 * threshold2; k++) {
                int ED = M[elLen][k];
                if (ED <= threshold2) {
                    for (auto& p : ED_end) {
                        if (p.first + ED > threshold)
                            break;
                        int start = right + 1 - elLen + threshold2 - k;
                        EDExtractResult res;
                        res.id = leftStrs[i].second;
                        res.pos = start;
                        res.len = p.second - start + 1;
                        res.sim = p.first + ED;
                        result.push_back(res);
                    }
                }
            }
        }
    }

}



int AEE::aeeED(const char *document, unsigned threshold, vector<EDExtractResult> &result) {
    result.clear();
    buildTrie((int)threshold);

    string doc(document);
    int len = (int)doc.length();
    // vector<Node*> candidates;

    // KMP Match
    buildNext();
    printTree(root, 0);
    Node* p = root;
    int i = 0;

    while (i < len - (Lmin - (int)threshold) + 1) {
        if (p->hasChild(doc[i])) {
            p = p->children[doc[i]];
            i ++;
        } else if (p == root) {
            i ++;
        } else {
            p = p->next;
        }
        if (p->isLeaf) {
            // candidates.push_back(p);
            cout << doc.substr(i - p->depth, p->depth) << endl;
            extension(p, doc, i - p->depth - 1, i, (int)threshold, result);
        }
    }

    // Naive Match
    // for (int i = 0; i < len - (Lmin - (int)threshold) + 1; i++) {
    //     Node* p = root;
    //     int j = i;
    //     while (p->hasChild(doc[j])) {
    //         p = p->children[doc[j]];
    //         if (p->isLeaf) {
    //             candidates.push_back(p);
    //             cmaout << doc.substr(i, j-i+1) << endl;
    //         }
    //         j++;
    //     }
    // }

    return SUCCESS;
}
