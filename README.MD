# Block Data Retrieval Server 📦

**Сервер высокой производительности, спроектированный для эффективного извлечения блоков данных, идентифицируемых их хешами.**

---

## 📋 Описание проекта

Клиент стремится получить набор блоков данных переменного размера (от 1 Байта до 1 Мегабайта) от сервера. Эти блоки данных идентифицируются по их хешам (например, SHA1, длина 128 байт). 

🔍 **Основные особенности:**
- Клиент отправляет серверу список необходимых хешей блоков.
- В ответ сервер отправляет блоки данных, соответствующие запрошенным хешам, учитывая переменный размер блоков.
- Блоки данных на сервере хранятся на блочном устройстве в случайном порядке.

🛠 **Возможности сервера:**
- Быстро определить номер блока на блочном устройстве на основе хеша.
- Быстро определить реальный размер блока в байтах.
- Эффективно считывать данные с блочного устройства.

### 🎯 Задача:

Цель заключается в том, чтобы наиболее элегантным образом реализовать серверную часть протокола взаимодействия для извлечения блоков данных клиентом, с особым вниманием к производительности.

---

## 💾 Инструкции по сборке:

```bash
sudo apt install libgtest-dev

python3 -m venv conan
source conan/bin/activate
pip install conan

conan profile detect
mkdir build && cd build
conan install .. -of=. --build missing 

cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

ctest --rerun-failed --output-on-failure
./BlockHandler block_device.dat --create
./BlockHandler block_device.dat

```

---

## 🌐 English Version

### 📋 Project Description

A client aims to obtain a set of data blocks of variable size (ranging from 1 Byte to 1 Megabyte) from the server. These data blocks are identified by their hashes (e.g., SHA1, with a length of 128 bytes). 

🔍 **Key Features:**
- The client sends a list of required block hashes to the server.
- In response, the server sends back the data blocks corresponding to the requested hashes, ensuring the support for variable block sizes.
- The data blocks on the server reside on a block device and are arranged randomly.

🛠 **Server Capabilities:**
- Swiftly determine the block number on the block device based on the hash.
- Quickly ascertain the actual size of the block in bytes.
- Efficiently read data from the block device.

### 🎯 Objective:

The objective is to elegantly implement the server-side of the client-server interaction protocol for the client's data block retrieval with a special focus on performance.

---

## 💾 Build Instructions:

```bash
sudo apt install libgtest-dev

python3 -m venv conan
source conan/bin/activate
pip install conan

conan profile detect
mkdir build && cd build
conan install .. -of=. --build missing 

cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

ctest --rerun-failed --output-on-failure
./BlockHandler block_device.dat --create
./BlockHandler block_device.dat

```