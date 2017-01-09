//
// Created by Tanki on 2017/1/1.
//
#include <memory>
#include <unordered_set>
#include <queue>
#include <string>
#include <set>
#include <thread>
#include "asset.cpp"
#include <gvc.h>
// Among the dependency of assets, it's impossible to have circle. So DAG should be proper way to store such structure.
using namespace std;

using ll = int64_t;
static char print[104857600];
class DAG {
public:
    // the node type will used for list, so override several type of operations is a must.
    class Node{
    private:
        mutable bool usable;
    public:
        Node(const Node&&) = delete;
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        struct nodeHash {
            std::hash<std::string> h;
            size_t operator()(const Node *node) const
            {
              return h(node->getName());
            }
        };
        struct nodeEqual {
            bool operator()(const Node* a, const Node* b) const { return *a == *b; }
        };
        Asset& value;
        void evaluateUsableWhenChange(const Node &node) const {
          this->updateUsable(usable && node.usable);
        }
        void updateUsable(bool canUse) const {
          if(canUse == usable) return;
          // dfs update usable
//          auto nodePair = ;
//          [this]{ this->usable = this->usable && canUse; return this; };
          auto toRefresh = queue<pair<const Node&, bool>>();
          toRefresh.push({ *this, canUse });
          while(toRefresh.size() != 0) {
            auto &top = toRefresh.front();
            auto node = top;
            toRefresh.pop();
            if(node.first.usable) {
              node.first.usable = node.first.usable && node.first.value.isAvailable() && node.second;
            } else if(!node.first.usable) {
              // if usable is false, it is too strong to reverse it, so have no idea but to detect all nodes
              bool isUsable = true;
              for(auto &cc: *node.first.connecting) isUsable = cc->usable && cc->value.isAvailable() && isUsable;
              node.first.usable = isUsable;
            }
//            printf("%s -> %s\n", node.first.getName().c_str(), (node.first.usable && node.second) ? "✅" : "❌");
            for(auto toAdd : *node.first.connected) {
              toRefresh.push({*toAdd, node.first.usable});
            }
          }
        }
        // it is connecting to other Nodes    this -> other
        shared_ptr<unordered_set<Node*, nodeHash, nodeEqual>> connecting;
        // it is connected to other Nodes     this <- other
        shared_ptr<unordered_set<Node*, nodeHash, nodeEqual>> connected;
        Node(Asset& asset, bool isFack = false): value(asset) {
          if(!isFack) {
            this->connecting = make_shared<unordered_set<Node*, nodeHash, nodeEqual>>();
            this->connected = make_shared<unordered_set<Node*, nodeHash, nodeEqual>>();
          } else {
//            this->connecting = shared_ptr<unordered_set<Node*, nodeHash, nodeEqual>>(nullptr);
//            this->connected = shared_ptr<unordered_set<Node*, nodeHash, nodeEqual>>(nullptr);
          }
          this->usable = this->value.isAvailable();
        }

        void reset() {
          this->connecting = make_shared<unordered_set<Node*, nodeHash, nodeEqual>>();
          this->connected = make_shared<unordered_set<Node*, nodeHash, nodeEqual>>();
        }
        Node(Asset& asset, ll expect): value(asset) {
          this->connecting = make_shared<unordered_set<Node*, nodeHash, nodeEqual>>(expect);
          this->connected = make_shared<unordered_set<Node*, nodeHash, nodeEqual>>(expect);
          this->usable = this->value.isAvailable();
        }
        inline bool operator == (const Node &node) const{
          return this->value == node.value;
        }
        string getName(bool raw = true) const {
          if(raw || this->usable) return this->value.getName();
          return "\033[31m" + this->value.getName() + "\033[0m";
        }
        string toJson() const {
          char json[100];
          sprintf(json, "{\"id\":\"%s\"}", this->getName().c_str());
          return json;
        }
        inline bool isUsable() const {
          return this->usable;
        }
    };
    DAG(ll expect): count(0) {
      this->expect = expect;
      this->nodes = make_unique<unordered_set<Node*, Node::nodeHash, Node::nodeEqual>>(expect);
      this->drawThread = thread([this] { this->_visualize();});
    }

    DAG(shared_ptr<Asset> &asset, ll expect) {
      auto node = new Node(*asset, expect);;
      this->nodes = make_unique<unordered_set<Node*, Node::nodeHash, Node::nodeEqual>>(expect);
      this->nodes->insert(node);
      this->count = 1;
      this->drawThread = thread([this] { this->_visualize();});
    }
    void connect(Asset &fromAsset, Asset &toAsset, bool evaluate = false) {
      auto source = new Node(fromAsset, true);
      auto dist = new Node(toAsset, true);
      auto from = this->nodes->find(source);
      auto to = this->nodes->find(dist);
      if(from == this->nodes->end()) {
        source->reset();
        this->nodes->insert(source);
      } else {
        delete source;
        source = *from;
      }
      if(to == this->nodes->end()) {
        dist->reset();
        this->nodes->insert(dist);
      } else {
        delete dist;
        dist = *to;
      }
      source->connecting->insert(dist);
      dist->connected->insert(source);
//      if(!evaluate) return;
//      source->evaluateUsableWhenChange(*dist);
    }
    string out(){
      time_t time1,time2;
      time(&time1);
      set<string> sortedLines;
      ll totalSize = 0;
      for(auto head : *this->nodes) {
        string line = "[";
        line += head->getName(false);
        line += "] \t";
        for(auto node: *head->connecting){
          line += node->getName(false);
          line += "  ";
        }
        line += "\n";
//        print += line;
        sortedLines.insert(line);
        totalSize += line.length();
      }
      time(&time2);
      double num = difftime(time2,time1);
//      char *print = new char[totalSize+100];
//      print[0]='\0';
      printf("print time: %lf\n", num);
      for(auto &line : sortedLines) {
        printf("%s", line.c_str());
        // strcat(print, line.c_str());
      }
      return "";
    };
    string* toJson(){
      auto print = new string("{");
      for(auto head : *this->nodes) {
        string line = "\"";
        line += head->getName();
        line += "\":[";
        for(auto node: *head->connecting){
          line += node->toJson() + ",";
        }
        line += "],\n";
        *print += line;
      }
      *print += "}";
      return print;
    }
    void visualize(string filename) {
      this->toDraw = true;
      this->toDrawName = filename;
    }
    bool remove(Asset &asset) {
      Node *$node = nullptr;
      bool flag = false;
      for(auto node : *this->nodes) {
        if(node->value == asset) {
          $node = node;
          flag = true;
          this->nodes->erase(node);
        }
        if($node != nullptr) {
          node->connecting->erase($node);
        }
      }
      return flag;
    }
    void updateUsable(Asset &asset) {
      for(auto node : *this->nodes) {
        if(node->value == asset) {
          node->updateUsable(asset.isAvailable());
          return;
        }
      }
    }
protected:
    ll expect;
    ll count;
    bool toDraw = false;
    string toDrawName;
    Node* fakeSourceNode = nullptr;
    Node* fakeDistNode = nullptr;
    thread drawThread;
    unique_ptr<unordered_set<Node*, Node::nodeHash, Node::nodeEqual>> nodes;
    int _visualize() {
      while(1) {
        string filename = this->toDrawName;
        if (!this->toDraw) continue;
        this->toDraw = false;
        FILE *fp;
        if ((fp = fopen("export.txt", "w")) == NULL) exit(1);
        fprintf(fp, "%s", "digraph {\nranksep=5;splines=true;radio=auto; ");
        for (auto head : *this->nodes) {
          if (!head->isUsable()) {
            fprintf(fp, "\"%s\" [style=\"filled\" color=\"red\" ];\n", head->getName().c_str());
          }

          for (auto node: *head->connecting) {
            if (node->isUsable()) {
              fprintf(fp, "\"%s\" -> \"%s\" [color=\"grey\",arrowhead=\"dot\", arrowsize = 0.5 ];\n",
                      head->getName().c_str(), node->getName().c_str());
            } else {
              fprintf(fp, "\"%s\" -> \"%s\" [color=\"red\",arrowhead=\"dot\", arrowsize = 0.5 ];\n",
                      head->getName().c_str(), node->getName().c_str());
            }
          }
        }
        fprintf(fp, "%s", "}");
        fclose(fp);
        if ((fp = fopen("export.txt", "r")) == NULL) exit(1);
        GVC_t *gvc = gvContext();
#ifdef WITH_CGRAPH
        Agraph_t *g = agread(fp, 0);
#else
        Agraph_t* g = agread(fp);
#endif
        gvLayout(gvc, g, "twopi");
        gvRenderFilename(gvc, g, "svg", filename.c_str());
        gvFreeLayout(gvc, g);
        agclose(g);
        fclose(fp);
        std::cout << "=> export to foo.svg" << std::endl;
      }
    }
};