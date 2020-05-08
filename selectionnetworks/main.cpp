#include "Combination.hpp"
#include "NetworkGenerator.hpp"

#include <vector>
#include <cstdio>

inline void comp_and_swap(int& x, int& y) {
    int item = x;
    x = (y < item) ? y : x;
    y = (y < item) ? item : y;
}


void permute(std::vector<std::vector<int>>& p, std::vector<int>& a, int l, int r) {
    if (l == r) {
        std::vector<int> copy(a);
        p.push_back(copy);

        return;
    }

    for (int i = l; i <= r; i++) {
        std::swap(a[l], a[i]);
        permute(p, a, l+1, r);
        std::swap(a[l], a[i]);
    }
}


std::vector<std::vector<int>> generatePermutations(int size) {
    std::vector<int> a;
    std::vector<std::vector<int>> perms;

    a.reserve(size);

    for (int i = 0; i < size; ++i) {
        a.push_back(i);
    }
    permute(perms, a, 0, size - 1);

    return perms;
}


bool validateNetwork(std::vector<std::vector<int>>& network, std::vector<std::vector<int>> test_cases) {
    bool valid = true;

    for (std::vector<int> test_case : test_cases) {
        std::vector<int> test_copy(test_case);

        for (auto comp : network) {
            comp_and_swap(test_copy[comp[0]], test_copy[comp[1]]);
        }

        bool median_in_position = test_copy[test_copy.size() / 2] == ((int) test_copy.size() / 2);

        if (!median_in_position) {
            valid = false;
            break;
        }

        for (unsigned long k = (test_copy.size() / 2) + 1; k < test_copy.size(); k++) {
            bool item_in_correct_partition = false;

            for (unsigned long j = (test_copy.size() / 2) + 1; j < test_copy.size(); ++j) {
                if ((unsigned long) test_copy[j] == k) {
                    item_in_correct_partition = true;
                    break;
                }
            }

            if (!item_in_correct_partition) {
                valid = false;
                break;
            }

        }

        if (!valid) break;
    }

    return valid;
}

int main() {
    int array_size = 4;
    int comparator_size = 5;
    NetworkGenerator gen(array_size, comparator_size);

    std::vector<std::vector<int>> test_cases = generatePermutations(array_size);

    for (unsigned int i = 0; i < gen.unique_comparator_combinations.size(); ++i) {
        auto network_batch = gen.requestBatch();

        for (auto network : network_batch) {
            if (validateNetwork(network, test_cases)) {
                for (auto n : network) printf("%d<->%d ", n[0], n[1]);
                printf("VALID\n");
                exit(0);
            }
        }
    }


}