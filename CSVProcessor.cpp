#include "CSVProcessor.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <omp.h>

CSVProcessor::CSVProcessor(const std::string& inputFile, const std::string& outputFile)
    : inputFile(inputFile), outputFile(outputFile), statsFile("processed_stats.txt"), totalRows(0) {}

void CSVProcessor::processFile() {
    std::ifstream file(inputFile);
    std::ofstream out(outputFile), statsOut(statsFile);
    std::string row;
    std::vector<std::string> headers;
    std::vector<int> globalEmptyCounts(NUM_FIELDS, 0);
    std::vector<std::string> lines; // To store all lines for processing
    std::vector<bool> validFields(NUM_FIELDS, true); // To mark fields with less than 50% missing data as valid

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << inputFile << std::endl;
        return;
    }

    // Read and store headers
    if (getline(file, row)) {
        std::istringstream headerStream(row);
        std::string header;
        while (getline(headerStream, header, ',')) {
            headers.push_back(header);
        }
    }

    // Store all lines for processing
    while (getline(file, row)) {
        lines.push_back(row);
    }
    file.close(); // Close the file as it's no longer needed for direct reading

    // Update totalRows based on the number of lines read
    totalRows = lines.size();

    // Parallel processing to count empty fields
    #pragma omp parallel
    {
        std::vector<int> localEmptyCounts(NUM_FIELDS, 0);
        #pragma omp for nowait
        for (size_t i = 0; i < lines.size(); ++i) {
            std::istringstream s(lines[i]);
            std::string field;
            size_t fieldIndex = 0;
            while (getline(s, field, ',') && fieldIndex < NUM_FIELDS) {
                if (field.empty() || field == "NA") {
                    localEmptyCounts[fieldIndex]++;
                }
                ++fieldIndex;
            }
        }

        #pragma omp critical
        for (size_t i = 0; i < NUM_FIELDS; ++i) {
            globalEmptyCounts[i] += localEmptyCounts[i];
        }
    }

    // Calculate percentage of missing data and update validFields accordingly
    for (size_t i = 0; i < NUM_FIELDS; ++i) {
        double percentageMissing = static_cast<double>(globalEmptyCounts[i]) / totalRows * 100;
        validFields[i] = percentageMissing < 50.0; // Mark field as valid if missing data is less than 50%
        statsOut << headers[i] << "," << globalEmptyCounts[i] << "," << percentageMissing << "%," << (validFields[i] ? "Kept" : "Removed") << "\n";
    }

    // Rewrite the file with only valid fields
    file.open(inputFile); // Re-open the file for reading
    getline(file, row); // Skip the original header row

    // Write valid headers to the output file
    bool isFirst = true;
    for (size_t i = 0; i < NUM_FIELDS; ++i) {
        if (validFields[i]) {
            if (!isFirst) {
                out << ",";
            } else {
                isFirst = false;
            }
            out << headers[i];
        }
    }
    out << "\n";

    // Filter rows based on validFields
    while (getline(file, row)) {
        std::istringstream s(row);
        std::string field;
        size_t fieldIndex = 0;
        std::string newRow;
        isFirst = true;
        while (getline(s, field, ',') && fieldIndex < NUM_FIELDS) {
            if (validFields[fieldIndex]) {
                if (!isFirst) {
                    newRow += ",";
                } else {
                    isFirst = false;
                }
                newRow += field;
            }
            ++fieldIndex;
        }
        out << newRow << "\n";
    }

    std::cout << "Filtered data saved to " << outputFile << std::endl;
    std::cout << "Field statistics saved to " << statsFile << std::endl;


}
