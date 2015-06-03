#include "AEE.h"
#include <fstream>
#include <ctime>

using namespace std;

int main(int argc, char **argv) {
  AEE aee;

  vector<EDExtractResult> resultED;
  vector<JaccardExtractResult> resultJaccard;

  unsigned q = 3, edThreshold = 2;
  double jaccardThreshold = 0.85;

  ifstream fin1(argv[1]);
  vector<string> entities;
  string entity;
  while (getline(fin1, entity)) {
    entities.push_back(entity);
  }
  aee.createIndex(argv[1]);
  ifstream fin(argv[2]);
  string s;
  while (getline(fin, s)) {
    printf("DOC: %s\n", s.c_str());
    clock_t begin = clock();
    aee.aeeED(s.c_str(), edThreshold, resultED);
    clock_t end = clock();
    printf("%.3lf\n", (end - begin) / CLOCKS_PER_SEC);
    vector<EDExtractResult>::iterator it;
    for (it  = resultED.begin(); it != resultED.end(); ++it) {
      // printf("id: %d, pos: %d, len: %d, sim: %d\n", it->id, it->pos, it->len, it->sim);
      printf("%2d: %s|%s|\n", it->sim, entities[it->id].c_str(), s.substr(it->pos, it->len).c_str());
    }
  };

  return 0;
}

