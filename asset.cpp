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
    bool available = true;
    Asset(){};
  public:
    Asset(string name): name(name){};
    static void reserve(int64_t amount) {
      map.reserve(amount);
    }
    static shared_ptr<Asset> get(string name, bool force = true) {
      if (name.length() == 0) return nullptr;
      auto it = Asset::map.find(name);
      if (it ==  Asset::map.end()) {
        if(!force) return shared_ptr<Asset>(nullptr);
        auto asset = Asset::buildSharedPtr(name);
        Asset::map.insert({name, asset});
        return asset;
      }
      return it->second;
    }
    static shared_ptr<Asset> buildSharedPtr(string name) {
      struct makeShared : public Asset {
          public: makeShared(string name): Asset(name){};
      };
      return make_shared<makeShared>(name);
    }
    inline string getName() const{
      return this->name;
    }
    static shared_ptr<Asset> setAvailableStatusTo(string name, bool status) {
      if (name.length() == 0) return nullptr;
      auto it = Asset::map.find(name);
      if (it ==  Asset::map.end()) return nullptr;
      it->second->available = status;
      return it->second;
    }

    bool setDeleteStatusTo(bool status) {
      this->available = status;
      return true;
    }

    bool isAvailable() {
      return this->available;
    }
    inline bool operator == (const Asset &asset) const{
      return !strcmp(this->name.c_str(), asset.name.c_str());
    }
};

unordered_map<string, shared_ptr<Asset>> Asset::map = {};