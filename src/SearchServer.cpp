#include "SearchServer.h"
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

SearchServer::SearchServer(InvertedIndex& idx) : index(idx) {
}

std::vector<std::string> splitIntoWords(const std::string& text) {
    std::istringstream iss(text);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> final_results;

    std::vector<std::string>::const_iterator query_it;
    for (query_it = queries_input.begin(); query_it != queries_input.end(); query_it++) {
        std::vector<std::string> words = splitIntoWords(*query_it);
        std::vector<std::string> unique_words;

        std::vector<std::string>::iterator w;
        for (w = words.begin(); w != words.end(); w++) {
            bool found = false;
            std::vector<std::string>::iterator u;
            for (u = unique_words.begin(); u != unique_words.end(); u++) {
                if (*w == *u) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                unique_words.push_back(*w);
            }
        }

        if (unique_words.empty()) {
            final_results.emplace_back();
            continue;
        }

        std::map<size_t, int> doc_absolute_relevance;

        std::vector<std::string>::iterator word_it;
        for (word_it = unique_words.begin(); word_it != unique_words.end(); word_it++) {
            std::vector<Entry> word_entries = index.GetWordCount(*word_it);
            std::vector<Entry>::iterator entry_it;

            for (entry_it = word_entries.begin(); entry_it != word_entries.end(); entry_it++) {
                doc_absolute_relevance[entry_it->doc_id] += entry_it->count;
            }
        }

        int max_absolute_relevance = 0;
        std::map<size_t, int>::iterator rel_it;
        for (rel_it = doc_absolute_relevance.begin(); rel_it != doc_absolute_relevance.end(); rel_it++) {
            if (rel_it->second > max_absolute_relevance) {
                max_absolute_relevance = rel_it->second;
            }
        }

        std::vector<RelativeIndex> rel_list;
        for (rel_it = doc_absolute_relevance.begin(); rel_it != doc_absolute_relevance.end(); rel_it++) {
            RelativeIndex ri{};
            ri.doc_id = rel_it->first;
            if (max_absolute_relevance > 0) {
                ri.rank = static_cast<float>(rel_it->second) / static_cast<float>(max_absolute_relevance);
            } else {
                ri.rank = 0.0f;
            }
            rel_list.push_back(ri);
        }

        std::sort(rel_list.begin(), rel_list.end(),
            [](const RelativeIndex& a, const RelativeIndex& b) {
                if (a.rank != b.rank) {
                    return a.rank > b.rank;
                }
                return a.doc_id < b.doc_id;
            });

        if (rel_list.size() > 5) {
            rel_list.resize(5);
        }

        final_results.push_back(rel_list);
    }

    return final_results;
}