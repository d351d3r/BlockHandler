#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

#include <string>
#include <vector>

// Struct to represent the response data for a given hash
struct ResponseData {
  std::string hash;
  std::string data;  // Assuming data is represented as a string for simplicity
};

class BlockHandler {
 public:
  BlockHandler() = default;
  // Special methods for testing purposes
  size_t test_get_block_number(const std::string& hash) const {
    return get_block_number(hash);
  }

  size_t test_get_block_size(const std::string& hash) const {
    return get_block_size(hash);
  }

  int test_get_block_data(size_t block_num, char* buffer,
                          size_t buffer_size) const {
    return get_block_data(block_num, buffer, buffer_size);
  }

  ResponseData fetch_block_data(const std::string& hash) const {
    size_t block_num = get_block_number(hash);
    size_t block_size = get_block_size(hash);
    
    std::vector<char> buffer(block_size);
    int result = get_block_data(block_num, buffer.data(), block_size);
    if (result != 0) {
        // Error occurred while reading block data
        return {"", ""};
    }
    return {hash, std::string(buffer.begin(), buffer.end())};
}


  // Constructor that initializes the block device filename
  BlockHandler(const std::string& filename);

  // Function to handle client requests
  std::vector<ResponseData> handle_client_request(
      const std::vector<std::string>& hashes);

  // Function to create a block device
  void create_block_device();

 private:
  // Helper function to compute the file offset for a given block number
  std::streamoff compute_file_offset(size_t block_num) const;

  // Helper function to get a block number for a given hash
  size_t get_block_number(const std::string& hash) const;

  // Helper function to get block size for a given hash
  std::streamoff get_block_size(const std::string& hash) const;

  // Helper function to get block data for a given block number
  int get_block_data(size_t block_num, char* buffer, size_t buffer_size) const;

  // Member variable to store the block device filename
  std::string block_device_filename = "block_device.dat";
};

#endif  // BLOCK_HANDLER_H
