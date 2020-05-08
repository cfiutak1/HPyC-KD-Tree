#ifndef COMBINATION_HPP
#define COMBINATION_HPP


#include <cstdio>
#include <vector>


class Combination {
private:
    std::vector<int> combinations;
    int range_begin;
    int range_end;
    unsigned int initial_state;
    unsigned int current_state;
    int combination_num = 0;

public:
    Combination(int range_begin_in, int range_end_in, int num_items):
        range_begin(range_begin_in),
        range_end(range_end_in)
    {
        combinations.reserve(range_end_in - range_begin_in);
        this->initial_state = (1 << num_items) - 1;
        this->current_state = initial_state;

        for (int i = range_begin_in; i < range_end_in; ++i) {
            combinations.push_back(i);
        }
    }


    std::vector<int> getCurrentCombination() {
        std::vector<int> current_combination;
        current_combination.reserve(this->combinations.size());

        for (int i = 0; i < this->combinations.size(); ++i) {
            if ((1 << i) & this->current_state) { current_combination.push_back(this->combinations[i]); }
        }

        this->getNextState();

        return current_combination;
    }

    void getNextState() {
        unsigned int next_state = this->current_state;
        int ones_count = 0; // ones_count = 0

        for (int i = 0; i < combinations.size(); ++i) {
            unsigned int bitmask = this->current_state & (1 << i);
            next_state ^= bitmask;

            if (bitmask) { ++ones_count; }

            if ((ones_count != 0) && (bitmask == 0)) {
                next_state += (1 << i);
                --ones_count;

                break;
            }
        }

        next_state += (1 << ones_count) - 1;
        this->current_state = next_state;

        if (this->current_state != this->initial_state) { ++this->combination_num; }
        else { this->combination_num = 0; }
    }

    std::vector<std::vector<int>> getAllCombinations() {
        std::vector<std::vector<int>> all_combinations;

        do {
            all_combinations.push_back(this->getCurrentCombination());
        } while (this->combination_num != 0);


        return all_combinations;
    }
};


#endif