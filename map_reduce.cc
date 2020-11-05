#include "map_reduce.hh"

using namespace MapReduce;
using namespace MR_Utilities;

//sets info from the global datastructure
void MR_Emit(const std::string& key, const std::string& value){
    int part_num = global_partioner(key, partition_number);
    MapWrapper* map = storage->get_mapping(part_num);
    List* list = map->get_list_or_initialize(key);
    list->add_value(value);
}


unsigned long MR_DefaultHashPartition(const std::string& key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    for (const char& c : key) {
        hash = hash * 33 + c;
    }
    return hash % num_partitions;
}


const std::string global_getter(const std::string& key, int partition_number) {
    MapWrapper* map = storage->get_mapping(partition_number);
    List* list = map->get_list_or_initialize(key);

    return list->get_value();
}



// void run_mapper(const std::string& file_name) {

// }



void MR_Run(int argc, char* argv[], mapper_t map, int num_mappers, reducer_t reduce, int num_reducers, partitioner_t partition) {
    global_partioner = partition;
    partition_number = num_reducers;
    mapper_number = num_mappers;
    reducer_number = num_reducers;
    
    std::async
}