#ifndef BLOCK_HANDLER_H
#define BLOCK_HANDLER_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <cstdint>

// Struct to represent the response data for a given hash
struct ResponseData {
  std::string hash;
  std::string data;  // Assuming data is represented as a string for simplicity
};

class BlockHandler {
 public:
  BlockHandler() = default;

  // Special methods for testing purposes
  std::streamoff test_get_block_number(std::string_view hash) const;
  std::streamoff test_get_block_size(std::string_view hash) const;
  std::streamoff test_get_block_data(std::streamoff block_num, char* buffer,
                                     std::streamoff buffer_size) const;

  ResponseData fetch_block_data(std::string_view hash) const;

  // Constructor that initializes the block device filename
  BlockHandler(const std::string& filename);

  // Function to handle client requests
  std::vector<ResponseData> handle_client_request(
      const std::vector<std::string>& hashes);

  // Function to create a block device
  void create_block_device();
  void load_metadata();

 private:
  std::string block_device_filename;

  // Helper function to compute the file offset for a given block number
  std::streamoff compute_file_offset(std::streamoff block_num) const;

  std::streamoff compute_total_expected_size() const;

  // Helper function to get a block number for a given hash
  std::streamoff get_block_number(std::string_view hash) const;

  // Helper function to get block size for a given hash
  std::streamoff get_block_size(std::string_view hash) const;

  // Helper function to get block data for a given block number
  std::streamoff get_block_data(std::streamoff block_num, char* buffer,
                                std::streamoff buffer_size) const;

    std::unordered_map<std::string, std::pair<uint64_t, std::streamsize>> metadata_cache;

  // Constants
  static constexpr std::streamoff MAX_BLOCKS = 100;
  //static constexpr std::streamoff METADATA_SIZE = 16;
  static constexpr std::streamoff METADATA_SIZE =
      sizeof(uint64_t) + sizeof(std::streamsize);

  static constexpr std::streamoff MAX_BLOCK_SIZE = 1 * 1024 * 1024;  // 1 MB

  static const inline std::string KNOWN_HASH = "known_hash_1";
  static const inline std::string DEFAULT_BLOCK_DEVICE_FILENAME =
      "block_device.dat";

  // Member variable
};

#endif  // BLOCK_HANDLER_H