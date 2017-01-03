//
// Created by Tanki on 2017/1/1.
//

#include <memory>
typedef long long ll;

template<class T>
class List {
    public:
        class Node
        {
        public:
            std::shared_ptr<Node> _next;
            T &data;
            Node(T &data): data(data){};
            ~Node(){
              delete &data;
            }
        };
    protected:
        std::shared_ptr<Node> head;
        ll count = 0;
    public:
        inline ll getCount() const {
            return count;
        }
        List(T &data) {
            auto node = std::make_shared<Node>(data);
            this->head = node;
            this->count = 1;
        }
        List(): head(nullptr) {
            this->count = 0;
        }
        inline int size() {
            return this->count * sizeof(Node);
        };
        T* find(T &data){
            auto next = this->head.get();
            while (next){
                if(next->data == data) return &(next->data);
                next = next->_next.get();
            }
            return nullptr;
        };
        void append(T &data){
            auto node = std::make_shared<Node>(data);
            node.get()->_next = this->head;
            this->head = node;
            this->count++;
        };
      Node* getHead() {
        return this->head.get();
      }
};