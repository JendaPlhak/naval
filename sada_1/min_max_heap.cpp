#include "min_max_heap.h"

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    auto random = []() {
        int max = 50;
        int min = -max;
        return min + (rand() % (int)(max - min + 1));
    };

    std::vector<int> test_permutation;
    for (int i = 0; i < 987; ++i) {
        test_permutation.push_back(random());
    }

    for (int test = 0; test < 10000; ++test) {
        // if (test % 10 == 0) {
        //     std::cout << "Test NO " << test << std::endl;
        // }
        std::random_shuffle(test_permutation.begin(), test_permutation.end());
        MinMaxHeap<int> heap;
        for (const auto n : test_permutation) {
            // std::cout << "inserting: " << n << "\n";
            heap.insert(n);
            // std::cout << "inserting: " << n * 49739 % 50 << "\n";
            heap.insert(n * 49739 % 50);
            // std::cout << "Deleting min.. " << "\n";
            heap.deleteMin();
        }
        for (const auto n : test_permutation) {
            (void) n;
            // std::cout << "Deleting min.. " << "\n";
            heap.deleteMin();
            // std::cout << "Deleting max.. " << "\n";
            heap.deleteMax();
        }
    }
    return 0;
}

