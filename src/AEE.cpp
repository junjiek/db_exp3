#include "AEE.h"
#include <fstream>
#include <string>
#include <climits>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <queue>

using namespace std;

AEE::AEE() {
    Hash[0] = HASH;
    Hash[1] = HASH * HASH;
    Hash[2] = Hash[0] * Hash[1];
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
    root = NULL;
    M = new int* [Lmax];
    for(int i = 0; i < Lmax; i++)
        M[i] = new int[maxED];
    last_threshold = 2;
    buildTrie(last_threshold);
    // KMP Match
    buildNext();
    // printTree(root, 0);
    return SUCCESS;
}

Node* AEE::insert(string& s) {
    Node* p = root;
    for (int i = 0; i < (int)s.length(); i++) {
        int c = alphaToNum(s[i]);
        Node* & q = p->children[c];
        if (!q) {
            q = new Node();
            q->depth = i+1;
        }
        p = q;
    }
    return p;
}

void AEE::printTree(Node* p, int depth) {
    if (p->isLeaf) {
        p->sort();
        for (auto& left : p->leftStrs)
            cout << "(" << left.first << ","  << left.second << ") ";
        cout << "; ";
        for (auto& right : p->rightStrs)
            cout << "(" << right.first << ","  << right.second << ") ";
        // cout << endl;
    }
    bool first = true;
    for (int c = 0; c < ALPHABET; c++) {
        Node* child = p->children[c];
        if (!child)
            continue;
        for (int i = 0; first == false && i < depth; i++)
            cout << "     ";
        cout << "->" << numToAlpha(c);
        if (child->next != NULL)
            cout << child->next->depth;
        else
            cout << "  ";
        printTree(child, depth+1);
        first = false;
    }
    if (first == true)
        cout << endl;
}

// Build KMP next
void AEE::buildNext() {
    queue<Node*> q;
    root->next = root;
    for (int i = 0; i < ALPHABET; i++) {
        if (root->children[i]) {
            root->children[i]->next = root;
            q.push(root->children[i]);
        }
    }
    while (!q.empty()) {
        Node* p = q.front();
        q.pop();
        for(int i = 0; i < ALPHABET; i++) {
            if (p->children[i]) {
                q.push(p->children[i]);
                Node* tmp = p->next;
                while (tmp != root && tmp->children[i] == NULL)
                    tmp = tmp->next;
                if (tmp->children[i])
                    p->children[i]->next = tmp->children[i];
                else
                    p->children[i]->next = root;
            }
        }
    }
}

void AEE::buildTrie(const int threshold) {
    root = new Node();
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
    while (i < bound && pre >= 0) {
        int len1 = (int)leftStrs[cur].first.length();
        int len2 = (int)leftStrs[pre].first.length();
        if (leftStrs[cur].first[len1 - 1 - i] == leftStrs[pre].first[len2 - 1 - i])
            i ++;
        else
            break;
    }
    return i;
}

void AEE::printM(int num, int threshold) {
    cout << num << ": ";
    for (int i = 0; i <= 2 * threshold; i++) {
        cout << M[num][i] << " "; 
    }
    cout << endl;
}

void AEE::extension(Node* p, string& D, int left, int right, int threshold, vector<EDExtractResult> &result) {
    vector<pair<string, int>>& leftStrs = p->leftStrs;
    vector<pair<string, int>>& rightStrs = p->rightStrs;
    for (int k = 0; k <= 2*threshold; k++) {
        M[0][k] = k - threshold > 0 ? k - threshold : threshold - k;
    }
    p->sort();
    int j = 0;

    unordered_map<int, vector<pair<int, int>>> entity_EDend;
    // printM(0, threshold);
    for (int i = 0; i < (int)rightStrs.size(); i++) {
        int erLen = (int)rightStrs[i].first.length();
        j = maxCommonPrefixLen(rightStrs, i, i-1, j);
        bool valid = true;
        for (; j < erLen; j++) {
            // |v| = j + 1
            // cout << j << ": " << endl;
            int minEd = threshold + 1;
            for (int k = 0; k <= 2*threshold; k++) {
                int c = 1;
                int index = right + j - threshold + k;
                if (index >= right && index < (int)D.length()) {
                    char v = rightStrs[i].first[j];
                    char d = D[index];
                    c = (v == d) ? 0 : 1;
                    // printf("  v: %c, d: %c\n", v, d);
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
            if (minEd > threshold) {
                valid = false;
                break;
            }
        }
        if (valid) {
            vector<pair<int, int>> ED_end;
            // printM(erLen, threshold);
            for (int k = 0; k <= 2 * threshold; k++) {
                int ED = M[erLen][k];
                if (ED <= threshold) {
                    int end = right - 1 + erLen - threshold + k;
                    if (end >= right - 1 && end < (int)D.length()) {
                        // printf("r: id:%d (%s|%s) ed:%d\n",  rightStrs[i].second,
                                // rightStrs[i].first.c_str(), D.substr(right, end - right + 1).c_str(), ED);
                        ED_end.push_back(make_pair(ED, end));
                    }
                }
            }
            entity_EDend[rightStrs[i].second] = ED_end;
        }
    }
    // cout << "left: " << endl;
    int last = -1;
    j = 0;
    for (int i = 0; i < (int)leftStrs.size(); i++) {
        if (entity_EDend.find(leftStrs[i].second) == entity_EDend.end())
            continue;
        vector<pair<int, int>>& ED_end = entity_EDend[leftStrs[i].second];
        std::sort(ED_end.begin(), ED_end.end());
        // int threshold2 = threshold - ED_end[0].first;
        int threshold2 = threshold;
        j = maxCommonSuffixLen(leftStrs, i, last, j);
        last = i;
        int elLen = (int)leftStrs[i].first.length();
        bool valid = true;
        for (; j < elLen; j++) {
            // |v| = j + 1
            // printM(j, threshold2);
            int minEd = threshold2 + 1;
            for (int k = 0; k <= 2 * threshold2; k++) {
                int c = 1;
                int index = left - j + threshold2 - k;
                if (index <= left && index >= 0) {
                    char v = leftStrs[i].first[elLen - 1 - j];
                    char d = D[index];
                    c = (v == d) ? 0 : 1;
                    // cout << "v: " << v << ", d: " << d << endl;
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
            if (minEd > threshold2) {
                valid = false;
                break;
            }
        }
        // printf("j:%d, elLen:%d\n", j, elLen);
        if (valid) {
            // printM(elLen, threshold2);
            for (int k = 0; k <= 2 * threshold2; k++) {
                int ED = M[elLen][k];
                if (ED <= threshold2) {
                    for (auto& pair : ED_end) {
                        if (pair.first + ED > threshold)
                            break;

                        int start = left + 1 - elLen + threshold2 - k;
                        if (start >= 0 && start <= left + 1) {
                            // printf("start: %d, left: %d\n", start, left);
                            EDExtractResult res;
                            res.id = leftStrs[i].second;
                            res.pos = start;
                            res.len = pair.second - start + 1;
                            long long HashCode = res.id * Hash[0] + res.pos * Hash[1] + res.len * Hash[2];
                            if (resultHashSet.find(HashCode) == resultHashSet.end()) {
                                int realED = getEditDistance(entities[leftStrs[i].second], D.substr(res.pos, res.len), threshold);
                                // if (realED != pair.first + ED) {
                                //     cout << "****" << endl;
                                //     int elLen = leftStrs[i].first.length();
                                //     int emLen = p->depth;
                                //     int erLen = entities[leftStrs[i].second].length() - emLen - elLen;
                                //     cout << entities[leftStrs[i].second] << "|" << D.substr(res.pos, res.len) << "|" << realED << endl;
                                //     cout << "l:|" << leftStrs[i].first << "|" << D.substr(res.pos, left - res.pos + 1) << "|" << pair.first << endl;
                                //     cout << "r:|" << entities[leftStrs[i].second].substr(elLen + emLen, erLen) << "|" << D.substr(right, pair.second - right + 1) << "|" << ED << endl;
                                //     exit(0);
                                // }

                                if (realED <= threshold) {
                                    res.sim = realED;
                                    resultHashSet.insert(HashCode);
                                    result.push_back(res);
                                }
                            }
                            // res.sim = pair.first + ED;
                            // int leftED = getEditDistance(leftStrs[i].first, D.substr(res.pos, left - res.pos + 1), threshold);
                            // if (realED != pair.first + ED) {
                                // cout << "****" << endl;
                                // int elLen = leftStrs[i].first.length();
                                // int emLen = p->depth;
                                // int erLen = entities[leftStrs[i].second].length() - emLen - elLen;
                                // int rightED = getEditDistance(entities[leftStrs[i].second].substr(elLen + emLen, erLen), D.substr(right, pair.second - right + 1), threshold);
                                // if (erLen == pair.second - right + 1 + 2) {
                                    // cout << rightED << endl;
                                    // cout << "****" << endl;
                                    // exit(0);
                                // }
                            // }
                            // printf("l: id:%d (%s|%s) ed:%d\n",  res.id, leftStrs[i].first.c_str(),
                            //     D.substr(start, left - start + 1).c_str(), ED);
                            // cout << "res: " << start << "," << pair.second << " " << D.substr(start, res.len) << endl;
                        }
                    }
                }
            }
        }
    }

}

int AEE::getEditDistance(const string &A, const string &B, int threshold) {
    // cout << A << " " << B << endl;
    int n = (int)A.length();
    int m = (int)B.length();
    if (abs(n - m) > threshold) return threshold + 1;
    int V[2][threshold * 2 + 3];
    memset(V, 0x80, sizeof(V));

    V[1][threshold] = 0;
    for (int p = 0; p <= threshold; ++p) {
        int *f = V[p & 1];
        int *g = V[!(p & 1)];
        for (int k = threshold + 1 - p; k <= threshold + 1 + p; ++k) {
            f[k] = max(max(g[k], g[k + 1]) + 1, g[k - 1]);
            int &t = f[k];
            int d = k - threshold - 1;
            if (t >= 0 && t + d >= 0)
                while (t < n && t + d < m && A[t] == B[t + d]) ++t; 
        }
        if (f[m - n + threshold + 1] >= n) return p;
    }
    return threshold + 1;
}

bool resultCmpED(const EDExtractResult& res1, const EDExtractResult& res2) {
    if (res1.id != res2.id)
        return res1.id < res2.id;
    if (res1.pos != res2.pos)
        return res1.pos < res2.pos;
    return res1.len < res2.len;
}

bool resultEqualED(const EDExtractResult &a, const EDExtractResult &b) {
    return a.id == b.id && a.pos == b.pos && a.len == b.len;
}

int AEE::aeeED(const char *document, unsigned threshold, vector<EDExtractResult> &result, bool flag) {
    result.clear();
    resultHashSet.clear();
    if (threshold != last_threshold) {
        if (root != NULL)
            delete root;
        buildTrie((int)threshold);
        last_threshold = threshold;
        // KMP Match
        buildNext();
        // printTree(root, 0);
    }

    string doc(document);
    int len = (int)doc.length();

    if (flag) {
        Node* p = root;
        int i = 0;
        while (i < len) {
            int c = alphaToNum(doc[i]);
            if (p->children[c]) {
                i ++;
                p = p->children[c];
            } else if (p == root) {
                i ++;
            } else {
                p = p->next;
            }
            if (p->isLeaf) {
                // cout << "+ " << doc.substr(i - p->depth, p->depth) << endl;
                extension(p, doc, i - p->depth - 1, i, (int)threshold, result);
                if (i == len || p->children[alphaToNum(doc[i])] == NULL) {
                    i = i - p->depth + 1;
                    p = root;
                }
                // cout << "out" << endl;
            }
        }

    } else {
        // Naive Match
        for (int i = 0; i <= len - (Lmin - (int)threshold) + 1; i++) {
            int ci = alphaToNum(doc[i]);
            // if (ci < 0) continue;
            Node* p = root->children[ci];
            int j = i;
            while (p) {
                if (p->isLeaf) {
                    // cout << "+ " << doc.substr(i, j-i+1) << endl;
                    extension(p, doc, i - 1, j + 1, (int)threshold, result);

                }
                j++;
                int cj = alphaToNum(doc[j]);
                // if (cj > 0)
                    p = p->children[cj];
                // else
                    // break;
            }
        }

    }

    sort(result.begin(), result.end(), resultCmpED);
    return SUCCESS;
}
