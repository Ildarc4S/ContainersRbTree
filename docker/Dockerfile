FROM ubuntu:25.04

RUN apt-get update && apt-get install -y wget gnupg2 software-properties-common

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] http://apt.llvm.org/mantic/ llvm-toolchain-mantic-18 main" > /etc/apt/sources.list.d/llvm.list && \
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

#RUN cmake -DCOVERAGE=ON -B build -S .
#RUN cmake --build build

#WORKDIR /project/build

CMD ["/bin/bash"]
