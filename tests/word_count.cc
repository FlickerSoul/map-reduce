#include "../map_reduce.hh"
#include <cassert>
#include <string.h>
#include <string>

void map_func(const char *file_name) {
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);

    char *line = NULL;
    size_t size = 0;
    while (getline(&line, &size, fp) != -1) {
        char *token, *dummy = line;
        while ((token = strsep(&dummy, " \t\n\r")) != NULL) {
            MapReduce::MR_Emit(token, "1");
        }
    }
    free(line);
    fclose(fp);
}

void reduce_func(const std::string & key, MapReduce::getter_t get_next, int partition_number) {
    int count = 0;
    std::string value;
    while ((value = get_next(key, partition_number)) != "")
        count++;
    printf("%s %d\n", key.c_str(), count);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("%s [file_name ...]", argv[0]);
        exit(1);
    } 
    MapReduce::MR_Run(argc, argv, map_func, 10, reduce_func, 10, MapReduce::MR_DefaultHashPartition);
    return 0;
}