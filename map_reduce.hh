// C++ redefinition of mapreduce.h

#ifndef MR_MAIN_
#define MR_MAIN_
#pragma once

#include <functional>
#include <string>
#include <cassert>
#include "thread_pool.hh"
#include "utilities.hh"

namespace MapReduce {

// Getters return "" when there are no more values matching the key (not NULL)
using getter_t = std::function<const std::string(const std::string& key, int partition_number)>;
using mapper_t = std::function<void(const char* filename)>;
using reducer_t = std::function<void(const std::string& key, getter_t getter, int partition_number)>;
using partitioner_t = std::function<unsigned long(const std::string& key, int num_partitions)>;

void MR_Emit(const std::string& key, const std::string& value);

unsigned long MR_DefaultHashPartition(const std::string& key, int num_partitions);

// template <typename T>
// void generate_map_input(T* map_inputs, int argc, char* argv[]) {
//     for (int i = 1; i < argc; i++) {
//         map_inputs->push(std::make_tuple(argv[i]));
//     }
// }


// template <typename T>
// void generate_reduce_input(T* reduce_inputs) {
//     for (int i = 0; i < partition_number; i++) {
//         const MR_Utilities::MapWrapper* partition = storage->get_mapping_no_sync(i);
//         if (partition == nullptr) {
//             continue;
//         }
//         for (const auto &[key, value] : partition->mapping) {
            
//             reduce_inputs->push(
//                 std::make_tuple(
//                     key, global_getter, i
//                 )
//             );
//         }
//     }
// }

void MR_Run(int argc, char* argv[],
            mapper_t map, int num_mappers,
            reducer_t reduce, int num_reducers,
            partitioner_t partition);
} // namespace

#endif
