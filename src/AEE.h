#ifndef __EXP3__AEE_H__
#define __EXP3__AEE_H__

#include <vector>
#include <string>
#include <unordered_set>
#include "Node.h"

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
const long long HASH = 95891;

class AEE {
    std::vector<std::string> entities;
    unordered_set<long long> resultHashSet;
    int Lmin, Lmax;
    unsigned last_threshold;
    int ** M;
    Node* root;
    long long Hash[3];
    void buildTrie(const int threshold);
    Node* insert(string& s);
    void buildNext();
    int maxCommonPrefixLen(std::vector<pair<string, int>>& rightStrs, int cur, int pre, int bound);
    int maxCommonSuffixLen(std::vector<pair<string, int>>& leftStrs, int cur, int pre, int bound);
    void extension(Node* p, string& D, int right, int left, int threshold, vector<EDExtractResult> &result);
    void printTree(Node* p, int depth);
    void printM(int num, int threshold);
    int getEditDistance(const string &A, const string &B, int threshold);

public:
    AEE();
    ~AEE();
    int createIndex(const char *entity_file_name);
    int aeeJaccard(const char *document, double threshold, std::vector<JaccardExtractResult> &result);
    int aeeED(const char *document, unsigned threshold, std::vector<EDExtractResult> &result, bool flag);
};

#endif
