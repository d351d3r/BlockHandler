#include "block_handler.h"

#include <exception>
#include <fstream>
#include <future>
#include <iostream>
#include <array>

BlockHandler::BlockHandler(const std::string& filename)
    : block_device_filename(filename) {}

std::vector<ResponseData> BlockHandler::handle_client_request(
    const std::vector<std::string>& hashes) {

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
    std::ofstream ofs(block_device_filename, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Failed to open file for writing: " + block_device_filename);
    }

    for (std::streamoff i = 0; i < MAX_BLOCKS; i++) {
        uint64_t hash = (i == 1) ? std::hash<std::string>{}(BlockHandler::KNOWN_HASH) : static_cast<uint64_t>(i);
        std::streamsize size = 512 + i;

        ofs.write(reinterpret_cast<char*>(&hash), sizeof(hash));
        if (!ofs) {
            throw std::runtime_error("Failed to write hash for block number: " + std::to_string(i));
        }

        ofs.write(reinterpret_cast<char*>(&size), sizeof(size));
        if (!ofs) {
            throw std::runtime_error("Failed to write size for block number: " + std::to_string(i));
        }

        std::vector<char> data(static_cast<std::size_t>(size), static_cast<char>('A' + (i % 26)));
        ofs.write(data.data(), size);
        if (!ofs) {
            throw std::runtime_error("Failed to write data for block number: " + std::to_string(i));
        }

        // Debug information
        std::cerr << "Written block number: " << i << ", current file size: " << ofs.tellp() << std::endl;
    }

    ofs.close();

    std::ifstream ifs(block_device_filename, std::ios::binary | std::ios::ate);
    std::streamoff expectedTotalSize = compute_total_expected_size();
    std::streamoff actualTotalSize = ifs.tellg();
    if (expectedTotalSize != actualTotalSize) {
        std::cerr << "Mismatch in total file size: expected " << expectedTotalSize
                  << ", but got " << actualTotalSize << "\n";
    }
}



std::streamoff BlockHandler::get_block_size(std::string_view hash) const {
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

std::streamoff BlockHandler::get_block_number(std::string_view hash) const {
  return std::hash<std::string_view>{}(hash) % MAX_BLOCKS;
}

ResponseData BlockHandler::fetch_block_data(std::string_view hash) const {
  std::streamoff block_num = get_block_number(hash);
  std::streamoff block_size = get_block_size(hash);

  std::vector<char> buffer(static_cast<size_t>(block_size));
  if (get_block_data(block_num, buffer.data(), block_size) < 0) {
    // Error occurred while reading block data
    return {"", ""};
  }
  return {std::string(hash), std::string(buffer.begin(), buffer.end())};
}

std::streamoff BlockHandler::test_get_block_number(std::string_view hash) const {
    return get_block_number(hash);
}

std::streamoff BlockHandler::test_get_block_size(std::string_view hash) const {
    return get_block_size(hash);
}

std::streamoff BlockHandler::test_get_block_data(std::streamoff block_num, char* buffer, std::streamoff buffer_size) const {
    return get_block_data(block_num, buffer, buffer_size);
}
