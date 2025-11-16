#ifndef QR_READER_IMAGE_PROCESSOR_H
#define QR_READER_IMAGE_PROCESSOR_H

#include <opencv2/opencv.hpp>

class ImageProcessor {
public:
    static cv::Mat enhanceForQRDetection(const cv::Mat& image);

    static cv::Mat convertToGrayscale(const cv::Mat& image);
    static cv::Mat enhanceContrast(const cv::Mat& image);
    static cv::Mat removeNoise(const cv::Mat& image);
    static cv::Mat sharpenImage(const cv::Mat& image);
    static cv::Mat resizeImage(const cv::Mat& image, int min_size = 500);
    static cv::Mat adjustBrightness(const cv::Mat& image, double alpha = 1.0, int beta = 0);

    static bool needsEnhancement(const cv::Mat& image);
    static double calculateQualityScore(const cv::Mat& image);

private:
    static cv::Mat applyCLAHE(const cv::Mat& image);
    static cv::Mat applyBilateralFilter(const cv::Mat& image);
};

#endif // QR_READER_IMAGE_PROCESSOR_H
