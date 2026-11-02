#pragma once
#include <algorithm>

// Keep explicitly ordered draws in place, sorting only uninterrupted ordinary
// runs. With no ordered draws this is the existing whole-queue material sort.
template<class Iterator, class Ordered, class Compare>
void SortFxUnorderedRuns(Iterator begin, Iterator end, Ordered ordered, Compare compare)
{
    while (begin != end) {
        if (ordered(*begin)) { ++begin; continue; }
        auto next=begin;
        while (next != end && !ordered(*next)) ++next;
        std::sort(begin,next,compare);
        begin=next;
    }
}
