//
// Created by Tanki on 2017/1/1.
//
#include <iostream>
#include <time.h>
#include <sstream>
#include <random>
#include <unordered_map>
#include "dag.cpp"
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
int _main(){
  FILE *fp;
  if((fp=fopen("resource.txt","w"))==NULL) exit(1);
  std::cout << "Hello, World!" << std::endl;
  // auto dag = new DAG();
  random_device rd;
  double max = rd.max();
  double min = rd.min();
  for (int64_t i = 0; i < 2 * 0x7FFFF; ++i) {
    fprintf(fp, "%lld %lld\n", int64_t((max - rd()) / (max - min) * 50000), int64_t((max - rd()) / (max - min) * 50000));
    // dag->connect(*(Asset::get(int2str(i))), *(Asset::get(int2str(i+1))));
    // dag->connect(*(Asset::get(int2str(i))), *(Asset::get(int2str(i+2))));
  }
//  for (int i = 0; i < 10000; ++i) {
//    dag->connect(*(Asset::get(int2str(i))), *(Asset::get(int2str(i-2))));
//  }
//  std::cout << dag->toString() << std::endl;
  return 0;
}

int main(){
  FILE *fp;
  if((fp=fopen("resource.txt","r"))==NULL) exit(1);
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
  std::cout << dag->toString() << std::endl;
  printf("time spend: %lf", num);
  return 0;
}