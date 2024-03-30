#ifndef CSVPROCESSOR_H
#define CSVPROCESSOR_H

#include <string>
#include <vector>

class CSVProcessor {
public:
    CSVProcessor(const std::string& inputFile, const std::string& outputFile);
    void processFile();
    void imputeMissingValues();
    void removeDuplicates();
    static constexpr size_t NUM_FIELDS = 43;

private:
    std::string inputFile;
    std::string outputFile;
    std::string statsFile = "empty_field_counts.txt";
    size_t totalRows = 0;

    std::vector<int> countEmptyFieldsPerColumn(const std::vector<std::string>& chunk);
    bool isValidRow(const std::string& row);
};

#endif // CSVPROCESSOR_H
