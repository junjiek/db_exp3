#include "AEE.h"

using namespace std;

AEE::AEE() {
}

AEE::~AEE() {
}

int AEE::createIndex(const char *filename) {
    q = 3;
    entities.clear();
    ifstream fin(filename);
    int lineNum = 0;
    string s;
    maxLen = -1;
    while (getline(fin, s)) {
            entities.push_back(s);
            int len = int(s.length()) - q;
            if ((int)s.length() > maxLen)
                maxLen = (int)s.length();
            vector<int> vl(1001);
            positionLists.push_back(vl);    
        for ( int i = 0; i <= len; i++ ) {
            string sub = s.substr(i, q); 
            if(inverted_lists.find(sub) == inverted_lists.end()){
                 vector<int> entity_list;
                 inverted_lists[sub] = entity_list;
            }
            inverted_lists[sub].push_back(lineNum);
        }   
        lineNum++;
        // cout << s << endl;
    }
    if (inverted_lists.empty())
        return FAILURE;
    return SUCCESS;
}

int AEE::aeeJaccard(const char *doc, double threshold, vector<JaccardExtractResult> &result) {
    return SUCCESS;
}

int AEE::binaryShift(int i ,int j, int Tl, int Te, vector<int>& positionList) {
    int lower = i, upper = j;
    while (lower <= upper) {
        int mid = (lower + upper + 1) >> 1;
        if (positionList[j] +(mid-i)-positionList[mid]+1 > Te)
            lower = mid + 1;
        else
            upper = mid - 1;
    }
    i = lower, j = i + Tl - 1;
    if (j < int(positionList.size()) && positionList[j] - positionList[i] + 1 > Te) {
        i = binaryShift(i, j, Tl, Te, positionList);
    } else {
        return i;
    }
}
void AEE::binarySpan(int i , int j, int Te, int dte, vector<int>& positionList,int id, 
                    int entitylen, std::vector<EDExtractResult> &candidate) {

    int lower = j, upper = min(i + Te - 1, int(positionList.size()) - 1);
    int mid;
    while (lower <= upper) {
        mid = (lower + upper + 1) >> 1;
        if (positionList[mid] - positionList[i] + 1 > Te) {
            upper = mid - 1;
        } else {
            lower = mid + 1;
        }
    }
    mid = upper;
    // Find Candidate Window in D[i...mid]
    int left = 0;
    if (i > 0) 
        left = max(left, positionList[i-1] + 1);
    EDExtractResult can;
    for (int start = left; start <= positionList[i]; start++) {
        int minv = start + dte -1;
        int maxv = min(start + Te-1, entitylen - q);
        for (int v = minv; v <= maxv; v++) {
            can.id = id;
            can.pos = start;
            can.len = v + q - start;
            can.sim = mid - i + 1;
            candidate.push_back(can);
        }
    }
}


unsigned AEE::calDistED(string &a, string &b, unsigned threshold,
                              vector<int> &d0, vector<int> &d1) {
    unsigned dis = threshold + 1;
    int len_a = a.length(), len_b = b.length();
    if(abs(len_a - len_b) > threshold)
        return dis;
    for(int i = 0; i <= len_a; ++ i) {
        int l = max(0, i - (int)threshold), r = min(len_b, i + (int)threshold);
        int minDis = threshold + 1;
        for(int j = l; j <= r; ++ j) {
            if(i == 0)
                d1[j] = j;
            else if(j == 0)
                d1[j] = i;
            else {
                if(a[i - 1] == b[j - 1])
                    d1[j] = d0[j - 1];
                else
                    d1[j] = d0[j - 1] + 1;
                if(j > l) d1[j] = min(d1[j], d1[j - 1] + 1);
                if(j < i + (int)threshold) d1[j] = min(d1[j], d0[j] + 1);
            }
            minDis = min(minDis, d1[j]);    
        }
        if(minDis > (int)threshold)
            return dis;
        swap(d0, d1);
    }
    dis = d0[len_b];
    return dis;
}

int AEE::aeeED(const char *doc, unsigned threshold, vector<EDExtractResult> &result) {
    result.clear();
    string s(doc);
    for (auto & sl : positionLists)
        sl.clear();
    int len = s.length();
    for (int pos = 0; pos <= len-q; pos++) {
        string sub = s.substr(pos, q);
        if (inverted_lists.find(sub) != inverted_lists.end()) {
            vector<int>& v = inverted_lists[sub];
            for (int i = 0; i < (int)v.size(); i++) {
             positionLists[v[i]].push_back(pos);
            }
        }
    }
    std::vector<EDExtractResult> candidate;
    int index = 0;
    for(auto & positionList : positionLists) {
        if(positionLists.empty())
            continue;
        int e = int(entities[index].length()) - q + 1;
        int Tl = max(e - int(threshold)*q, 1); // TODO: fix
        int Te = max(e + int(threshold), 0);
        int dte = max(e - int(threshold) , 0); 
        if ((int)positionList.size() >= Tl) {
            int i = 0; 
            while (i <= (int)positionList.size() - Tl) {
                int j = i + Tl - 1;
                // cout << "j: " << j << ", i: " << i << ", Tl: " << Tl << endl;
                // cout << positionList[j] - positionList[i] + 1 << ", " << Te << endl;
                if (positionList[j] - positionList[i] + 1 <= Te) {
                    //binarySpan
                    binarySpan(i , j, Te, dte, positionList,index, len, candidate);
                    i++;
                } else {
                    //binaryshift
                    i = binaryShift(i, j, Tl, Te, positionList);
                }
            }
        }
        index ++;
    }
    EDExtractResult oner;
    cout << candidate.size() << endl;
    string sub_doc;
    vector<int> d0(maxLen + threshold), d1(maxLen + threshold);
    for (auto & can :candidate) {
        sub_doc = s.substr(can.pos, can.len);
        int distance = calDistED(sub_doc, entities[can.id], threshold, d0, d1);
        if (distance <= (int)threshold) {
            oner.id = can.id;
            oner.pos = can.pos;
            oner.len = can.len;
            oner.sim = distance;
            result.push_back(oner);
        }
    }
    return SUCCESS;
}

