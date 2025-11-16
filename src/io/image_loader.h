#ifndef QR_READER_IMAGE_LOADER_H
#define QR_READER_IMAGE_LOADER_H

#include <opencv2/opencv.hpp>
#include <string>

class ImageLoader {
public:
    struct LoadResult {
        bool success;
        cv::Mat image;
        std::string error_msg;
        std::string file_path;
    };

    static LoadResult loadFromFile(const std::string& file_path);

    static LoadResult loadFromWebcam(int camera_index = 0);

    static bool isValidImage(const cv::Mat& image);

    static std::string getImageInfo(const cv::Mat& image);

private:
    static std::string getFileExtension(const std::string& file_path);
    static bool isSupportedFormat(const std::string& extension);
    static LoadResult createErrorResult(const std::string& error_msg, const std::string& file_path = "");
};

#endif // QR_READER_IMAGE_LOADER_H
