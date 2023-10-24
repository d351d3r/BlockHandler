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

  for (const auto& hash : hashes) {
    futures.push_back(std::async(std::launch::async, [this, hash]() {
      std::streamoff block_num = get_block_number(hash);
      std::streamoff block_size = get_block_size(hash);

      std::vector<char> buffer(static_cast<size_t>(block_size));
      if (get_block_data(block_num, buffer.data(), block_size) < 0) {
        throw std::runtime_error("Failed to fetch block data");
      }

      return ResponseData{hash, std::string(buffer.begin(), buffer.end())};
    }));
  }

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

  for (std::streamoff i = 0; i < MAX_BLOCKS; i++) {
    uint64_t hash;
    if (i == 1) {
      hash = std::hash<std::string>{}("known_hash_1");
    } else {
      hash = static_cast<uint64_t>(i);
    }
    std::streamsize size = 512 + i;
    ofs.write(reinterpret_cast<char*>(&hash), sizeof(hash));
    ofs.write(reinterpret_cast<char*>(&size), sizeof(size));
    std::vector<char> data(static_cast<size_t>(size),
                           static_cast<char>('A' + (i % 26)));

    ofs.write(data.data(), static_cast<std::streamsize>(size));
    if (!ofs) {
      throw std::runtime_error("Error occurred during file write");
    }
  }

  ofs.close();

  std::ifstream ifs(block_device_filename, std::ios::binary | std::ios::ate);
  std::streamoff expectedTotalSize = MAX_BLOCKS * METADATA_SIZE;
  for (std::streamoff i = 0; i < 100; i++) {
    expectedTotalSize += (512 + i);
  }
  std::streamoff actualTotalSize = ifs.tellg();
  if (expectedTotalSize != actualTotalSize) {
    std::cerr << "Mismatch in total file size: expected " << expectedTotalSize
              << ", but got " << actualTotalSize << "\n";
  }
}

std::streamoff BlockHandler::get_block_size(const std::string& hash) const {
  std::streamoff block_num = get_block_number(hash);
  std::ifstream ifs(block_device_filename, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error("Failed to open block device file " +
                             block_device_filename);
  }
  std::streamoff offset = compute_file_offset(block_num);
  ifs.seekg(offset, std::ios::beg);
  uint64_t hash_from_file;
  std::streamsize size;
  ifs.read(reinterpret_cast<char*>(&hash_from_file), sizeof(hash_from_file));
  ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
  if (!ifs) {
    throw std::runtime_error("Error reading block size from block device file");
  }

  // Debug information
  std::cerr << "Block Num: " << block_num << ", File Offset: " << offset
            << ", Hash from File: " << hash_from_file
            << ", Block Size: " << size << std::endl;

  return size;
}

std::streamoff BlockHandler::get_block_data(std::streamoff block_num,
                                            char* buffer,
                                            std::streamoff buffer_size) const {

  std::ifstream ifs(block_device_filename, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error("Failed to open block device file " +
                             block_device_filename);
  }
  std::streamoff offset = compute_file_offset(block_num) + METADATA_SIZE;
  ifs.seekg(offset, std::ios::beg);
  ifs.read(buffer, buffer_size);
  std::streamoff bytesRead = ifs.gcount();
  if (!ifs || bytesRead != buffer_size) {
    throw std::runtime_error("Error reading block data from block device file");
  }

  // Debug information
  std::cerr << "Reading Block Data - Block Num: " << block_num
            << ", File Offset: " << offset
            << ", Bytes Requested: " << buffer_size
            << ", Bytes Read: " << bytesRead << std::endl;

  return 0;
}

std::streamoff BlockHandler::compute_file_offset(
    std::streamoff block_num) const {
  std::streamoff offset = block_num * METADATA_SIZE;
  for (std::streamoff i = 0; i < block_num; i++) {
    offset += (512 + i);
  }
  return offset;
}

std::streamoff BlockHandler::get_block_number(const std::string& hash) const {
  return std::hash<std::string>{}(hash) % MAX_BLOCKS;
}

ResponseData BlockHandler::fetch_block_data(const std::string& hash) const {
  std::streamoff block_num = get_block_number(hash);
  std::streamoff block_size = get_block_size(hash);

  std::vector<char> buffer(static_cast<size_t>(block_size));
  if (get_block_data(block_num, buffer.data(), block_size) < 0) {
    // Error occurred while reading block data
    return {"", ""};
  }
  return {hash, std::string(buffer.begin(), buffer.end())};
}