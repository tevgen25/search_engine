#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class ConverterJSON {
public:
    ConverterJSON();
    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();
    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);
    void CheckVersion();
    std::string GetEngineName();
};