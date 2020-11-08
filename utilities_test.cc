#include <cassert>
#include "utilities.hh"
using namespace MR_Utilities;

void init_global_storage() {
    GlobalStorage* storage = new GlobalStorage();
    assert(storage->storage_vector.size() == 0);

    MapWrapper* m1 = storage->get_mapping(2);
    assert (m1 != nullptr);
    assert (m1 = storage->storage_vector[2]);
    assert (storage->storage_vector.size() == 2);

    MapWrapper* m2 = storage->get_mapping(5);
    assert (m2 != nullptr);
    assert (m2 = storage->storage_vector[5]);
    assert (storage->storage_vector.size() == 5);


    MapWrapper* m3 = storage->get_mapping(1);
    assert(m3 != nullptr);
    assert(storage->storage_vector.size() == 5);

    assert(m3->mapping["abc"] == nullptr);
    List* l = m3->get_list_or_initialize("abc");

    assert(l != nullptr);
    assert(m3->mapping["abc"] == l);

    int range = 10;
    for (int i = 0; i < range; i++) {
        l->add_value(std::to_string(i));
    }

    assert(l->list.size() == 10);

    std::string s = l->get_value();
    assert(s == "0");
    assert(l->head == 1);
    l->head = 0;

    for (int i = 0; i < storage->storage_vector.size(); i++) {
        const MapWrapper* m = storage->get_mapping_no_sync(i);
    }
}


int main() {
    init_global_storage();
}