/* #include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <curl/curl.h>
// #include <json/json.h>
#include <fstream>

#define StrikePrice 23500
#define MaxStrikePrice 24150
#define MinStrikePrice 22750

struct optionData {
    double last_price;
    double no_loss_ce;
    double no_loss_pe;
    double underlying_value;
    double implicit_value;
};

class StoreData {
public:
    StoreData() {
        fetchOptionsData();
    }

private:
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::string url = "https://www.nseindia.com/api/option-chain-indices?symbol=NIFTY";
    std::unordered_map<std::string, optionData> JsonData;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    void fetchOptionsData() {
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            curl_easy_cleanup(curl);
        }
        parseOptionsData();
    }

    void parseOptionsData() {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(readBuffer, obj);
        
        for (int sPrice = StrikePrice; sPrice <= MaxStrikePrice; sPrice += 50) {
            int count = 0;
            while (count <= 10) {
                std::string expiryDate = "some_logic_to_get_expiry_date"; // You need to implement this part
                std::string strikePriceStr = std::to_string(sPrice);
                parseOptionsData(strikePriceStr, expiryDate, "CE");
                parseOptionsData(strikePriceStr, expiryDate, "PE");
                ++count;
            }
        }
    }

    void parseOptionsData(const std::string& strikePrice, const std::string& expiryDate, const std::string& type) {
        std::string key = strikePrice + "_" + expiryDate + "_" + type;
        
        optionData data;
        // Parse the JSON data here to populate the optionData structure
        // Example: data.last_price = obj["lastPrice"].asDouble();

        // Implement your parsing logic here. This is just a placeholder.
        data.last_price = 0.0;
        data.no_loss_ce = std::stod(strikePrice) + data.last_price;
        data.no_loss_pe = 0.0;
        data.underlying_value = 0.0; // Replace with actual value
        data.implicit_value = 0.0; // Replace with actual value

        JsonData[key] = data;
    }

    void storeJsonData_to_csvFile() {
        std::ofstream file("options_data.csv");
        file << "Key,Last Price,No Loss CE,No Loss PE,Underlying Value,Implicit Value\n";
        for (const auto& pair : JsonData) {
            const auto& key = pair.first;
            const auto& data = pair.second;
            file << key << "," << data.last_price << "," << data.no_loss_ce << "," << data.no_loss_pe << "," << data.underlying_value << "," << data.implicit_value << "\n";
        }
        file.close();
    }
};

int main() {
    StoreData storeData;
    return 0;
}
 */