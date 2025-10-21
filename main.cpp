#include "include/ConverterJSON.h"
#include "include/InvertedIndex.h"
#include "include/SearchServer.h"
#include <iostream>

int main() {

    ConverterJSON converter;

    converter.CheckVersion();

    std::string engine_name = converter.GetEngineName();
    std::cout << "Starting " << engine_name << "\n";

    std::vector<std::string> docs = converter.GetTextDocuments();
    if (docs.empty()) {
        std::cout << "No documents to process.\n";
        return 1;
    }

    int max_responses = converter.GetResponsesLimit();

    std::vector<std::string> requests = converter.GetRequests();
    if (requests.empty()) {
        std::cout << "No requests to process.\n";
        return 1;
    }

    InvertedIndex index;
    index.UpdateDocumentBase(docs);

    SearchServer server(index, max_responses);
    std::vector<std::vector<RelativeIndex>> results = server.search(requests);

/*    std::vector<std::vector<std::pair<int, float>>> answers_to_save;

    std::vector<std::vector<RelativeIndex>>::iterator it;

    for (it = results.begin(); it != results.end(); it++) {
        std::vector<std::pair<int, float>> limited;
        int count = 0;
        std::vector<RelativeIndex>::iterator ri_it;
        for (ri_it = it->begin(); ri_it != it->end(); ri_it++) {
            if (count >= max_responses) {
                break;
            }
            std::pair<int, float> p;
            p.first = ri_it->doc_id;
            p.second = ri_it->rank;
            limited.push_back(p);
            count++;
        }
        answers_to_save.push_back(limited);
    }*/

    std::vector<std::vector<std::pair<int, float>>> answers_to_save;

    std::vector<std::vector<RelativeIndex>>::const_iterator it;
    for (it = results.begin(); it != results.end(); it++) {
        std::vector<std::pair<int, float>> converted;
        std::vector<RelativeIndex>::const_iterator ri_it;
        for (ri_it = it->begin(); ri_it != it->end(); ri_it++) {
            std::pair<int, float> p;
            p.first = (int)ri_it->doc_id;
            p.second = ri_it->rank;
            converted.push_back(p);
        }
        answers_to_save.push_back(converted);
    }

    converter.putAnswers(answers_to_save);
    std::cout << "Search completed. Results saved to answers.json\n";

    return 0;
}