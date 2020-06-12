#pragma once

#include <random>
#include <string>

class WordGenerator {
    virtual const std::wstring& genWord() = 0;
};

class FileWordGenerator : WordGenerator {
public:
    FileWordGenerator() : FileWordGenerator(std::random_device()()) {}

    explicit FileWordGenerator(uint32_t seed) : gen(seed) {}

    void readFile(const char* path, size_t& maxTriesOut);

    const std::wstring& genWord() override {
        std::uniform_int_distribution<size_t> dist(0, words.size()-1);
        return words[dist(gen)];
    }

private:
    std::vector<std::wstring> words;

    std::mt19937 gen;
};
