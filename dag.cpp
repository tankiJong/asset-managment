//
// Created by Tanki on 2017/1/1.
//
#include <memory>
#include <unordered_set>
#include <stack>
#include <functional>
#include <string>
#include <set>
#include "asset.cpp"
#include <gvc.h>
// Among the dependency of assets, it's impossible to have circle. So DAG should be proper way to store such structure.
using namespace std;

using ll = int64_t;

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
          auto toRefresh = stack<pair<const Node&, bool>>();
          toRefresh.push({ *this, canUse });
          while(toRefresh.size() != 0) {
            auto &top = toRefresh.top();
            auto node = top;
            toRefresh.pop();
            printf("%s -> %s\n", node.first.getName().c_str(), (node.first.usable && node.second) ? "✅" : "❌");
            node.first.usable = node.first.usable && node.second;
            for(auto toAdd : *node.first.connected) {
              toRefresh.push({*toAdd, node.first.usable});
            }
          }
        }
        // it is connecting to other Nodes    this -> other
        shared_ptr<unordered_set<Node*, nodeHash, nodeEqual>> connecting;
        // it is connected to other Nodes     this <- other
        shared_ptr<unordered_set<Node*, nodeHash, nodeEqual>> connected;
        Node(Asset& asset): value(asset) {
          this->connecting = make_shared<unordered_set<Node*, nodeHash, nodeEqual>>();
          this->connected = make_shared<unordered_set<Node*, nodeHash, nodeEqual>>();
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
    DAG(): count(0) {
      this->nodes = make_unique<unordered_set<Node*, Node::nodeHash, Node::nodeEqual>>(100);
    }

    DAG(shared_ptr<Asset> &asset) {
      auto node = new Node(*asset);;
      this->nodes = make_unique<unordered_set<Node*, Node::nodeHash, Node::nodeEqual>>(100);
      this->nodes->insert(node);
      this->count = 1;
    }
    void connect(Asset &fromAsset, Asset &toAsset) {
      auto source = new Node(fromAsset);
      auto dist = new Node(toAsset);
      auto from = this->nodes->find(source);
      auto to = this->nodes->find(dist);
      if(from == this->nodes->end()) {
        this->nodes->insert(source);
      } else {
        delete source;
        source = *from;
      }
      if(to == this->nodes->end()) {
        this->nodes->insert(dist);
      } else {
        delete dist;
        dist = *to;
      }
      source->connecting->insert(dist);
      dist->connected->insert(source);
      source->evaluateUsableWhenChange(*dist);
    }
    string print(){
      string print = "";
      set<string> sortedLines;
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
      }
      for(auto &line : sortedLines) {
        print = line + print;
      }
      return print;
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
    int visualize(string filename) {
      string print = "digraph {ranksep=5;splines=true;radio=auto; ";
      set<string> sortedLines;
      for(auto head : *this->nodes) {
        string line = "";
        string from = "\"";
        from += head->getName();
        from += "\"-> ";
        if(!head->isUsable()){
          line += "\"" + head->getName() + "\" " + "[style=\"filled\" color=\"red\" ];";
        }
        for(auto node: *head->connecting){
          string to = from + "\"";
          to += node->getName();
          to += "\" ";
          if(node->isUsable()) {
            to += "[color=\"grey\",arrowhead=\"dot\", arrowsize = 0.5 ];";
          } else {
            to += "[color=\"red\",arrowhead=\"dot\", arrowsize = 0.5 ];";
          }
          line += to;
        }
//        print += line;
        sortedLines.insert(line);
      }
      for(auto &line : sortedLines) {
        print += line;
      }
      print += "}";
      GVC_t* gvc = gvContext();
      Agraph_t* g = agmemread(print.c_str());
      gvLayout(gvc, g, "twopi");
      gvRenderFilename(gvc, g, "png", filename.c_str());
      gvFreeLayout(gvc, g);
      agclose(g);
      return (gvFreeContext(gvc));
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
    ll count;
    unique_ptr<unordered_set<Node*, Node::nodeHash, Node::nodeEqual>> nodes;
};