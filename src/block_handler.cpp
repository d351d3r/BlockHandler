#include "block_handler.h"

#include <exception>
#include <fstream>
#include <future>
#include <iostream>

BlockHandler::BlockHandler(const std::string& filename)
    : block_device_filename(filename) {}

std::vector<ResponseData> BlockHandler::handle_client_request(
    const std::vector<std::string>& hashes) {
  std::vector<std::future<ResponseData>> futures;
  std::vector<ResponseData> responses;

  // Asynchronously fetch block data for each hash
  for (const auto& hash : hashes) {
    futures.push_back(std::async(std::launch::async, [this, hash]() {
        size_t block_num = get_block_number(hash);
        size_t block_size = get_block_size(hash);

        std::vector<char> buffer(block_size);
        if (get_block_data(block_num, buffer.data(), block_size) < 0) {
            throw std::runtime_error("Failed to fetch block data");
        }

        return ResponseData{hash, std::string(buffer.begin(), buffer.end())};
    }));
}


  // Gather responses
  for (auto& future : futures) {
    responses.push_back(future.get());
  }

  return responses;
}

void BlockHandler::create_block_device() {
  std::ofstream ofs(block_device_filename, std::ios::binary);
  if (!ofs) {
    throw std::runtime_error("Failed to open file for writing: " +
                             block_device_filename);
  }

  for (int i = 0; i < 100; i++) {
 //   uint64_t hash = std::hash<std::string>{}("known_hash_" + std::to_string(i));
 uint64_t hash;
if (i == 1) {
    hash = std::hash<std::string>{}("known_hash_1");
} else {
    hash = i;
}
    uint64_t size = 512 + i;
    ofs.write(reinterpret_cast<char*>(&hash), sizeof(hash));
    ofs.write(reinterpret_cast<char*>(&size), sizeof(size));
    std::vector<char> data(size, static_cast<char>('A' + (i % 26)));

    ofs.write(data.data(), size);
    if (!ofs) {
      throw std::runtime_error("Error occurred during file write");
    }
  }

  ofs.close();

  // Check the total file size
  std::ifstream ifs(block_device_filename, std::ios::binary | std::ios::ate);
  size_t expectedTotalSize = 100 * 16;  // Metadata for each block
  for (int i = 0; i < 100; i++) {
    expectedTotalSize += (512 + i);
  }
  size_t actualTotalSize = ifs.tellg();
  if (expectedTotalSize != actualTotalSize) {
    std::cerr << "Mismatch in total file size: expected " << expectedTotalSize
              << ", but got " << actualTotalSize << "\n";
  }
}

size_t BlockHandler::get_block_size(const std::string& hash) const {
    size_t block_num = get_block_number(hash);
    std::ifstream ifs(block_device_filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open block device file " +
                                 block_device_filename);
    }
    auto offset = compute_file_offset(block_num);
    ifs.seekg(offset, std::ios::beg);
    uint64_t hash_from_file;
    uint64_t size;
    ifs.read(reinterpret_cast<char*>(&hash_from_file), sizeof(hash_from_file));
    ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!ifs) {
        throw std::runtime_error("Error reading block size from block device file");
    }

    // Debug information
    std::cerr << "Block Num: " << block_num << ", File Offset: " << offset << ", Hash from File: " << hash_from_file << ", Block Size: " << size << std::endl;

    return size;
}

int BlockHandler::get_block_data(size_t block_num, char* buffer,
                                 size_t buffer_size) const {
    std::ifstream ifs(block_device_filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open block device file " +
                                 block_device_filename);
    }
    auto offset = compute_file_offset(block_num) + 16;
    ifs.seekg(offset, std::ios::beg);
    ifs.read(buffer, buffer_size);
    auto bytesRead = ifs.gcount();
    if (!ifs || bytesRead != static_cast<std::streamsize>(buffer_size)) {
        throw std::runtime_error("Error reading block data from block device file");
    }

    // Debug information
    std::cerr << "Reading Block Data - Block Num: " << block_num << ", File Offset: " << offset << ", Bytes Requested: " << buffer_size << ", Bytes Read: " << bytesRead << std::endl;

    return 0;
}

size_t BlockHandler::compute_file_offset(size_t block_num) const {
  size_t offset = block_num * 16;  // Metadata for each block
  for (size_t i = 0; i < block_num; i++) {
    offset += (512 + i);
  }
  return offset;
}

size_t BlockHandler::get_block_number(const std::string& hash) const {
  return std::hash<std::string>{}(hash) % 100;
}
