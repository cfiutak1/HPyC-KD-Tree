#ifndef NETWORKGENERATOR_HPP
#define NETWORKGENERATOR_HPP

#include "Combination.hpp"

#include <vector>
#include <atomic>
#include <algorithm>

class NetworkGenerator {
public:
    int array_size;
    int num_comparators;
    std::vector<std::vector<int>> possible_comparators;
    std::vector<std::vector<int>> unique_comparator_combinations;
    std::atomic<int> comparator_combination_index{0};

    NetworkGenerator(int array_size_in, int num_comparators_in):
        array_size(array_size_in),
        num_comparators(num_comparators_in)
    {
        Combination comparators(0, array_size_in, 2);
        this->possible_comparators = comparators.getAllCombinations();

        Combination all_combinations(0, possible_comparators.size(), num_comparators_in);
        this->unique_comparator_combinations = all_combinations.getAllCombinations();
    }

    std::vector<std::vector<std::vector<int>>> requestBatch() {
        std::vector<std::vector<std::vector<int>>> all_perms;

        auto comps = this->unique_comparator_combinations[this->comparator_combination_index++];

        do {
            std::vector<std::vector<int>> comb;
            comb.reserve(comps.size());

            for (int i : comps) { comb.push_back(this->possible_comparators[i]); }

            all_perms.push_back(comb);
        } while (std::next_permutation(comps.begin(), comps.end()));

        return all_perms;
    }

};

#endif