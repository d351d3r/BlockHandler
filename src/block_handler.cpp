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
            size_t block_num = get_block_number(hash);
            std::streamoff block_sz = get_block_size(hash);
            size_t block_size = static_cast<size_t>(block_sz);

            std::vector<char> buffer(block_size);
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
        throw std::runtime_error("Failed to open file for writing: " + block_device_filename);
    }

    for (uint64_t i = 0; i < MAX_BLOCKS; i++) {
        uint64_t hash;
        if (i == 1) {
            hash = std::hash<std::string>{}("known_hash_1");
        } else {
            hash = i;
        }
        std::streamsize size = 512 + static_cast<std::streamsize>(i);
        ofs.write(reinterpret_cast<char*>(&hash), sizeof(hash));
        ofs.write(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<char> data(static_cast<size_t>(size), static_cast<char>('A' + (i % 26)));

        ofs.write(data.data(), size);
        if (!ofs) {
            throw std::runtime_error("Error occurred during file write");
        }
    }

    ofs.close();

    std::ifstream ifs(block_device_filename, std::ios::binary | std::ios::ate);
    std::streamoff expectedTotalSize = static_cast<std::streamoff>(MAX_BLOCKS) * static_cast<std::streamoff>(METADATA_SIZE);
    for (uint64_t i = 0; i < 100; i++) {
        expectedTotalSize += (512 + i);
    }
    std::streamoff actualTotalSize = ifs.tellg();
    if (expectedTotalSize != actualTotalSize) {
        std::cerr << "Mismatch in total file size: expected " << expectedTotalSize
                  << ", but got " << actualTotalSize << "\n";
    }
}

std::streamoff BlockHandler::get_block_size(const std::string& hash) const {
  size_t block_num = get_block_number(hash);
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


int BlockHandler::get_block_data(size_t block_num, char* buffer,
                                 size_t buffer_size) const {
  std::ifstream ifs(block_device_filename, std::ios::binary);
  if (!ifs) {
    throw std::runtime_error("Failed to open block device file " +
                             block_device_filename);
  }
std::streamoff offset = compute_file_offset(block_num) + static_cast<std::streamoff>(METADATA_SIZE);
  ifs.seekg(offset, std::ios::beg);
  ifs.read(buffer, static_cast<std::streamsize>(buffer_size));
  auto bytesRead = ifs.gcount();
  if (!ifs || bytesRead != static_cast<std::streamsize>(buffer_size)) {
    throw std::runtime_error("Error reading block data from block device file");
  }

  // Debug information
  std::cerr << "Reading Block Data - Block Num: " << block_num
            << ", File Offset: " << offset
            << ", Bytes Requested: " << buffer_size
            << ", Bytes Read: " << bytesRead << std::endl;

  return 0;
}

std::streamoff BlockHandler::compute_file_offset(size_t block_num) const {
    std::streamoff offset = static_cast<std::streamoff>(block_num) * static_cast<std::streamoff>(METADATA_SIZE);
    for (size_t i = 0; i < block_num; i++) {
        offset += (512 + i);
    }
    return offset;
}

size_t BlockHandler::get_block_number(const std::string& hash) const {
    return std::hash<std::string>{}(hash) % MAX_BLOCKS;
}

ResponseData BlockHandler::fetch_block_data(const std::string& hash) const {
    size_t block_num = get_block_number(hash);
    std::streamoff block_sz = get_block_size(hash);
    size_t block_size = static_cast<size_t>(block_sz);

    std::vector<char> buffer(block_size);
    int result = get_block_data(block_num, buffer.data(), block_size);
    if (result != 0) {
        // Error occurred while reading block data
        return {"", ""};
    }
    return {hash, std::string(buffer.begin(), buffer.end())};
}
