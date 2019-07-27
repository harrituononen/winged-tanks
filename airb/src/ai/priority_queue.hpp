#ifndef _PRIORITY_QUEUE_HPP_
#define _PRIORITY_QUEUE_HPP_

#include <queue>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>

using namespace std;

template<class T, class Compare> class PriorityQueue
{
    vector<T> v;
    Compare comp;
public:
    // Don't need to call make_heap(); it's empty:
    PriorityQueue(Compare cmp = Compare()) : comp(cmp) {}
    void push(const T& x)
    {
        v.push_back(x); // Put it at the end
                        // Re-adjust the heap:
        push_heap(v.begin(), v.end(), comp);
    }
    void pop()
    {
        // Move the top element to the last position:
        pop_heap(v.begin(), v.end(), comp);
        v.pop_back(); // Remove that element
    }
    const T& top() { return v.front(); }
    bool empty() const { return v.empty(); }
    int size() const { return v.size(); }
    typedef vector<T> TVec;
    TVec get_vector()
    {
        TVec r(v.begin(), v.end());
        // It s already a heap
        sort_heap(r.begin(), r.end(), comp);
        // Put it into priority-queue order:
        reverse(r.begin(), r.end());
        return r;
    }
};
#endif
