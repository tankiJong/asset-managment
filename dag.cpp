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
        struct nodeHash {
            std::hash<std::string> h;
            size_t operator()(const Node &node) const
            {
              return h(node.getName());
            }
        };
        struct nodeEqual {
            bool operator()(const Node& a, const Node& b) const { return a == b; }
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
            auto &node = toRefresh.top();
            toRefresh.pop();
            node.first.usable = node.first.usable && node.second;
            for(auto &toAdd : *node.first.connected) {
              toRefresh.push({toAdd, node.first.usable});
            }
          }
        }
        // it is connecting to other Nodes    this -> other
        shared_ptr<unordered_set<Node, nodeHash, nodeEqual>> connecting;
        // it is connected to other Nodes     this <- other
        shared_ptr<unordered_set<Node, nodeHash, nodeEqual>> connected;
        Node(Asset& asset): value(asset) {
          this->connecting = make_shared<unordered_set<Node, nodeHash, nodeEqual>>();
          this->connected = make_shared<unordered_set<Node, nodeHash, nodeEqual>>();
          this->usable = this->value.isAvailable();
        }
        inline bool operator == (const Node &node) const{
          return this->value == node.value;
        }
        string getName() const {
          return this->value.getName();
        }
        string toJson() const {
          char json[100];
          sprintf(json, "{\"id\":\"%s\"}", this->getName().c_str());
          return json;
        }
    };
    DAG(): count(0) {
      this->nodes = make_unique<unordered_set<Node, Node::nodeHash, Node::nodeEqual>>(100);
    }

    DAG(shared_ptr<Asset> &asset) {
      auto node = new Node(*asset);;
      this->nodes = make_unique<unordered_set<Node, Node::nodeHash, Node::nodeEqual>>(100);
      this->nodes->insert(*node);
      this->count = 1;
    }
    void connect(Asset &fromAsset, Asset &toAsset) {
      auto source = make_shared<Node>(fromAsset);
      auto dist = make_shared<Node>(toAsset);
      auto from = this->nodes->find(*source);
      auto to = this->nodes->find(*dist);
      if(from == this->nodes->end()) {
        this->nodes->insert(*source);
      } else {
        source = make_shared<Node>(*from);
      }
      if(to == this->nodes->end()) {
        this->nodes->insert(*dist);
      } else {
        dist = make_shared<Node>(*to);
      }
      source->connecting->insert(*dist);
      dist->connected->insert(*source);
      source->evaluateUsableWhenChange(*dist);
    }
    string print(){
      string print = "";
      set<string> sortedLines;
      for(auto& head : *this->nodes) {
        string line = "[";
        line += head.getName();
        line += "] \t";
        for(auto &node: *head.connecting){
          line += node.getName();
          line += "  ";
        }
        line += "\n";
//        print += line;
        sortedLines.insert(line);
      }
      for(auto &line : sortedLines) {
        print += line;
      }
      return print;
    };
    string* toJson(){
      auto print = new string("{");
      for(auto& head : *this->nodes) {
        string line = "\"";
        line += head.getName();
        line += "\":[";
        for(auto &node: *head.connecting){
          line += node.toJson() + ",";
        }
        line += "],\n";
        *print += line;
      }
      *print += "}";
      return print;
    }
    bool remove(Asset &asset) {
      Node const *$node = nullptr;
      bool flag = false;
      for(auto &node : *this->nodes) {
        if(node.value == asset) {
          $node = &node;
          flag = true;
          this->nodes->erase(node);
        }
        if($node != nullptr) {
          node.connecting->erase(*$node);
        }
      }
      return flag;
    }
    void updateUsable(Asset &asset) {
      for(auto &node : *this->nodes) {
        if(node.value == asset) {
          node.updateUsable(asset.isAvailable());
          return;
        }
      }
    }
protected:
    ll count;
    unique_ptr<unordered_set<Node, Node::nodeHash, Node::nodeEqual>> nodes;
};