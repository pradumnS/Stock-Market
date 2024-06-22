#include "../include/readNSE.hpp"
#include "simdjson.h"
#include "curl/curl.h"


ReadOptionData* ReadOptionData::instance = nullptr;
// std::unordered_map<std::string, DailyStockData>ReadOptionData::JsonData(100);
bool ReadOptionData::initializeOnce = true;
void ReadOptionData::InitialiseReadOptionData()
{
    std::cout<< " InitialiseReadOptionData called "<<std::endl;
     instance  = ReadOptionData::GetInstance();
     instance->ReadJsonFile();
}
ReadOptionData* ReadOptionData::GetInstance()
{
     if (!instance) {
        instance = new ReadOptionData();
    }
    return instance;
}
ReadOptionData::ReadOptionData()
{

}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch(std::bad_alloc &e) {
        // Handle memory problem
        return 0;
    }
    return newLength;
}
std::string fetchUrl(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return readBuffer;
}
void ReadOptionData::ReadData()
{
    std::string url = "https://www.nseindia.com/api/option-chain-indices?symbol=NIFTY";
    
    std::string KafkaMsg = fetchUrl(url);
    simdjson::dom::parser parser1;
    simdjson::dom::element doc1;
    auto error1 = parser1.parse(KafkaMsg).get(doc1);
    if (error1) {
        return;
    }

    simdjson::dom::object json_data1 = doc1.get_object();
    for (const auto& member : json_data1) {
        const auto& expiryDates = std::string(member.key);
        const auto& cam_data = member.value;
        std::cout << cam_data << std::endl;

    }  

        // if (!cam_id.empty() && json_data.find(cam_id) != json_data.end()) {
        //     const simdjson::dom::element& cam_data = json_data[cam_id];

        //     if (cam_data.is_object()) {
                //std::cout << " Yes it is an object : " << std::endl;
}
void ReadOptionData::ReadJsonFile()
{
    simdjson::dom::parser parser;
    simdjson::dom::element document;
    
    // std::ifstream jsonFile(JsonFilePath); //JsonFilePath1
    std::ifstream jsonFile(JsonFilePath);
    std::string jsonDataStr((std::istreambuf_iterator<char>(jsonFile)), std::istreambuf_iterator<char>());
    auto error = parser.parse(jsonDataStr).get(document);

    if (initializeOnce) {
        if (error) {
            std::cerr << "Error parsing JSON file: " << JsonFilePath << std::endl;
            return;
        }

        if (!document.is<simdjson::dom::object>()) {
            std::cerr << "Root of JSON is not an object: " << JsonFilePath << std::endl;
            return;
        }
        for (auto field : document.get_object()) {
            std::string key(field.key);
            // std::cout<< "key: "<<key<<std::endl;
            simdjson::dom::element value = field.value;
            if (value.is<simdjson::dom::object>()) {
                DailyStockData data;
               if (value["Open"].get_double().get(data.Open) ||
                    value["High"].get_double().get(data.High) ||
                    value["Low"].get_double().get(data.Low) ||
                    value["Close"].get_double().get(data.Close) ||
                    value["Shares Traded"].get_double().get(data.SharesTraded) ||
                    value["Turnover (₹ Cr)"].get_double().get(data.Turnover)) {
                    std::cerr << "Error parsing stock data for date: " << key << std::endl;
                    continue; // Skip this entry on error
                }
                // JsonData[key] = data;
                JsonData.emplace_back(key, data);
                // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        double prevClose = 0.0;
        double prevOpen = 0.0;
        double prevLow = 0.0;
        double prevHigh = 0.0;
        for (const auto& pair : JsonData) {
            const auto& date = pair.first;
            const auto& data = pair.second;
            AnalysedData analyseData;

            analyseData.Open = data.Open;
            analyseData.Close = data.Close;
            analyseData.Low = data.Low;
            analyseData.High = data.High;

            analyseData.prevClose_Open = data.Open - prevClose;
            analyseData.open_Close =  data.Close - data.Open;
            analyseData.open_Low = data.Low - data.Open;
            analyseData.open_High = data.High - data.Open;

            analyseData.close_High = data.High - data.Close;
            analyseData.close_Low = data.Low - data.Close;
            analyseData.high_Low = data.High - data.Low;

            analyseData.pOpen_cOpen = data.Open - prevOpen;
            analyseData.pClose_cClose = data.Close - prevClose;
            analyseData.pLow_cLow = data.Low - prevLow;
            analyseData.pHigh_cHigh = data.High - prevHigh;

            prevClose = data.Close;
            prevOpen = data.Open;
            prevLow = data.Low;
            prevHigh = data.High;
            analysedData.emplace_back(date, analyseData);
        }

        for (const auto& pair : analysedData) {
            const auto& date = pair.first;
            const auto& data = pair.second;
        }
    }
    exportToCSV(csvFilePath);
}
/* std::vector<AnalysedData> ReadOptionData::calculateDerivedMetrics() const {
    std::vector<AnalysedData> derivedMetrics;
    auto prevClose = 0.0;
    for (auto it = JsonData.begin(); it != JsonData.end(); ++it) {
        AnalysedData derived;
        std::string date = it->first;
        const auto& data = it->second;
        derived.prevClose_Open = prevClose ? data.Open - prevClose : 0.0;
        derived.open_Close = data.Open - data.Close;
        derived.open_Low = data.Open - data.Low;
        derived.open_High = data.High - data.Open;
        derived.close_Low = data.Close - data.Low;
        derived.close_High = data.High - data.Close;
        derived.high_Low = data.High - data.Low;

        derivedMetrics.push_back(derived);
        prevClose = data.Close;
    }
    return derivedMetrics;
} */


void ReadOptionData::exportToCSV(const std::string& csvFilePath) {
    std::ofstream csvFile(csvFilePath);
    // auto derivedMetrics = calculateDerivedMetrics();
    // Write the header
    // csvFile << "Date, prevClose, openClose, openLow, OpenHigh, closeLow, CloseHigh, highLow \n";
    csvFile << std::setw(12) << "Date"
            << std::setw(8) << "Open"
            << std::setw(8) << "Close"
            << std::setw(8) << "Low"
            << std::setw(8) << "High"
            << std::setw(12) << "pCloseOpen"
            << std::setw(10) << "openClose"
            << std::setw(10) << "openLow"
            << std::setw(10) << "openHigh"
            << std::setw(10) << "clsLow"
            << std::setw(10) << "clsHigh"
            << std::setw(10) << "highLow"
            << std::setw(10) << "p_cOpen"
            << std::setw(10) << "p_cClose"
            << std::setw(10) << "p_cLow"
            << std::setw(10) << "p_cHigh"
            << "\n";

    // Write each entry in the map to the CSV file
    /* for (const auto& data : derivedMetrics) {
        csvFile * << data. << "," 
                << data.prevClose_Open << ","
                << data.open_Close << ","
                << data.open_Low << ","
                << data.open_High << ","
                << data.close_Low << ","
                << data.close_High << ","
                << data.high_Low << "\n";
    } */
    for (const auto& pair : analysedData) {
        const auto& date = pair.first;
        const auto& data = pair.second;
       /*  csvFile << date << ","
                << data.prevClose_Open << ","
                << data.open_Close << ","
                << data.open_Low << ","
                << data.open_High << ","
                << data.close_Low << ","
                << data.close_High << ","
                << data.high_Low<< "\n"; */

        csvFile << std::setw(12) << date

                << std::setw(8) << data.Open
                << std::setw(8) << data.Close
                << std::setw(8) << data.Low
                << std::setw(8) << data.High

                << std::setw(12) << data.prevClose_Open
                << std::setw(10) << data.open_Close
                << std::setw(10) << data.open_Low
                << std::setw(10) << data.open_High
                << std::setw(10) << data.close_Low
                << std::setw(10) << data.close_High
                << std::setw(10) << data.high_Low

                << std::setw(10) << data.pOpen_cOpen
                << std::setw(10) << data.pClose_cClose
                << std::setw(10) << data.pLow_cLow
                << std::setw(10) << data.pHigh_cHigh
                << "\n";
    }
    csvFile.close();
    std::cout << "Data exported to " << csvFilePath << std::endl;
}

    
            /* json_data.clear();
            for (auto field : document.get_object()) {
                std::string key(field.key);
                //std::cout << "Key: " << key << ", Value: ";
                simdjson::dom::element value = field.value;
                json_data[key] = value;

            }
            initializeOnce = false;
            for (const auto& entry : json_data) {
                std::cout << "Key: " << entry.first << ", Value: ";
                simdjson::dom::element value = entry.second;
                std::cout << value << std::endl;
            } */
    /* for (const auto& member : json_data) {
    std::string data = std::string(member.first);
    std::cout << data << std::endl;
    } */
    

       /*  const simdjson::dom::element& cam_data = json_data[data];  //member.second["Open"];
        if (cam_data.is_object()) {
            //std::cout << " Yes it is an object : " << std::endl;
        } else {
            //std::cout << " No, it is not an object : " << std::endl;
        }

        if (cam_data["Open"].get_double()) {  
            double open = cam_data["Open"].get_double();
                       
            std::cout << " open value : - " <<open << std::endl;
        } else {
            //std::cout << " camType is not a string." << std::endl;
        }
        if (cam_data["Close"].get_double()) {  
            double open = cam_data["Close"].get_double();           
            std::cout << " Close value : - " <<open << std::endl;
        } else {
            //std::cout << " camType is not a string." << std::endl;
        }
        if (cam_data["High"].get_double()) {  
            double open = cam_data["High"].get_double();           
            std::cout << " High value : - " <<open << std::endl;
        } else {
            //std::cout << " camType is not a string." << std::endl;
        }
        if (cam_data["Low"].get_double()) {  
            double open = cam_data["Low"].get_double();           
            std::cout << " Low value : - " <<open << std::endl;
        } else {
            //std::cout << " camType is not a string." << std::endl;
        } */
    

/* analysedData[date] = analyseData;
std::cout << "date: " << date << std::endl;
const auto& prevData = JsonData.back().second; // Getting the previous day's data
std::cout<< "curDate: "<<curDate << "prevDate: "<<prevDate<<std::endl;
const auto& curDate = pair.first;
std::cout << "Date: " << date << ", Open: " << data.Open << ", High: " << data.High
      << ", Low: " << data.Low << ", Close: " << data.Close
      << ", Shares Traded: " << data.SharesTraded << ", Turnover: " << data.Turnover
      << std::endl;
std::this_thread::sleep_for(std::chrono::milliseconds(10));
std::cout << "Date: " << date << std::endl << ", prevClose_Open: " << data.prevClose_Open << ", open_Close: " << data.open_Close
      << ", open_Low: " << data.open_Low << ", open_High: " << data.open_High << std::endl
      << ", close_Low: " << data.close_Low << ", close_High: " << data.close_High
      << ", high_Low: " << data.high_Low << std::endl; */