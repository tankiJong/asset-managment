//
// Created by Tanki on 2017/1/1.
//
#include <stdio.h>
#include <string.h>
#include <iostream>
//#include <time.h>
#include <sstream>
#include <random>
#include "dag.cpp"
#include <vector>
#include <gvc.h>
#include <math.h>
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

int exportToFile(const char* str){
  FILE *fp;
  if((fp=fopen("resource.txt","w"))==NULL) exit(1);
  fprintf(fp, "%s", str);
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
  char a[100], b[100];
  time_t time1,time2;
  time(&time1);
  auto toProcess = vector<pair<string, string>>();
  while(!feof(fp)){
    fscanf(fp, "%s %s", a, b);
    toProcess.push_back({a , b});
  }
//  auto dag = new DAG(10000);
  auto dag = new DAG(sqrt(toProcess.size()));
  Asset::reserve(toProcess.size());
  for(auto &p: toProcess) {
    dag->connect(*(Asset::get(p.first)), *(Asset::get(p.second)));
//    Asset::get(p.first);
//    Asset::get(p.second);
  }
  time(&time2);
  double num = difftime(time2,time1);
  time(&time1);
  std::cout << "ok " << num << "\n " << toProcess.size() << std::endl;
  std::cout << dag->out() << std::endl;
  std::cout << "ok " << num << "\n " << toProcess.size() << std::endl;
  time(&time2);
  num = difftime(time2,time1);
  std::cout << "ok " << num << "\n " << toProcess.size() << std::endl;
  dag->visualize("foo.svg");
  std::cout << "svg ok" << std::endl;
  while(scanf("%s %s", a, b) != EOF) {
    if(!strcmp(a, "q")) return 0;
    if(!strcmp(a, "enable")) {
      updateAssetAvailableStatus(b, true, *dag);
    } else if(!strcmp(a, "disable")) {
      updateAssetAvailableStatus(b, false, *dag);
    }
    std::cout << dag->out() << std::endl;
//     dag->visualize("foo.png");
//     std::cout << "=> export to foo.png" << std::endl;
  }

  fclose(fp);
  return 0;
}

#define MIN_PER_RANK 100 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 100
#define MIN_RANKS 100    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 100
#define PERCENT 1     /* Chance of having an Edge.  */

// http://www.graphviz.org/doc/Dot.ref
string generateRandomGraph (void) {
  int i, j, k,nodes = 0;
  srand (time (NULL));

  int ranks = MIN_RANKS
              + (rand () % (MAX_RANKS - MIN_RANKS + 1));

  string re = "digraph {ranksep=8;splines=true;radio=auto; ";
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
  exportToFile(foo.c_str());
  printf("%s", foo.c_str());
  return re;
}


// http://www.graphviz.org/doc/libguide/libguide.pdf
int $main(){
//  const char *str = graph.c_str();
  string str = generateRandomGraph();
  GVC_t* gvc = gvContext();
  Agraph_t* g = agmemread(str.c_str());
  gvLayout(gvc, g, "twopi");

  gvRenderFilename(gvc, g, "svg", "foo.svg");
  gvFreeLayout(gvc, g);

  agclose(g);

  return (gvFreeContext(gvc));
}