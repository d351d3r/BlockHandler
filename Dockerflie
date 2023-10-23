# Используем Debian Bookworm как базовый образ
ARG DEBIAN_VERSION=bookworm
FROM debian:${DEBIAN_VERSION}

LABEL name = "debian-boolworm-llvm"
LABEL maintainer = "Anatolii Cherednichenko <std.anatolii@gmail.com>"
LABEL version = "1.0"
LABEL description = "Docker image in Debian Bookworm for C++ development with LLVM/Clang"

ARG DEBIAN_VERSION=bookworm

ARG LLVM_VERSION=17

ENV DEBIAN_FRONTEND=noninteractive
# Устанавливаем необходимые пакеты для работы с HTTPS и добавления ключа
RUN apt-get update && apt-get install -y --no-install-recommends apt-utils wget gnupg ca-certificates && \
    wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc

# Добавляем репозиторий LLVM
RUN echo "deb http://apt.llvm.org/${DEBIAN_VERSION}/ llvm-toolchain-${DEBIAN_VERSION}-${LLVM_VERSION} main" >> /etc/apt/sources.list && \
    echo "deb-src http://apt.llvm.org/${DEBIAN_VERSION}/ llvm-toolchain-${DEBIAN_VERSION}-${LLVM_VERSION} main" >> /etc/apt/sources.list

# Устанавливаем пакеты LLVM и другие необходимые пакеты
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    clang-${LLVM_VERSION} \
    clang-tidy-${LLVM_VERSION} \
    clang-format-${LLVM_VERSION} \
    clang-tools-${LLVM_VERSION} \
    llvm-${LLVM_VERSION}-dev \
    lld-${LLVM_VERSION} \
    lldb-${LLVM_VERSION} \
    llvm-${LLVM_VERSION}-tools \
    libomp-${LLVM_VERSION}-dev \
    libc++-${LLVM_VERSION}-dev \
    libc++abi-${LLVM_VERSION}-dev \
    libclang-common-${LLVM_VERSION}-dev  \
    libclang-${LLVM_VERSION}-dev \
    libclang-cpp${LLVM_VERSION}-dev \
    libunwind-${LLVM_VERSION}-dev \
    cmake \
    make \
    gdb \
    python3-pip \
    python3-venv \
    libstdc++6 \
    libatomic1 \
    libasan8 \
    libubsan1 \
    liblsan0 \
    libstdc++-11-dev \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m venv /venv && \
    /venv/bin/pip install conan

ENV PATH="/venv/bin:$PATH"

# Создаем профиль по умолчанию для Conan
RUN conan profile detect

WORKDIR /app/build
ENV DEBIAN_FRONTEND newt
