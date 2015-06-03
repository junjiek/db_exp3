#ifndef __EXP3__AEE_H__
#define __EXP3__AEE_H__

#include <vector>
#include <string>
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

class AEE {
    std::vector<std::string> entities;
    int Lmin, Lmax;
    Node* root;
    void buildTrie(const int threshold);
    Node* insert(Node* parent, string& s, int pos);
    void printTree(Node* p, int depth);
public:
    AEE();
    ~AEE();
    int createIndex(const char *entity_file_name);
    int aeeJaccard(const char *document, double threshold, std::vector<JaccardExtractResult> &result);
    int aeeED(const char *document, unsigned threshold, std::vector<EDExtractResult> &result);
};

#endif
