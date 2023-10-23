
# Block Data Retrieval Server

A high-performance server designed to efficiently handle the retrieval of data blocks identified by their hashes.

## Project Description

A client aims to obtain a set of data blocks of variable size (ranging from 1 Byte to 1 Megabyte) from the server. These data blocks are identified by their hashes (e.g., SHA1, with a length of 128 bytes). The client sends a list of required block hashes to the server. In response, the server sends back the data blocks corresponding to the requested hashes, ensuring the support for variable block sizes. The data blocks on the server reside on a block device and are arranged randomly.

### Server Capabilities:
- Swiftly determine the block number on the block device based on the hash using the function: `size_t get_block_number(const string& hash)`.
- Quickly ascertain the actual size of the block in bytes using the function: `size_t get_block_size(const string& hash)`.
- Efficiently read data from the block device with the function: `int get_block_data(size_t block_num, char* buffer, size_t buffer_size)`.

### Objective:

Elegantly implement the server-side of the client-server interaction protocol for the client's data block retrieval. Ensure that the system can handle a large number of block requests with a special focus on performance.

## Features

1. **Block Identification by Hash**:
   - Utilizes `get_block_number` to determine the block's number based on the provided hash.
   - Employs `get_block_size` to discern the block's size.

2. **Block Data Retrieval**:
   - Leverages asynchronous reading (`io_uring`) for block data retrieval, ensuring peak performance, especially when handling a multitude of block requests.

3. **Support for Variable Block Sizes**:
   - Each block is processed individually, accounting for its size to prevent buffer overflows or under-allocation.

4. **Parallel Processing for Enhanced Performance**:
   - Incorporates Threading Building Blocks (TBB) for concurrent block processing, maximizing CPU core utilization.

5. **Support for a Large Number of Requests**:
   - The combination of asynchronous reading and parallel processing allows efficient handling of a vast number of block requests.

6. **Linux-specific Optimizations**:
   - Direct I/O and other Linux optimizations enable data reading with utmost efficiency by bypassing filesystem caching.

## Setup and Build

Ensure you have `Conan` and `CMake` installed.

1. Clone the repository.
2. Navigate to the project root directory.
3. Install dependencies:
   ```bash
   conan install . --build=missing
   ```
4. Create a build directory and navigate to it:
   ```bash
   mkdir build
   cd build
   ```
5. Build the project:
   ```bash
   cmake ..
   cmake --build .
   ```

## Usage

Execute the built `BlockHandler` binary to run the server.

## Note

For a complete server-side protocol implementation, consider adding the network component (accepting client requests, sending responses, etc.). The provided code primarily focuses on data block processing, not network interactions. For assistance in adding the network component, refer to additional documentation or seek expert guidance.
