#ifndef MR_UTILITIES_
#define MR_UTILITIES_

#include <string>
#include <map>
#include <mutex>
#include <iostream>
#include <vector>

using namespace std;

namespace MR_Utilities {
  class List {
    public: 
      vector<string> list;
      unsigned long head = 0;
      std::mutex lock;

      void add_value(const string& value){
          this->lock.lock();
          this->list.push_back(value);
          this->lock.unlock();
      }
          
      std::string get_value(){
          this->lock.lock();
          unsigned long current_head = head++;
          this->lock.unlock();

          if (current_head >= this->list.size()) {
            return "";
          } else {
            return (this->list[current_head]);
          }
      }
  };


  class MapWrapper {
    public: 
      map<std::string, List*> mapping;
      mutex lock;

      List* get_list_or_initialize(const std::string & key){
          this->lock.lock();

          List* list = this->mapping[key];
          if (list == nullptr) {
            list = new List();
            this->mapping[key] = list;
          }

          this->lock.unlock();
          return list;
      }
  };


  class GlobalStorage {
    public: 
      vector<MapWrapper*> storage_vector;
      mutex lock;

      ~GlobalStorage();
      MapWrapper* get_mapping(unsigned long partition_num) {
        // if the vector is not large enough, we make it larger 
        this->lock.lock();
        if (partition_num >= this->storage_vector.size()) {
          this->storage_vector.resize(partition_num);
        }
        
        MapWrapper* wrapper = this->storage_vector[partition_num];

        if (wrapper == nullptr) {
          wrapper= new MapWrapper();
          this->storage_vector[partition_num] = wrapper;
        }
        this->lock.unlock();
        
        return wrapper;
      }

      MapWrapper* get_mapping_no_sync(unsigned long partition_num) {
        return this->storage_vector[partition_num];
      }
  };

  extern GlobalStorage* storage;
}

#endif
