#include "image_processor.h"
#include "../utils/logger.h"

cv::Mat ImageProcessor::enhanceForQRDetection(const cv::Mat& image) {
    Logger::startOperation("Enhancing image for QR detection");

    if (image.empty()) {
        return image;
    }

    cv::Mat processed = image.clone();

    if (processed.channels() > 1) {
        cv::cvtColor(processed, processed, cv::COLOR_BGR2GRAY);
    }

    cv::Mat binary;
    cv::threshold(processed, processed, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    processed = enhanceContrast(processed);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(processed, processed, cv::MORPH_CLOSE, kernel);

    if (std::min(processed.rows, processed.cols) < 300) {
        double scale = 600.0 / std::min(processed.rows, processed.cols);
        cv::resize(processed, processed, cv::Size(), scale, scale, cv::INTER_CUBIC);
    }

    Logger::endOperation("Enhancing image for QR detection");
    return processed;
}

cv::Mat ImageProcessor::convertToGrayscale(const cv::Mat& image) {
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    return gray;
}

cv::Mat ImageProcessor::enhanceContrast(const cv::Mat& image) {
    cv::Mat enhanced;

    if (image.channels() == 1) {
        enhanced = applyCLAHE(image);
    } else {
        image.convertTo(enhanced, -1, 1.3, 0);
    }

    return enhanced;
}

cv::Mat ImageProcessor::removeNoise(const cv::Mat& image) {
    cv::Mat denoised;

    if (image.channels() == 1) {
        cv::medianBlur(image, denoised, 3);
    } else {
        denoised = applyBilateralFilter(image);
    }

    return denoised;
}

cv::Mat ImageProcessor::sharpenImage(const cv::Mat& image) {
    cv::Mat sharpened;
    cv::GaussianBlur(image, sharpened, cv::Size(0, 0), 3);
    cv::addWeighted(image, 1.5, sharpened, -0.5, 0, sharpened);
    return sharpened;
}

cv::Mat ImageProcessor::resizeImage(const cv::Mat& image, int min_size) {
    double scale = 1.0;
    int new_width = image.cols;
    int new_height = image.rows;

    if (image.cols < min_size || image.rows < min_size) {
        if (image.cols < image.rows) {
            scale = static_cast<double>(min_size) / image.cols;
        } else {
            scale = static_cast<double>(min_size) / image.rows;
        }

        new_width = static_cast<int>(image.cols * scale);
        new_height = static_cast<int>(image.rows * scale);
    }

    cv::Mat resized;
    cv::resize(image, resized, cv::Size(new_width, new_height), 0, 0, cv::INTER_CUBIC);

    Logger::debug("Image resized from " + std::to_string(image.cols) + "x" +
                  std::to_string(image.rows) + " to " + std::to_string(new_width) +
                  "x" + std::to_string(new_height));

    return resized;
}

cv::Mat ImageProcessor::adjustBrightness(const cv::Mat& image, double alpha, int beta) {
    cv::Mat adjusted;
    image.convertTo(adjusted, -1, alpha, beta);
    return adjusted;
}

bool ImageProcessor::needsEnhancement(const cv::Mat& image) {
    if (image.empty()) return false;

    cv::Scalar mean = cv::mean(image);
    double avg_brightness = mean[0];

    return avg_brightness < 50 || avg_brightness > 200;
}

double ImageProcessor::calculateQualityScore(const cv::Mat& image) {
    if (image.empty()) return 0.0;

    cv::Mat gray, laplacian;
    if (image.channels() > 1) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image;
    }

    cv::Laplacian(gray, laplacian, CV_64F);
    cv::Scalar mean, stddev;
    cv::meanStdDev(laplacian, mean, stddev);

    double variance = stddev.val[0] * stddev.val[0];
    return variance;
}

cv::Mat ImageProcessor::applyCLAHE(const cv::Mat& image) {
    cv::Mat enhanced;
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(2.0);
    clahe->apply(image, enhanced);
    return enhanced;
}

cv::Mat ImageProcessor::applyBilateralFilter(const cv::Mat& image) {
    cv::Mat filtered;
    cv::bilateralFilter(image, filtered, 9, 75, 75);
    return filtered;
}
