#include "image_loader.h"
#include "../utils/logger.h"
#include <filesystem>

const std::vector<std::string> SUPPORTED_FORMATS = {
    ".jpg", ".jpeg", ".png", ".bmp", ".tiff", ".tif", ".webp"
};

ImageLoader::LoadResult ImageLoader::loadFromFile(const std::string& file_path) {
    Logger::startOperation("Loading image from file: " + file_path);

    if (!std::filesystem::exists(file_path)) {
        Logger::error("File does not exist: " + file_path);
        return createErrorResult("File does not exist: " + file_path, file_path);
    }

    std::string extension = getFileExtension(file_path);
    if (!isSupportedFormat(extension)) {
        Logger::error("Unsupported image format: " + extension);
        return createErrorResult("Unsupported image format: " + extension, file_path);
    }

    cv::Mat image = cv::imread(file_path, cv::IMREAD_COLOR);

    if (image.empty()) {
        Logger::error("Failed to load image (may be corrupted): " + file_path);
        return createErrorResult("Failed to load image (file may be corrupted)", file_path);
    }

    Logger::info("Image loaded successfully: " + getImageInfo(image));
    Logger::endOperation("Loading image from file");

    return {true, image, "", file_path};
}

ImageLoader::LoadResult ImageLoader::loadFromWebcam(int camera_index) {
    Logger::startOperation("Loading image from webcam (device " + std::to_string(camera_index) + ")");

    cv::VideoCapture cap(camera_index);

    if (!cap.isOpened()) {
        Logger::error("Failed to open webcam device: " + std::to_string(camera_index));
        return createErrorResult("Failed to open webcam device: " + std::to_string(camera_index));
    }

    cv::Mat frame;
    cap >> frame;
    cap.release();

    if (frame.empty()) {
        Logger::error("Failed to capture frame from webcam");
        return createErrorResult("Failed to capture frame from webcam");
    }

    Logger::info("Webcam image captured: " + getImageInfo(frame));
    Logger::endOperation("Loading image from webcam");

    return {true, frame, "", "webcam_device_" + std::to_string(camera_index)};
}

bool ImageLoader::isValidImage(const cv::Mat& image) {
    return !image.empty() && image.cols > 0 && image.rows > 0 && image.data != nullptr;
}

std::string ImageLoader::getImageInfo(const cv::Mat& image) {
    if (!isValidImage(image)) {
        return "Invalid image";
    }

    std::string type;
    switch (image.type()) {
        case CV_8UC1: type = "Grayscale"; break;
        case CV_8UC3: type = "BGR Color"; break;
        case CV_8UC4: type = "BGRA Color"; break;
        default: type = "Unknown type"; break;
    }

    return std::to_string(image.cols) + "x" +
           std::to_string(image.rows) + ", " +
           type + ", " +
           "Channels: " + std::to_string(image.channels());
}

std::string ImageLoader::getFileExtension(const std::string& file_path) {
    size_t dot_pos = file_path.find_last_of(".");
    if (dot_pos == std::string::npos) {
        return "";
    }

    std::string extension = file_path.substr(dot_pos);

    for (char& c : extension) {
        c = std::tolower(c);
    }

    return extension;
}

bool ImageLoader::isSupportedFormat(const std::string& extension) {
    for (const auto& supported : SUPPORTED_FORMATS) {
        if (extension == supported) {
            return true;
        }
    }
    return false;
}

ImageLoader::LoadResult ImageLoader::createErrorResult(const std::string& error_msg, const std::string& file_path) {
    return {false, cv::Mat(), error_msg, file_path};
}
