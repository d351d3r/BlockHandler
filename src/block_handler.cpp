#include "block_handler.h"

#include <array>
#include <exception>
#include <fstream>
#include <future>
#include <iostream>

BlockHandler::BlockHandler(const std::string& filename)
    : block_device_filename(filename) {
  load_metadata();
}

std::vector<ResponseData> BlockHandler::handle_client_request(const std::vector<std::string>& hashes) {
  std::vector<std::future<ResponseData>> futures;
  std::vector<ResponseData> responses;

  std::array<char, MAX_BLOCK_SIZE> fixed_buffer;

  for (const auto& hash : hashes) {
    futures.push_back(std::async(std::launch::async, [this, &fixed_buffer, hash = std::string_view(hash)]() {
      std::streamoff block_num = get_block_number(hash);
      std::streamoff block_size = get_block_size(hash);

      if (block_size > MAX_BLOCK_SIZE) {
        throw std::runtime_error("Block size exceeds the maximum allowed size");
      }

      if (get_block_data(block_num, fixed_buffer.data(), block_size) < 0) {
        throw std::runtime_error("Failed to fetch block data");
      }

      return ResponseData{std::string(hash), std::string(fixed_buffer.data(), static_cast<std::size_t>(block_size))};
    }));
  }

  for (auto& future : futures) {
    responses.push_back(future.get());
  }

  return responses;
}

void BlockHandler::create_block_device() {
  std::filebuf fbuf;
  if (!fbuf.open(block_device_filename, std::ios::out | std::ios::binary)) {
    throw std::runtime_error("Failed to open file for writing: " + block_device_filename);
  }

  for (std::streamoff i = 0; i < MAX_BLOCKS; i++) {
    uint64_t hash = (i == 1) ? std::hash<std::string>{}(BlockHandler::KNOWN_HASH) : static_cast<uint64_t>(i);
    std::streamsize size = 512 + i;

    fbuf.sputn(reinterpret_cast<const char*>(&hash), sizeof(hash));
    fbuf.sputn(reinterpret_cast<const char*>(&size), sizeof(size));

    std::vector<char> data(static_cast<std::size_t>(size), static_cast<char>('A' + (i % 26)));
    fbuf.sputn(data.data(), size);

    // Debug information
    std::cerr << "Written block number: " << i << ", current file size: " << fbuf.pubseekoff(0, std::ios::cur) << std::endl;
  }

  fbuf.close();
}

std::streamoff BlockHandler::get_block_size(std::string_view hash) const {
  auto itr = metadata_cache.find(std::string(hash));
  if (itr != metadata_cache.end()) {
    return itr->second.second;
  }

  std::ifstream ifs(block_device_filename, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error("Failed to open block device file: " + block_device_filename);
  }

  std::streamoff block_num = get_block_number(hash);
  std::streamoff offset = compute_file_offset(block_num);
  ifs.seekg(offset);

  uint64_t hash_from_file;
  std::streamsize size;
  ifs.read(reinterpret_cast<char*>(&hash_from_file), sizeof(hash_from_file));
  ifs.read(reinterpret_cast<char*>(&size), sizeof(size));

  if (!ifs.good()) {
    throw std::runtime_error("Error reading block size from block device file");
  }

  if (size > MAX_BLOCK_SIZE) {
    throw std::runtime_error("Block size exceeds the maximum allowed size");
  }

  return size;
}

std::streamoff BlockHandler::compute_total_expected_size() const {
  std::streamoff totalSize = MAX_BLOCKS * METADATA_SIZE;
  for (std::streamoff i = 0; i < MAX_BLOCKS; i++) {
    totalSize += (512 + i);
  }
  return totalSize;
}

std::streamoff BlockHandler::get_block_data(std::streamoff block_num, char* buffer, std::streamoff buffer_size) const {
  std::ifstream ifs(block_device_filename, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error("Failed to open block device file: " + block_device_filename);
  }

  std::streamoff offset = compute_file_offset(block_num) + METADATA_SIZE;
  ifs.seekg(offset);
  ifs.read(buffer, buffer_size);

  if (ifs.gcount() != buffer_size) {
    throw std::runtime_error("Error reading block data from block device file");
  }

  return 0;
}

std::streamoff BlockHandler::compute_file_offset(std::streamoff block_num) const {
  std::streamoff offset = block_num * METADATA_SIZE;
  for (std::streamoff i = 0; i < block_num; i++) {
    offset += (512 + i);
  }

  return offset;
}

std::streamoff BlockHandler::get_block_number(std::string_view hash) const {
  return std::hash<std::string_view>{}(hash) % MAX_BLOCKS;
}

ResponseData BlockHandler::fetch_block_data(std::string_view hash) const {
  std::streamoff block_num = get_block_number(hash);
  std::streamoff block_size = get_block_size(hash);

  std::vector<char> buffer(static_cast<size_t>(block_size));
  if (get_block_data(block_num, buffer.data(), block_size) < 0) {
    return {"", ""};
  }

  return {std::string(hash), std::string(buffer.begin(), buffer.end())};
}

void BlockHandler::load_metadata() {
  std::ifstream ifs(block_device_filename, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error("Failed to open block device file: " + block_device_filename);
  }

  for (std::streamoff i = 0; i < MAX_BLOCKS; i++) {
    uint64_t hash;
    std::streamsize size;

    ifs.read(reinterpret_cast<char*>(&hash), sizeof(hash));
    ifs.read(reinterpret_cast<char*>(&size), sizeof(size));

    metadata_cache[std::to_string(hash)] = {i, size};
  }
}
std::streamoff BlockHandler::test_get_block_number(std::string_view hash) const {
    return get_block_number(hash);
}

std::streamoff BlockHandler::test_get_block_data(std::streamoff block_num, char* buffer, std::streamoff buffer_size) const {
    return get_block_data(block_num, buffer, buffer_size);
}
