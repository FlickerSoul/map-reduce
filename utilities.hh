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

      List* get_list_no_sync(const std::string & key) {
        return this->mapping[key];
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
        // printf("get mapping locked\n");
        if (partition_num >= this->storage_vector.size()) {
          this->storage_vector.resize(partition_num + 1);
          // printf("mapping vec resized\n");
        }
        
        // printf("changed\n");
        // printf("size %lu\n", this->storage_vector.size());
        MapWrapper* wrapper = this->storage_vector.at(partition_num);

        // printf("get vec %p\n", wrapper);

        if (wrapper == nullptr) {
          wrapper= new MapWrapper();
          this->storage_vector[partition_num] = wrapper;
        }
        this->lock.unlock();
        
        return wrapper;
      }

      MapWrapper* get_mapping_no_sync(unsigned long partition_num) {
        if (this->storage_vector.size() == 0) {
          return nullptr;
        }
        return this->storage_vector[partition_num];
      }
  };


  template <typename T>
  struct MR_Info {
    T global_partioner;
    int partition_number;
    int mapper_number;
    int reducer_number;
  };

  extern GlobalStorage* storage;
}

#endif
