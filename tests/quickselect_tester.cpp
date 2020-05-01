#include "../kdtree/Point.hpp"
#include "../quickselect/AdaptiveQuickselect.hpp"

#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <chrono>


bool ptcomp(Point* p1, Point* p2) {
    return p1->coordinates[0] < p2->coordinates[0];
}


int main() {
    srand (42);


    for (int i = 1; i < 2000; ++i) {

        std::vector<Point*> a;
        for (int j = 0; j < i; j++) {
            Point* p = new Point(1);
            p->coordinates[0] = float(rand() % 100) / ((rand() % 100) + 1);

            a.push_back(p);
        }

        std::random_shuffle(a.begin(), a.end());

        auto beg = a.begin();
        auto end = a.end();
        AdaptiveQuickselect qs(a, 0);
        Point* med_qs = *(qs.adaptiveQuickselect(beg, end, i / 2));

        std::sort(a.begin(), a.end(), ptcomp);
        assert (a[i / 2]->coordinates[0] == med_qs->coordinates[0]);
     }


    std::vector<Point*> a;



    return 0;
}
