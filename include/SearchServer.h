#pragma once

#include <vector>
#include <string>
#include <cmath>
#include "InvertedIndex.h"

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const {
        return doc_id == other.doc_id &&
               std::fabs(rank - other.rank) < 0.0001f;
    }

    bool operator<(const RelativeIndex& other) const {
        if (std::fabs(rank - other.rank) > 0.0001f)
            return rank > other.rank;
        return doc_id < other.doc_id;
    }
};

class SearchServer {
public:
    SearchServer(InvertedIndex& idx);
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& index;
};