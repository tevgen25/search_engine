#include "ConverterJSON.h"
#include <fstream>
#include <iostream>
#include <sstream>

ConverterJSON::ConverterJSON() {}

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::ifstream config_file("../config.json");
    if (!config_file.is_open()) {
        std::cout << "config file is missing!\n";
        return std::vector<std::string>();
    }

    nlohmann::json config;
    config_file >> config;
    config_file.close();

    if (config.find("config") == config.end()) {
        std::cout << "config file is empty!\n";
        return std::vector<std::string>();
    }

    if (config.find("files") == config.end()) {
        std::cout << "config file is empty!\n";
        return std::vector<std::string>();
    }

    std::vector<std::string> result;
    std::vector<std::string> file_paths = config["files"].get<std::vector<std::string>>();

    std::vector<std::string>::iterator it;
    for (it = file_paths.begin(); it != file_paths.end(); it++) {
        std::ifstream doc_file(*it);
        if (!doc_file.is_open()) {
            std::cout << "File not found: " << *it << "\n";
            continue;
        }
        std::stringstream buffer;
        buffer << doc_file.rdbuf();
        result.push_back(buffer.str());
        doc_file.close();
    }

    return result;
}

int ConverterJSON::GetResponsesLimit() {
    std::ifstream config_file("../config.json");
    if (!config_file.is_open()) {
        std::cout << "config file is missing!\n";
        return 5;
    }

    nlohmann::json config;
    config_file >> config;
    config_file.close();

    if (config["config"].find("max_responses") != config["config"].end()) {
        return config["config"]["max_responses"].get<int>();
    }
    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::ifstream requests_file("../requests.json");
    if (!requests_file.is_open()) {
        std::cout << "requests.json not found!\n";
        return std::vector<std::string>();
    }

    nlohmann::json requests_json;
    requests_file >> requests_json;
    requests_file.close();

    if (requests_json.find("requests") == requests_json.end()) {
        return std::vector<std::string>();
    }

    return requests_json["requests"].get<std::vector<std::string>>();
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
    nlohmann::json output;
    output["answers"] = nlohmann::json::object();

    int answers_size = (int)answers.size();
    int i = 0;
    for (i = 0; i < answers_size; i++) {
        std::string num = std::to_string(i + 1);
        if (num.length() == 1) {
            num = "00" + num;
        } else if (num.length() == 2) {
            num = "0" + num;
        }
        std::string request_id = "request" + num;

        if (answers[i].empty()) {
            output["answers"][request_id]["result"] = "false";
        } else {
            output["answers"][request_id]["result"] = "true";
            if (answers[i].size() > 1) {
                output["answers"][request_id]["relevance"] = nlohmann::json::array();
                std::vector<std::pair<int, float>>::iterator it;
                for (it = answers[i].begin(); it != answers[i].end(); it++) {
                    nlohmann::json item;
                    item["docid"] = it->first;
                    item["rank"] = it->second;
                    output["answers"][request_id]["relevance"].push_back(item);
                }
            } else {
                output["answers"][request_id]["docid"] = answers[i][0].first;
                output["answers"][request_id]["rank"] = answers[i][0].second;
            }
        }
    }

    std::ofstream out_file("../answers.json");
    out_file << output.dump(4);
    out_file.close();
}

void ConverterJSON::CheckVersion() {
    std::ifstream config_file("../config.json");
    if (!config_file.is_open()) {
        return;
    }

    nlohmann::json config;
    config_file >> config;
    config_file.close();

    if (config["config"].find("version") != config["config"].end()) {
        std::string version = config["config"]["version"];
        if (version != "0.1") {
            std::cout << "config.json has incorrect file version!\n";
        }
    }
}

std::string ConverterJSON::GetEngineName() {
    std::ifstream config_file("../config.json");
    if (!config_file.is_open()) {
        return "SearchEngine";
    }

    nlohmann::json config;
    config_file >> config;
    config_file.close();

    if (config["config"].find("name") != config["config"].end()) {
        return config["config"]["name"].get<std::string>();
    }
    return "SearchEngine";
}
