#include "../map_reduce.hh"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>

const char* search_phrase;
const int search_phrase_index = 1;
const int file_index_start = 2;

void map_func(const char *line) {
    // printf("new line here\n");
    if (strstr(line, search_phrase)) {
        // printf("start emitting\n");
        MapReduce::MR_Emit(line, "1");
        // printf("finished emitting\n");
    }
}

void reduce_func(const std::string & key, MapReduce::getter_t get_next, int partition_number) {
    int count = 0;
    std::string value;
    while ((value = get_next(key, partition_number)) != "") {
        count++;
    }
    printf("%s %d\n", key.c_str(), count);
}

void grep_from_stdin() {
    std::string line;
    std::vector<std::string> container;

    while (std::getline(std::cin, line)) {
        container.push_back(line);
    }

    int input_argc = container.size() + 1;
    char* inputs[input_argc];

    for (int i = 1; i < input_argc; i++) {
        inputs[i] = strdup(container[i-1].c_str());
        // printf("%s\n", inputs[i]);
    }
    MapReduce::MR_Run(input_argc, inputs, map_func, 8, reduce_func, 1, MapReduce::MR_DefaultHashPartition);
}

void greap_from_files(int argc, char* argv[]) {
    std::vector<std::string> container;
    std::string line;

    for (int i = file_index_start; i < argc; i++) {
        std::fstream inf(argv[i]);
        if (!inf.is_open()) {
            continue;
        }

        while (std::getline(inf, line)) {
            container.push_back(line);
        }
        inf.close();
    }

    int input_argc = container.size() + 1;
    char* inputs[input_argc];

    for (int i = 1; i < input_argc; i++) {
        inputs[i] = strdup(container[i-1].c_str());
        // printf("%s\n", inputs[i]);
    }
    MapReduce::MR_Run(input_argc, inputs, map_func, 8, reduce_func, 1, MapReduce::MR_DefaultHashPartition);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "%s: searchterm [file ...]\n", argv[0]);
        exit(1);
    }

    search_phrase = argv[search_phrase_index];
    printf("===== START =====\n");


    if (argc < 3) {
        grep_from_stdin();
    } else {
        greap_from_files(argc, argv);
    }

    printf("===== DONE =====\n");

    return 0;
}