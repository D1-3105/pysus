#include "ThreadHolder.h"
#include "iostream"
#include "fstream"
#include "sstream"
#include "filesystem"

int main (int argc, const char * argv[]) {
    if (argc != 3) {
        throw std::runtime_error("pass python script directly!");
    }
    std::filesystem::path pythonScriptPath = argv[1];
    std::filesystem::path setCwd = argv[2];

    if (pythonScriptPath.extension() != ".py" or !exists(pythonScriptPath)) {
        throw std::runtime_error("python file expected!");
    }

    if (!exists(setCwd)) {
        throw std::runtime_error("working dir does not exist!");
    }

    std::ifstream file(pythonScriptPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string pythonCode = buffer.str();

    std::filesystem::current_path(setCwd);

    auto holder = threader::ThreadHolder(pythonCode);
}