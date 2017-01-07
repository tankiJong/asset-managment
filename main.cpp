//
// Created by Tanki on 2017/1/1.
//
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <sstream>
#include <random>
#include "dag.cpp"
#include <gvc.h>
using namespace std;
//
//void storeToMap(hash_map<string, unique_ptr<Asset>> &mp, Asset &asset){
////    if
//}

string int2str(const int &i) {
  char str[64];
  sprintf(str, "%d", i);
  return string(str);
}

string int2str(const int &&i) {
  return int2str(i);
}

// delete or re-add asset
void updateAssetAvailableStatus(string name, bool isAvailable, DAG &dag) {
  auto asset = Asset::setAvailableStatusTo(name, isAvailable);
  dag.updateUsable(*asset);
}

int generateAGraph(){
  FILE *fp;
  if((fp=fopen("resource.txt","w"))==NULL) exit(1);
  std::cout << "Hello, World!" << std::endl;
  // auto dag = new DAG();
  random_device rd;
  double max = rd.max();
  double min = rd.min();
  for (int64_t i = 0; i < 2 * 25; ++i) {
    fprintf(fp, "%lld %lld\n", int64_t((max - rd()) / (max - min) * 10), int64_t((max - rd()) / (max - min) * 10));
    // dag->connect(*(Asset::get(int2str(i))), *(Asset::get(int2str(i+1))));
    // dag->connect(*(Asset::get(int2str(i))), *(Asset::get(int2str(i+2))));
  }
  std::cout << "done" << std::endl;
  fclose(fp);
//  for (int i = 0; i < 10000; ++i) {
//    dag->connect(*(Asset::get(int2str(i))), *(Asset::get(int2str(i-2))));
//  }
//  std::cout << dag->toString() << std::endl;
  return 0;
}

int main(){
  FILE *fp;
  if((fp=fopen("resource.txt","r"))==NULL) {
    std::cout << "open file error" << std::endl;
    exit(1);
  }
  std::cout << "Hello, World!" << std::endl;
  char a[10], b[10];
  auto dag = new DAG();
  time_t time1,time2;
  time(&time1);
  while(!feof(fp)){
    fscanf(fp, "%s %s", a, b);
    // printf("%s %s\n", a, b);
    // Asset aas(a);
    // Asset bas(b);
    // Asset::get(a);
    // Asset::get(b);
    dag->connect(*(Asset::get(a)), *(Asset::get(b)));
    // dag->connect(*new Asset(a), *new Asset(b));
  }
  time(&time2);
  double num = difftime(time2,time1);
//  for (int i = 0; i < 10000; ++i) {
//    dag->connect(*(Asset::get(int2str(i))), *(Asset::get(int2str(i-2))));
//  }
//  std::cout << "ok\n" << std::endl;
//  auto server = Server::get(4545);
  std::cout << dag->print() << std::endl;
  dag->visualize("foo.png");
  while(scanf("%s", a) != EOF) {
    if(!strcmp(a, "q")) return 0;
    updateAssetAvailableStatus(a, false, *dag);
//    auto asset = Asset::setAvailableStatusTo(a, false);
//    if(asset.get() == nullptr) {
//      printf("asset of such name don't exist.\n");
//      continue;
//    }
//    dag->updateUsable(*asset);
    std::cout << dag->print() << std::endl;
    dag->visualize("foo.png");
  }
//  server.onReceive([](string data){
//      std::cout << "received: " << data << "\n" << std::endl;
//  });
//  std::cout << dag->toJson() << std::endl;

  fclose(fp);
  return 0;
}

#define MIN_PER_RANK 20 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 50
#define MIN_RANKS 3    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 5
#define PERCENT 10     /* Chance of having an Edge.  */

// http://www.graphviz.org/doc/Dot.ref
string generateRandomGraph (void) {
  int i, j, k,nodes = 0;
  srand (time (NULL));

  int ranks = MIN_RANKS
              + (rand () % (MAX_RANKS - MIN_RANKS + 1));

  string re = "digraph {ranksep=5;splines=true;radio=auto; ";
  string foo = "";
  for (i = 0; i < ranks; i++)
  {
    /* New nodes of 'higher' rank than all nodes generated till now.  */
    int new_nodes = MIN_PER_RANK
                    + (rand () % (MAX_PER_RANK - MIN_PER_RANK + 1));

    /* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
    for (j = 0; j < nodes; j++)
      for (k = 0; k < new_nodes; k++)
        if ( (rand () % 100) < PERCENT) {
          char str[100];
          sprintf(str, "  %d -> %d [ shape=\"box\",style=\"filled\",color=\"grey\",arrowhead=\"dot\", arrowsize = 0.5 ];", j, k + nodes);
          re += string(str);  /* An Edge.  */
          char fostr[100];
          sprintf(fostr, "%d %d\n", j, k + nodes);
          foo += fostr;
        }
    nodes += new_nodes; /* Accumulate into old node set.  */
  }
  re += "}\0";
  printf("%s", foo.c_str());
  return re;
}


// http://www.graphviz.org/doc/libguide/libguide.pdf
int testDrawer(){
//  const char *str = graph.c_str();
  string str = generateRandomGraph();
  GVC_t* gvc = gvContext();
  Agraph_t* g = agmemread(str.c_str());
  gvLayout(gvc, g, "twopi");

  gvRenderFilename(gvc, g, "png", "foo.png");
  gvFreeLayout(gvc, g);

  agclose(g);

  return (gvFreeContext(gvc));
}