#include "map_reduce.hh"


MR_Utilities::GlobalStorage* MR_Utilities::storage = new MR_Utilities::GlobalStorage();
MR_Utilities::MR_Info<MapReduce::partitioner_t> mr_info;


//sets info from the global datastructure
void MapReduce::MR_Emit(const std::string& key, const std::string& value) {
    printf("emitted value\n");
    int part_num = mr_info.global_partioner(key, mr_info.partition_number);
    printf("get partition value\n");
    MR_Utilities::MapWrapper* map = MR_Utilities::storage->get_mapping(part_num);
    assert(map != nullptr);
    printf("get map\n");
    MR_Utilities::List* list = map->get_list_or_initialize(key);
    assert(list != nullptr);
    printf("get list\n");
    list->add_value(value);
    printf("added value\n");
}

unsigned long MapReduce::MR_DefaultHashPartition(const std::string& key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    for (const char& c : key) {
        hash = hash * 33 + c;
    }
    return hash % num_partitions;
}

const std::string global_getter(const std::string& key, int partition_number) {
    MR_Utilities::MapWrapper* map = MR_Utilities::storage->get_mapping(partition_number);
    MR_Utilities::List* list = map->get_list_or_initialize(key);

    return list->get_value();
}

template <typename T>
void generate_map_input(T* map_inputs, int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        map_inputs->push(std::make_tuple(argv[i]));
    }
}


template <typename T>
void generate_reduce_input(T* reduce_inputs) {
    for (int i = 0; i < mr_info.partition_number; i++) {
        const MR_Utilities::MapWrapper* partition = MR_Utilities::storage->get_mapping_no_sync(i);
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


void MapReduce::MR_Run(int argc, char* argv[], 
            MapReduce::mapper_t map_func, int num_mappers, 
            MapReduce::reducer_t reduce_func, int num_reducers, 
            MapReduce::partitioner_t partition) {
    mr_info.global_partioner = partition;
    mr_info.partition_number = num_reducers;
    mr_info.mapper_number = num_mappers;
    mr_info.reducer_number = num_reducers;

    //call the mappers 
    std::queue<std::tuple<const char*>> map_inputs;
    generate_map_input<decltype(map_inputs)>(&map_inputs, argc, argv);
    ThreadPool<std::tuple<const char*>, MapReduce::mapper_t> mapper (num_mappers, map_inputs, map_func);
    mapper.start_jobs();
    mapper.terminate_and_wait();
    // wait mapper to finish 

    // call reducer 
    std::queue<std::tuple<std::string, MapReduce::getter_t, int>> reduce_inputs;
    generate_reduce_input<decltype(reduce_inputs)>(&reduce_inputs);
    ThreadPool<std::tuple<std::string, MapReduce::getter_t, int >, MapReduce::reducer_t> reducer (num_reducers, reduce_inputs, reduce_func);
    reducer.start_jobs();
    reducer.terminate_and_wait();
    // wait reducer to finish

    printf("mapper counter %llu\n", mapper.counter);
    printf("reducer counter %llu\n", reducer.counter);
}
