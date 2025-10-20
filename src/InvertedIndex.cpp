#include "InvertedIndex.h"
#include <sstream>
#include <map>
#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>

InvertedIndex::InvertedIndex() {
    this->docs = std::vector<std::string>();
    this->freq_dictionary = std::map<std::string, std::vector<Entry>>();
}

void InvertedIndex::ProcessDocument(size_t doc_id, const std::string& content) {
    std::istringstream iss(content);
    std::string word;
    std::map<std::string, int> word_count;

    while (iss >> word) {
        if (word.length() > 100) {
            continue;
        }
        word_count[word] = word_count[word] + 1;
    }

    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    std::map<std::string, int>::iterator wc_it;
    for (wc_it = word_count.begin(); wc_it != word_count.end(); wc_it++) {
        Entry entry;
        entry.doc_id = doc_id;
        entry.count = wc_it->second;

        std::map<std::string, std::vector<Entry>>::iterator dict_it = freq_dictionary.find(wc_it->first);
        if (dict_it != freq_dictionary.end()) {
            bool found = false;
            std::vector<Entry>::iterator entry_it;
            for (entry_it = dict_it->second.begin(); entry_it != dict_it->second.end(); entry_it++) {
                if (entry_it->doc_id == doc_id) {
                    entry_it->count += wc_it->second;
                    found = true;
                    break;
                }
            }
            if (!found) {
                dict_it->second.push_back(entry);

                std::sort(dict_it->second.begin(), dict_it->second.end(),
                    [](const Entry& a, const Entry& b) { return a.doc_id < b.doc_id; });
            }
        } else {
            freq_dictionary[wc_it->first] = std::vector<Entry>{entry};
        }
    }
}

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    this->docs = input_docs;
    this->freq_dictionary.clear();

    std::vector<std::thread> threads;
    size_t docs_size = this->docs.size();

    for (size_t doc_id = 0; doc_id < docs_size; doc_id++) {
        if (this->docs[doc_id].length() > 1000 * 100) {
            continue;
        }
        threads.push_back(std::thread(&InvertedIndex::ProcessDocument, this, doc_id, this->docs[doc_id]));
    }

    std::vector<std::thread>::iterator thread_it;
    for (thread_it = threads.begin(); thread_it != threads.end(); thread_it++) {
        if (thread_it->joinable()) {
            thread_it->join();
        }
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::map<std::string, std::vector<Entry>>::iterator it;
    it = this->freq_dictionary.find(word);
    if (it != this->freq_dictionary.end()) {
        return it->second;
    }
    return std::vector<Entry>();
}