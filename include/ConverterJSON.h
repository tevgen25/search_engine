#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class ConverterJSON {
public:
    ConverterJSON();

    static std::vector<std::string> GetTextDocuments();

    static int GetResponsesLimit();

    static std::vector<std::string> GetRequests();

    static void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);

    static void CheckVersion();

    static std::string GetEngineName();
};