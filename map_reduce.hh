// C++ redefinition of mapreduce.h

#pragma once

#include <functional>
#include <string>
#include "thread_pool.hh"
#include "utilities.hh"

namespace MapReduce {

// Getters return "" when there are no more values matching the key (not NULL)
using getter_t = std::function<const std::string(const std::string& key, int partition_number)>;
using mapper_t = std::function<void(const char* filename)>;
using reducer_t = std::function<void(const std::string& key, getter_t getter, int partition_number)>;
using partitioner_t = std::function<unsigned long(const std::string& key, int num_partitions)>;

partitioner_t global_partioner;
int partition_number;
int mapper_number;
int reducer_number;

MR_Utilities::GlobalStorage* storage = new MR_Utilities::GlobalStorage();


void MR_Emit(const std::string& key, const std::string& value) {
    int part_num = global_partioner(key, partition_number);
    MR_Utilities::MapWrapper* map = storage->get_mapping(part_num);
    MR_Utilities::List* list = map->get_list_or_initialize(key);
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
    MR_Utilities::MapWrapper* map = storage->get_mapping(partition_number);
    MR_Utilities::List* list = map->get_list_or_initialize(key);

    return list->get_value();
}


template <typename T>
void generate_map_input(T* map_inputs, int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        map_inputs->push(std::make_tuple(argv[i]));
    }
}


template <typename T>
void generate_reduce_input(T* reduce_inputs) {
    for (int i = 0; i < partition_number; i++) {
        const MR_Utilities::MapWrapper* partition = storage->get_mapping_no_sync(i);
        if (partition == nullptr) {
            continue;
        }
        for (const auto &[key, value] : partition->mapping) {
            
            reduce_inputs->push(
                std::make_tuple(
                    key, global_getter, i
                )
            );
        }
    }
}

void MR_Run(int argc, char* argv[],
            mapper_t map, int num_mappers,
            reducer_t reduce, int num_reducers,
            partitioner_t partition) {

    global_partioner = partition;
    partition_number = num_reducers;
    mapper_number = num_mappers;
    reducer_number = num_reducers;

    //call the mappers 
    std::queue<std::tuple<const char*>> map_inputs;
    generate_map_input<decltype(map_inputs)>(&map_inputs, argc, argv);
    ThreadPool<std::tuple<const char*>, mapper_t> mapper (num_mappers, map_inputs, map);
    mapper.start_jobs();
    mapper.terminate_and_wait();
    // wait mapper to finish 

    // call reducer 
    std::queue<std::tuple<std::string, getter_t, int>> reduce_inputs;
    generate_reduce_input<decltype(reduce_inputs)>(&reduce_inputs);
    ThreadPool<std::tuple<std::string, getter_t, int >, reducer_t> reducer (num_reducers, reduce_inputs, reduce);
    reducer.start_jobs();
    reducer.terminate_and_wait();
    // wait reducer to finish
}
} // namespace
