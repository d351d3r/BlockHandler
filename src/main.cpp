#include <iostream>
#include <vector>
#include <cstdint>
#include "block_handler.h"

int main(int argc, char* argv[]) {
  // Check for the minimum number of arguments
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0]
              << " <path to block data file> [--create]\n";
    return 1;
  }

  std::string block_data_file_path = argv[1];
  bool create_device = (argc == 3 && std::string(argv[2]) == "--create");

  BlockHandler handler(block_data_file_path);

  if (create_device) {
    handler.create_block_device();
    std::cout << "Block device created at " << block_data_file_path << "\n";
    return 0;  // Exit after creating the device
  }

  std::vector<std::string> sample_hashes = {
      "365a4a205c01386b16b3dc1eb2d89b5c2185247f",
      "83ad8510bbd3f22363d068e1c96f82fd0fcccd31",
      "d3eba3caf81627832cbac3fcd8ad9fefc57b3398"};

  try {
    auto responses = handler.handle_client_request(sample_hashes);

    // Print results for demonstration
    for (const auto& response : responses) {
      std::cout << "Hash: " << response.hash << "\n"
                << "Data: " << response.data << "\n"
                << "-------------------------\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 2;
  }

  return 0;
}
