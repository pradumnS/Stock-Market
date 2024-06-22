#include "../include/OptionMarket.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>

OptionsMarket::OptionsMarket() {}

void OptionsMarket::addOption(double strikePrice, const std::string& expiryDate, double impliedVolatility) {
    std::time_t expiry = parseDate(expiryDate);
    OptionData option = {strikePrice, expiry, impliedVolatility};
    optionsMap[strikePrice].push_back(option);
}

std::vector<OptionData> OptionsMarket::searchOptions(double strikePrice, const std::string& currentDate) const {
    std::vector<OptionData> result;
    auto it = optionsMap.find(strikePrice);
    if (it != optionsMap.end()) {
        std::time_t current = parseDate(currentDate);
        std::time_t threeMonthsLater = getThreeMonthsLater(currentDate);
        
        for (const OptionData& option : it->second) {
            if (option.expiryDate >= current && option.expiryDate <= threeMonthsLater) {
                result.push_back(option);
            }
        }
    }
    return result;
}

std::time_t OptionsMarket::parseDate(const std::string& dateStr) const {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date");
    }
    return std::mktime(&tm);
}

std::time_t OptionsMarket::getThreeMonthsLater(const std::string& dateStr) const {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date");
    }
    tm.tm_mon += 3; // Add 3 months
    return std::mktime(&tm);
}
