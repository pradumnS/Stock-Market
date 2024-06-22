#include "../include/FetchOptionData.hpp"
#include <curl/curl.h>
#include <iostream>
#include <stdexcept>
#include "simdjson.h"

// Helper function to write data from curl to a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void OptionsFetcher::fetchOptionsData(const std::string& symbol, OptionsMarket& market) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    
    std::string url = "https://www.nseindia.com/api/option-chain-indices?symbol=" + symbol;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // Set user-agent to avoid being blocked by the server
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3");

        // Set headers to mimic a browser request
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Connection: keep-alive");
        headers = curl_slist_append(headers, "Host: www.nseindia.com");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            throw std::runtime_error("Failed to fetch data from URL");
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    
    parseOptionsData(readBuffer, market);
}

void OptionsFetcher::parseOptionsData(const std::string& jsonData, OptionsMarket& market) {
    simdjson::dom::parser parser;
    simdjson::dom::element root = parser.parse(jsonData);

    for (simdjson::dom::element data : root["records"]["data"]) {
        if (data["PE"].error() == simdjson::SUCCESS) {
            simdjson::dom::element pe = data["PE"];
            double strikePrice = pe["strikePrice"].get_double();
            std::string expiryDate = std::string(pe["expiryDate"].get_c_str());
            double impliedVolatility = pe["impliedVolatility"].get_double();
            market.addOption(strikePrice, expiryDate, impliedVolatility);
        }
        if (data["CE"].error() == simdjson::SUCCESS) {
            simdjson::dom::element ce = data["CE"];
            double strikePrice = ce["strikePrice"].get_double();
            std::string expiryDate = std::string(ce["expiryDate"].get_c_str());
            double impliedVolatility = ce["impliedVolatility"].get_double();
            market.addOption(strikePrice, expiryDate, impliedVolatility);
        }
    }
}
