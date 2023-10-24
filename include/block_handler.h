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
  std::streamoff test_get_block_number(const std::string& hash) const {
    return get_block_number(hash);
  }

  std::streamoff test_get_block_size(const std::string& hash) const {
    return get_block_size(hash);
  }

  std::streamoff test_get_block_data(std::streamoff block_num, char* buffer,
                                     std::streamoff buffer_size) const {
    return get_block_data(block_num, buffer, buffer_size);
  }

  ResponseData fetch_block_data(const std::string& hash) const;

  // Constructor that initializes the block device filename
  BlockHandler(const std::string& filename);

  // Function to handle client requests
  std::vector<ResponseData> handle_client_request(
      const std::vector<std::string>& hashes);

  // Function to create a block device
  void create_block_device();

 private:
  // Helper function to compute the file offset for a given block number
  std::streamoff compute_file_offset(std::streamoff block_num) const;

  // Helper function to get a block number for a given hash
  std::streamoff get_block_number(const std::string& hash) const;

  // Helper function to get block size for a given hash
  std::streamoff get_block_size(const std::string& hash) const;

  // Helper function to get block data for a given block number
  std::streamoff get_block_data(std::streamoff block_num, char* buffer,
                                std::streamoff buffer_size) const;

  static constexpr std::streamoff MAX_BLOCKS = 100;
  static constexpr std::streamoff METADATA_SIZE = 16;

  // Constants
  static const inline std::string DEFAULT_BLOCK_DEVICE_FILENAME =
      "block_device.dat";

  // Member variable
  std::string block_device_filename = DEFAULT_BLOCK_DEVICE_FILENAME;
};

#endif  // BLOCK_HANDLER_H