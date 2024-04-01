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
    double parsed_data;
};

//Helper to print mxn matrix
void printMxN(std::vector<std::vector<double>> data_points) {
    std::cout << "[" << std::endl;
    for(auto data_point : data_points) {
        std::cout << "[ ";
        for(auto point : data_point){
            std::cout << point << ", ";
        }
        std::cout << "]," << std::endl;
    }
    std::cout << "]" << std::endl;
}

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

std::vector<double> parseStates(std::vector<LogEntry> entries) {
    std::vector<std::vector<double>> data_points; //mxn dimensional matrix containing all the state values
    size_t max_num_states = 0;

    //Converting the string from the log file to data points
    for(auto entry : entries) {
        std::vector<double> d_data_point;
        std::string s_data_point = entry.data;
        s_data_point = s_data_point.substr(1, s_data_point.size()-2);

        std::string delimiter = ",";
        size_t pos = 0;
        std::string token;

        while ((pos = s_data_point.find(delimiter)) != std::string::npos) {
            token = s_data_point.substr(0, pos);
            d_data_point.push_back(std::stod(token));
            s_data_point.erase(0, pos + delimiter.length());
        }
        d_data_point.push_back(std::stod(s_data_point));
        
        if(d_data_point.size() > max_num_states) {
            max_num_states = d_data_point.size();
        }

        data_points.push_back(d_data_point);
    }

    std::vector<double> max_states(max_num_states, 0.0);

    for(auto point : data_points) {
        for(size_t i = 0; i < max_num_states; i++) {
            if(max_states[i] < point[i]) {
                max_states[i] = point[i];
            }
        }
    }

    double total = 0.0;
    for(auto itr : max_states) {
        total += itr;
    }

    std::vector<double> new_states;
    auto pos = data_points.begin();
    for(auto entry : entries) {
        double sum = 0.0;
        for(auto itr : *pos) {
            sum += itr;
        }
        sum /= total;
        new_states.push_back(sum);
        pos++;    
    }

    return new_states;
}

// Function to generate a simple gradient color based on a ratio
cv::Vec3b generateColor(double ratio) {
    // This is a simple example: a gradient from blue to red
    uchar red = static_cast<uchar>(ratio * 255);
    uchar blue = 255 - red;
    uchar green = 127 - blue;
    return cv::Vec3b(green, blue, red); // Assuming OpenCV uses BGR format by default
}

//Function to generate the map between states and color
std::unordered_map<double, cv::Vec3b> loadColor(const std::vector<LogEntry> entries) {

    std::unordered_map<double, cv::Vec3b> color_map;
    double max_state = std::numeric_limits<double>::min();
    double min_state = std::numeric_limits<double>::max();

    for(auto entry : entries) {
        if(max_state < entry.parsed_data) {
            max_state = entry.parsed_data;
        }
        if(min_state > entry.parsed_data) {
            min_state = entry.parsed_data;
        }
    }

    // Extract unique times for frames
    std::set<double> unique_states;
    for (const auto& entry : entries) {
        unique_states.insert(entry.parsed_data);
    }

    int num_points = unique_states.size() * sizeof(double);
    double increment = (max_state - min_state)/num_points;

    for(int i = 0; i < num_points; ++i) {
        double dataPoint = min_state + i * increment;
        double ratio = static_cast<double>(i) / (num_points - 1); // Ratio for color gradient
        cv::Vec3b color = generateColor(ratio);
        color_map[dataPoint] = color;
    }

    return color_map;
} 

//Process the color file
// std::unordered_map<std::string, cv::Vec3b> loadColor(const std::string& filePath) {

//     std::unordered_map<std::string, cv::Vec3b> colorMap;
//     std::ifstream file(filePath);
//     std::string line;

//     while(std::getline(file, line)) {
//         std::istringstream s(line);
//         std::string field;

//         std::getline(s, field, ';');
//         std::string state = field;

//         std::getline(s, field, ',');
//         int g = std::stoi(field);

//         std::getline(s, field, ',');
//         int b = std::stoi(field);

//         std::getline(s, field, ',');
//         int r = std::stoi(field);

//         colorMap[state] = cv::Vec3b(g, b, r);
//     }

//     return colorMap;
// } 

// Main function
int main(int argc, char **argv) {

    if(argc < 2) {
        std::cout << "Insufficient arguments." << std::endl;
        std::cout << "Usage: ./visualize [path/to/logfile] [path/to/colourfile]" << std::endl;
        return -1;
    }

    std::cout << "Loading Data" << std::endl;
    // Load the data
    auto entries = loadData(argv[1]);

    std::cout << "Parsing States" << std::endl;
    auto states = parseStates(entries);

    auto pos = states.begin();
    for(auto& itr : entries) {
        itr.parsed_data = *pos;
        pos++;
    }

    

    std::cout << "Generating Colors" << std::endl;
    std::unordered_map<double, cv::Vec3b> colorMap = loadColor(entries);

    // Extract unique times for frames
    std::set<double> uniqueTimes;
    for (const auto& entry : entries) {
        uniqueTimes.insert(entry.time);
    }

    std::cout << "Starting Visualization Window" << std::endl;
    // Setup the grid and OpenCV window
    cv::namedWindow("Simulation", cv::WINDOW_NORMAL);
    int gridWidth = 1000, gridHeight = 1000;
    cv::Mat image = cv::Mat::zeros(gridHeight, gridWidth, CV_8UC3);
    image.setTo(cv::Scalar(0, 0, 0)); // Clear the grid

    auto time = uniqueTimes.begin();
    // Process entries for the current time
    for (const auto& entry : entries) {
        if (entry.time == *time) {
            cv::Vec3b color = colorMap[entry.parsed_data];
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
                    cv::Vec3b color = colorMap[entry.parsed_data];
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
                            cv::Vec3b color = colorMap[entry.parsed_data];
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
                if(time != uniqueTimes.end()) {
                    time++;
                }
                // Process entries for the current time
                for (const auto& entry : entries) {
                    if (entry.time == *time) {
                        cv::Vec3b color = colorMap[entry.parsed_data];
                        image.at<cv::Vec3b>(entry.model_position.second, entry.model_position.first) = color;
                    }
                }

                cv::imshow("Simulation", image);
                if(cv::waitKey(100) > 0) {
                    break;
                }
            }
        }
    }

    return 0;
}