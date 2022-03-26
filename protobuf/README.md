# Protobuf-cpp (https://github.com/protocolbuffers/protobuf/releases)
Protocol Buffers (a.k.a., protobuf) are Google's language-neutral, platform-neutral, extensible mechanism for serializing structured data.
# Building and Run
``` 
    1. mkdir Protobuf-cpp && cd Protobuf-cpp
    2. unzip protobuf.zip -d Protobuf-cpp/
    3. mkdir Protobuf-cpp/Debug && cd Protobuf-cpp/Debug
    4. cmake .. -DCMAKE_BUILD_TYPE=Debug -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=.
    5. cmake --build .
    6. cmake --install .
    7. mkdir Protobuf-cpp/Release && cd Protobuf-cpp/Release
    8. cmake .. -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=.
    9. cmake --build .
    10. cmake --install .
```
# System requirements and Stack
  1. C++17
  2. GCC version 8.1.0
  3. Cmake 3.21.2 (minimal 3.10)
