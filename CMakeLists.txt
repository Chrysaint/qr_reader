cmake_minimum_required(VERSION 3.16)
project(QRReader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(qr_reader ${SOURCES} ${HEADERS})

target_sources(qr_reader
    PRIVATE
        src/core/qr_detector.cpp
        src/processors/image_processor.cpp
        src/io/image_loader.cpp
        src/io/result_writer.cpp
        src/utils/logger.cpp
        src/main.cpp
)

target_include_directories(qr_reader PRIVATE src)

target_link_libraries(qr_reader ${OpenCV_LIBS})
