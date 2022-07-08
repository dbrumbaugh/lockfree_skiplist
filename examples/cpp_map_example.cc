#include "sl_map.h"

#include <stdio.h>
#include <algorithm>

struct MapKey {
    int key;
    friend bool operator<(const MapKey &a, const MapKey &b) {
        return a.key < b.key;
    }

    friend bool operator>(const MapKey &a, const MapKey &b) {
        return a.key > b.key;
    }

    friend bool operator!=(const MapKey &a, const MapKey &b) {
        return a.key != b.key;
    }

    friend bool operator==(const MapKey &a, const MapKey &b) {
        return a.key == b.key;
    }
};

int main() {
    // sl_map: Busy-waiting implementation.
    //         erase() API may be blocked by concurrent
    //         operations dealing with iterator on the
    //         same key.
    //
    // sl_map_gc: Lazy reclaiming implementation.
    //            erase() API will not be blocked by
    //            any concurrent operations, but may
    //            consume more memory.

    // sl_map<int, int> slist;
    sl_map_gc<MapKey, int> slist;

    //   << Insertion >>
    // Insert 100 KV pairs: {0, 0}, {1, 10}, {2, 20}.
    for (int i=0; i<100; ++i) {
        auto res = slist.insert(std::make_pair(MapKey{i}, i*10));
        printf("[insertion] key: %d \t success %d\n", i, res.second);
    }

    auto res = slist.insert(std::make_pair(MapKey{5}, 5*10));
    printf("[insertion] key: %d \t success %d\n", 5, res.second);

    //   << Point lookup >>
    for (int i=0; i<100; ++i) {
        auto k = MapKey{i};
        auto itr = slist.find(k);
        if (itr == slist.end()) continue; // Not found.
        printf("[point lookup] key: %d, value: %d\n", itr->first.key, itr->second);

        // Note: In `sl_map`, while `itr` is alive and holding a node
        //       in skiplist, other thread cannot erase and free the node.
        //       Same as `shared_ptr`, `itr` will automatically release
        //       the node when it is not referred anymore.
        //       But if you want to release the node before that,
        //       you can do it as follows:
        // itr = slist.end();
    }

    //   << Erase >>
    // Erase the KV pair for key 1: {1, 10}.
    slist.erase(MapKey{1});

    //   << Iteration >>
    for (auto& entry: slist) {
        printf("[iteration] key: %d, value: %d\n", entry.first.key, entry.second);
    }

    slist.erase(MapKey{51});
    slist.erase(MapKey{50});
    auto lb = slist.lower_bound(MapKey{50});
    auto ub = slist.upper_bound(MapKey{99});

    for (; lb!=ub; lb++) {
        printf("[bound traversal] key: %d, value: %d\n", lb->first.key, lb->second);
    }
    printf("[bound traversal] key: %d, value: %d\n", lb->first.key, lb->second);


    return 0;
}

