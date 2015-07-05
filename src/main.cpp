#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <ctime>
#include "AEE.h"

int main(int argc, char** argv) {
if (argc == 3) {
    AEE aee;

    vector<EDExtractResult> resultED;
    vector<JaccardExtractResult> resultJaccard;

    unsigned edThreshold = 2;
    // double jaccardThreshold = 0.85;

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
      // printf("DOC: %s\n", s.c_str());
      // clock_t begin = clock();
      int size1, size2;
      aee.aeeED(s.c_str(), edThreshold, resultED, false);
      size1 = (int)resultED.size();
      vector<EDExtractResult>::iterator it;
      printf("%d result found\n", (int)resultED.size());
      for (it  = resultED.begin(); it != resultED.end(); ++it) {
        // printf("id: %d, pos: %d, len: %d, sim: %d\n", it->id, it->pos, it->len, it->sim);
        printf("%2d: %s|%s|\n", it->sim, entities[it->id].c_str(), s.substr(it->pos, it->len).c_str());
      }
      printf("-----------\n");
      aee.aeeED(s.c_str(), edThreshold, resultED, true);
      printf("%d result found\n", (int)resultED.size());
      for (it  = resultED.begin(); it != resultED.end(); ++it) {
        // printf("id: %d, pos: %d, len: %d, sim: %d\n", it->id, it->pos, it->len, it->sim);
        printf("%2d: %s|%s|\n", it->sim, entities[it->id].c_str(), s.substr(it->pos, it->len).c_str());
      }
      size2 = (int)resultED.size();
      if (size1 != size2) {
        printf("size1: %d, size2: %d\n", size1, size2);
        printf("%s\n", s.c_str());
      }
      // clock_t end = clock();
      // printf("%.3lf\n", double((end - begin) / CLOCKS_PER_SEC));
    };

    return 0;
}

while(1) {
      AEE aee;
      vector<EDExtractResult> resultED;
      unsigned edThreshold = 2;

      srand((unsigned)time(0));
      // while(1)
      const char* entityFile = "../data/entity_num";
      const char* docFile = "../data/doc_num";
      ofstream entity(entityFile);
      ofstream doc(docFile);
      for (int i = 0; i < 100; i ++) {
          int len = (rand() % 5) + 5;
          int e = rand() % (len * (int)pow(10, len));
          entity << e << endl;
      }
      entity.close();
      aee.createIndex(entityFile);

     for  (int cnt = 0; cnt < 50; cnt ++) {
          string s = "";
          string tmp;
          for (int i = 0; i < 10; i++) {
              int len = (rand() % 5) + 5;
              int e = rand() % (len * (int)pow(10, len));
              stringstream stream;
              stream << e;
              stream >> tmp;
              s += tmp + " ";
          }
          // cout << s << endl;
          int size1, size2;
          aee.aeeED(s.c_str(), edThreshold, resultED, false);
          size1 = (int)resultED.size();
          aee.aeeED(s.c_str(), edThreshold, resultED, true);
          size2 = (int)resultED.size();
          printf("size1: %d, size2: %d\n", size1, size2);
          if (size1 > size2) {
              printf("*******************\n");
              printf("%s\n", s.c_str());
              doc << s << endl;
              return 0;
          }
    };
}
    return 0;
}
