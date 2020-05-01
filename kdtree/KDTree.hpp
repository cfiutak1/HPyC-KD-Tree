#ifndef KDTREE_HPP
#define KDTREE_HPP

#include "../filedata/TrainingFileData.hpp"
#include "../threadpool/ThreadCounter.hpp"
#include "../quickselect/AdaptiveQuickselect.hpp"
#include "Point.hpp"
#include "KDNode.hpp"

#include <cstdint>
#include <vector>
#include <map>
#include <future>


class ConstructionTask {
public:
    const KDNode* parent_node;
    const uint64_t& chunk_begin;
    const uint64_t& chunk_end;
    uint64_t depth;

    ConstructionTask(const KDNode* parent_node_in, const uint64_t& chunk_begin_in, const uint64_t& chunk_end_in, uint64_t depth_in):
        parent_node(parent_node_in), chunk_begin(chunk_begin_in), chunk_end(chunk_end_in), depth(depth_in)
    {}

};


class KDTree {
private:
    KDNode* root = nullptr;
    unsigned int num_threads = 0;
    uint64_t num_points = 0;
    uint64_t num_dimensions = 0;
    ThreadCounter thread_counter;
    std::vector<Point*> points;

    uint64_t current_size = 0;
    std::map<uint64_t, PointDimensionDistanceComparator> dimension_distance_comparators;
    unsigned int height = 0;


    void buildTreeBFS();
    KDNode* buildTreeDFSWrapper(std::pair<uint64_t, uint64_t> process, uint64_t depth);
    KDNode* buildTreeDFS(uint64_t begin, uint64_t end, uint64_t depth);

public:
    KDTree(
        unsigned int num_threads_in,
        TrainingFileData* training_file_data_in,
        std::vector<Point*> points_in
    ):
        num_threads(num_threads_in),
        num_points(training_file_data_in->num_points),
        num_dimensions(training_file_data_in->num_dimensions),
        thread_counter(num_threads_in),
        points(points_in),
        height(std::ceil(log2(num_points)))
    {
        for (uint64_t i = 0; i < this->num_dimensions; i++) {
            PointDimensionDistanceComparator comp(i);
            this->dimension_distance_comparators[i] = comp;
        }
    }

    ~KDTree() { this->destructorHelper(this->root); }

    void destructorHelper(KDNode* n) {
        if (n == nullptr) return;

        this->destructorHelper(n->left_child);
        this->destructorHelper(n->right_child);
        // printf("Deleting ");
        // n->point->printCoordinates();

        delete n;
    }

    KDNode* getRoot() { return this->root; }
    uint64_t getNumPoints() { return this->num_points; }
    uint64_t getNumDimensions() { return this->num_dimensions; }
    unsigned int getHeight() { return this->height; }

    std::vector<Point*> nearestNeighborsSearch(float* query_point, uint64_t num_neighbors);

    void buildTreeParallel();
    KDNode* buildSubTreeDepthFirst(const uint64_t& begin, const uint64_t& end, uint64_t depth, long allocated_threads);
    // void buildBaseTreeBreadthFirst(
    //     std::vector<std::pair<uint64_t, uint64_t>>& process_queue,
    //     std::vector<KDNode*>& base_tree_leaves,
    //     uint64_t& depth
    // );
    // void connectBaseAndSubTrees(
    //     std::vector<KDNode*> base_tree_leaves,
    //     std::vector<KDNode*> sub_tree_roots
    // );
    //
    // std::vector<KDNode*> buildSubTrees(
    //     std::vector<std::pair<uint64_t, uint64_t>> process_queue,
    //     uint64_t depth
    // );
    //
    // KDNode* buildSubTreeDepthFirstWrapper(std::pair<uint64_t, uint64_t> process, uint64_t depth);
    //
    // KDNode* buildSubTreeDepthFirst(uint64_t begin, uint64_t end, uint64_t depth);
};

#endif
