#ifndef OPTIONS_MARKET_HPP
#define OPTIONS_MARKET_HPP

#include <string>
#include <vector>
#include <map>
#include <ctime>

struct OptionData {
    double strikePrice;
    std::time_t expiryDate;
    double impliedVolatility;
};

class OptionsMarket {
public:
    OptionsMarket();
    
    // Adds an option to the market
    void addOption(double strikePrice, const std::string& expiryDate, double impliedVolatility);

    // Searches for options by strike price and returns those within the next 3 months
    std::vector<OptionData> searchOptions(double strikePrice, const std::string& currentDate) const;

private:
    std::map<double, std::vector<OptionData>> optionsMap;
    
    // Helper method to parse date string to std::time_t
    std::time_t parseDate(const std::string& dateStr) const;

    // Helper method to get the time_t representation of the date 3 months from now
    std::time_t getThreeMonthsLater(const std::string& dateStr) const;
};

#endif // OPTIONS_MARKET_HPP
