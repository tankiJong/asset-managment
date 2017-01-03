//
// Created by Tanki on 2017/1/1.
//
#include <memory>
#include <unordered_set>
#include <functional>
#include <string>
#include "list.cpp"
#include "asset.cpp"
// Among the dependency of assets, it's impossible to have circle. So DAG should be proper way to store such structure.
using namespace std;

using ll=int64_t;

//class DAG {
//  public:
//    // the node type will used for list, so override several type of operations is a must.
//    class Node{
//      public:
//        Asset& value;
//        unique_ptr<List<Node>> connected;
//        Node(Asset& asset): value(asset) {
//          this->connected = make_unique<List<Node>>();
//        }
//        inline bool operator == (const Node &node) const{
//          return this->value == node.value;
//        }
//        string getName() {
//          return this->value.getName();
//        }
//    };
//    DAG(): count(0), nodes(new List<Node>()) {
//    }
//
//    DAG(shared_ptr<Asset> &asset) {
//      auto node = new Node(*asset);
//      this->nodes = new List<Node>(*node);
//      this->count = 1;
//    }
//    void connect(Asset &fromAsset, Asset &toAsset) {
//      auto source = new Node(fromAsset);
//      auto dist = new Node(toAsset);
//      Node *from = this->nodes->find(*source);
//      Node *to = this->nodes->find(*dist);
//      if(from  == nullptr) {
//        this->nodes->append(*source);
//        from = source;
//      }
//      if(to == nullptr) {
//        this->nodes->append(*dist);
//        to = dist;
//      }
//      from->connected.get()->append(*to);
//    }
//
//    string toString(){
//      auto head = this->nodes->getHead();
//      string print = "";
//      while(head){
//        string line = "";
//        auto list = &head->data;
//        line += list->value.getName();
//        line += "  ";
//        auto listHead = list->connected.get()->getHead();
//
//        while(listHead) {
//          line += listHead->data.getName();
//          line += "  ";
//          listHead = listHead->_next.get();
//        }
//
//        line += "\n";
//        print += line;
//        head = head->_next.get();
//      }
//
//      return print;
//    };
//  protected:
//    ll count;
//    List<Node>* nodes;
//};
class DAG {
public:
    // the node type will used for list, so override several type of operations is a must.
    class Node{
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
        shared_ptr<unordered_set<Node, nodeHash, nodeEqual>> connected;
        Node(Asset& asset): value(asset) {
          this->connected = make_shared<unordered_set<Node, nodeHash, nodeEqual>>();
        }
        inline bool operator == (const Node &node) const{
          return this->value == node.value;
        }
        string getName() const {
          return this->value.getName();
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
      source->connected->insert(*dist);
    }

    string toString(){
      string print = "";
      for(auto& head : *this->nodes) {
        string line = "[";
        line += head.getName();
        line += "] ";
        for(auto &node: *head.connected){
          line += node.getName();
          line += "  ";
        }
        line += "\n";
        print += line;
      }
      return print;
    };
protected:
    ll count;
    unique_ptr<unordered_set<Node, Node::nodeHash, Node::nodeEqual>> nodes;
};