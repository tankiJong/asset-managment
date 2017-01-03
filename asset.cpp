//
// Created by Tanki on 2017/1/1.
//
#include <iostream>
#include <unordered_map>
using namespace std;
class Asset {
  protected:
    static unordered_map<string, shared_ptr<Asset>> map;
    string name;

    Asset(){};
  public:
    Asset(string name): name(name){};
    static shared_ptr<Asset> get(string name) {
      if (name.length() == 0) return nullptr;
      auto it = Asset::map.find(name);
      if (it ==  Asset::map.end()) {
          auto asset = Asset::buildSharedPtr(name);
          Asset::map.insert({name, asset});
          return asset;
      }
      return it->second;
    }
    static shared_ptr<Asset> buildSharedPtr(string name){
      struct makeShared : public Asset {
          public: makeShared(string name): Asset(name){};
      };
      return make_shared<makeShared>(name);
    };
    inline string getName() const{
      return this->name;
    }
    inline bool operator == (const Asset &asset) const{
      return this->name == asset.name;
    }
};

unordered_map<string, shared_ptr<Asset>> Asset::map = {};