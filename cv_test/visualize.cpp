#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <opencv2/opencv.hpp>

// Structure to hold each log entry after parsing
struct LogEntry {
    double time;
    std::string port_name; // We're skipping entries with this field non-empty
    std::pair<int, int> model_position; // Assuming model_name can be parsed into a position
    std::string data;
};

// Function to split a string based on a delimiter and return as pair<int, int>
std::pair<int, int> parsePosition(const std::string& posStr) {
    std::stringstream ss(posStr.substr(1, posStr.length() - 2)); // Remove parentheses
    std::string item;
    std::getline(ss, item, ',');
    int x = std::stoi(item);
    std::getline(ss, item, ',');
    int y = std::stoi(item);
    return {x, y};
}

// Function to load data from a CSV file
std::vector<LogEntry> loadData(const std::string& filePath) {
    std::vector<LogEntry> entries;
    std::ifstream file(filePath);
    std::string line;

    // Assuming the first line is the header and skipping it
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream s(line);
        std::string field;
        LogEntry entry;

        std::getline(s, field, ';');
        
        entry.time = std::stod(field);

        std::getline(s, field, ';'); //I don't care about model_id

        std::getline(s, field, ';');

        entry.model_position = parsePosition(field);

        std::getline(s, entry.port_name, ';');
        if (!entry.port_name.empty()) continue; // Skip rows where 'port_name' is populated

        std::getline(s, field, ';');
        entry.data = field;

        entries.push_back(entry);
    }
    return entries;
}

//Process the color file
std::unordered_map<std::string, cv::Vec3b> loadColor(const std::string& filePath) {

    std::unordered_map<std::string, cv::Vec3b> colorMap;
    std::ifstream file(filePath);
    std::string line;

    while(std::getline(file, line)) {
        std::istringstream s(line);
        std::string field;

        std::getline(s, field, ';');
        std::string state = field;

        std::getline(s, field, ',');
        int g = std::stoi(field);

        std::getline(s, field, ',');
        int b = std::stoi(field);

        std::getline(s, field, ',');
        int r = std::stoi(field);

        colorMap[state] = cv::Vec3b(g, b, r);
    }

    return colorMap;
} 

// Main function
int main() {
    // Load the data
    auto entries = loadData("../bin/grid_log.csv");

    auto colorMap = loadColor("states.col");

    // Extract unique times for frames
    std::set<double> uniqueTimes;
    for (const auto& entry : entries) {
        uniqueTimes.insert(entry.time);
    }
    // Setup the grid and OpenCV window
    cv::namedWindow("Simulation", cv::WINDOW_NORMAL);
    int gridWidth = 20, gridHeight = 20;
    cv::Mat image = cv::Mat::zeros(gridHeight, gridWidth, CV_8UC3);
    image.setTo(cv::Scalar(0, 0, 0)); // Clear the grid

    auto time = uniqueTimes.begin();
    // Process entries for the current time
    for (const auto& entry : entries) {
        if (entry.time == *time) {
            cv::Vec3b color = colorMap[entry.data];
            image.at<cv::Vec3b>(entry.model_position.second, entry.model_position.first) = color;
        }
    }

    cv::imshow("Simulation", image);
    time++;

    while (true) {
        cv::imshow("Simulation", image);
        char key = (char) cv::waitKey(30); // Wait for a key press for 30 ms

        if (key == 27) { // ESC key
            std::cout << "ESC key pressed." << std::endl;
            break;
        } else if (key == 'a') {
            if(time != uniqueTimes.end()) {
                time++;
            }
            // Process entries for the current time
            for (const auto& entry : entries) {
                if (entry.time == *time) {
                    cv::Vec3b color = colorMap[entry.data];
                    image.at<cv::Vec3b>(entry.model_position.second, entry.model_position.first) = color;
                }
            }

            cv::imshow("Simulation", image);

        } else if (key == 'd') {
            if(time != uniqueTimes.begin()){
                time --;
            }

            for(auto rtime : uniqueTimes){

                if(rtime <= *time) {
                    // Process entries for the current time
                    for (const auto& entry : entries) {
                        if (entry.time == rtime) {
                            cv::Vec3b color = colorMap[entry.data];
                            image.at<cv::Vec3b>(entry.model_position.second, entry.model_position.first) = color;
                        }
                    }
                } else {
                    break;
                }

            }

            cv::imshow("Simulation", image);
        } else if (key == 's') {

            while(true) {
                if(time != uniqueTimes.end()){
                    time ++;
                }

                for(auto rtime : uniqueTimes){

                    if(rtime <= *time) {
                        // Process entries for the current time
                        for (const auto& entry : entries) {
                            if (entry.time == rtime) {
                                cv::Vec3b color = colorMap[entry.data];
                                image.at<cv::Vec3b>(entry.model_position.second, entry.model_position.first) = color;
                            }
                        }
                    } else {
                        break;
                    }

                }

                cv::imshow("Simulation", image);
                if(cv::waitKey(200) > 0) {
                    break;
                }
            }
        }
    }

    return 0;
}