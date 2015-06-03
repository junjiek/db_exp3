#ifndef __EXP3__AEE_H__
#define __EXP3__AEE_H__
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <cstring>
#include <cmath>

using namespace std;

template <typename EntityIDType, typename PosType, typename LenType, typename SimType>
struct ExtractResult {
    EntityIDType id;
    PosType pos;
    LenType len;
    SimType sim;
};
typedef ExtractResult<unsigned, unsigned, unsigned, unsigned> EDExtractResult;
typedef ExtractResult<unsigned, unsigned, unsigned, double> JaccardExtractResult;

const int SUCCESS = 0;
const int FAILURE = 1;

class AEE {
    int q;
    int maxLen;
    vector<string> entities;
    unordered_map<string, vector<int>> inverted_lists;
    vector<vector<int>> positionLists;
    int binaryShift(int i ,int j, int tl, int te, vector<int>& positionList);
    void binarySpan(int i , int j, int te, int dte, vector<int>& positionList,int id, 
                    int entirylen,std::vector<EDExtractResult> &candidate);
    unsigned calDistED(string &a, string &b, unsigned threshold,
                       vector<int> &d0, vector<int> &d1);
public:
    AEE();
    ~AEE();

    int createIndex(const char *filename);
    int aeeJaccard(const char *doc, double threshold,
                   std::vector<JaccardExtractResult> &result);
    int aeeED(const char *doc, unsigned threshold,
              std::vector<EDExtractResult> &result);
};

#endif
