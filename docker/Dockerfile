FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    software-properties-common \
    wget

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-18 main" && \
    apt-get update && \
    apt-get install -y \
    build-essential \
    libgtest-dev \
    cmake \
    python3-pip \
    git \
    cppcheck \
    lcov \
    valgrind \
    clang-format-18 \
    clang-tidy-18 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /project

COPY CMakeLists.txt ./

COPY . .

RUN cmake -B build -S .

RUN cmake --build build

#CMD ["./build/s21_test_rb_tree"]
WORKDIR /project/build

# Запускаем ctest из папки build
CMD ["ctest", "--output-on-failure"]
