#include <iostream>
#include <future>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <fstream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include "simdjson.h"


struct DailyStockData {
    double Open;
    double High;
    double Low;
    double Close;
    double SharesTraded; // Assuming large numbers for shares traded
    double Turnover;
};
struct AnalysedData 
{
    int Open;
    int Close;
    int Low;
    int High;

    int prevClose_Open;
    int open_Close;
    int open_Low;
    int open_High;
    int close_Low;
    int close_High;
    int high_Low;

    int pClose_cClose;
    int pOpen_cOpen;
    int pLow_cLow;
    int pHigh_cHigh;
};

class ReadOptionData 
{
    private:
        void ReadData();
        void ReadJsonFile();
        static ReadOptionData* instance;
        void exportToCSV(const std::string& csvFilePath);
        std::string JsonFilePath = "/home/schnellecke_test/data.json";
         std::string JsonFilePath1 = "/home/schnellecke_test/Nifty_9_Year_data.csv";
        std::string csvFilePath = "/home/schnellecke_test/analyzedData.csv";
        static bool initializeOnce;
        std::vector<AnalysedData> calculateDerivedMetrics() const;
        //std::vector<std::unordered_map<std::string, double>>Json_Data;
        // std::unordered_map<std::string, std::vector<int>>Low;
        // std::unordered_map<std::string, std::vector<int>>High;
        std::unordered_map<std::string, simdjson::dom::element> json_data;
        // std::map<std::string, DailyStockData> JsonData;
        std::vector<std::pair<std::string, DailyStockData>> JsonData;
        std::vector<std::pair<std::string, AnalysedData>>analysedData;
        // std::map<std::string, AnalysedData>analysedData;
    public:
        static void InitialiseReadOptionData();
        static ReadOptionData* GetInstance();
        ReadOptionData();
};