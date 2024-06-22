#ifndef OPTIONS_FETCHER_HPP
#define OPTIONS_FETCHER_HPP

#include "OptionMarket.hpp"
#include <string>

class OptionsFetcher {
public:
    // Fetches option data from the provided URL and adds it to the market
    void fetchOptionsData(const std::string& symbol, OptionsMarket& market);
    
private:
    // Helper method to parse JSON response
    void parseOptionsData(const std::string& jsonData, OptionsMarket& market);
};

#endif // OPTIONS_FETCHER_HPP
