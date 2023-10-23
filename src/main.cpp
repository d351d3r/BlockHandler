#include <iostream>
#include <vector>
#include "block_handler.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path to block data file> [--create]\n";
        return 1;
    }

    std::string block_data_file_path = argv[1];
    bool create_device = false;

    if (argc == 3 && std::string(argv[2]) == "--create") {
        create_device = true;
    }

    BlockHandler handler(block_data_file_path);

    if (create_device) {
        handler.create_block_device();
        std::cout << "Block device created at " << block_data_file_path << "\n";
        return 0;  // Exit after creating the device
    }

    std::vector<std::string> sample_hashes = {"sample_hash_1", "sample_hash_2","sample_hash_3"};

    try {
        auto responses = handler.handle_client_request(sample_hashes);

        // Printing the results for demonstration
        for (const auto& response : responses) {
            std::cout << "Hash: " << response.hash << "\n";
            std::cout << "Data: " << response.data << "\n";
            std::cout << "-------------------------\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}
