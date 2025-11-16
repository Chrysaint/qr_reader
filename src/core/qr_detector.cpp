#include "qr_detector.h"
#include "../utils/logger.h"
#include "../processors/image_processor.h"

QRDetector::QRDetector() {
    Logger::info("QRDetector initialized");
}

QRDetector::DetectionResult QRDetector::detectFromImage(const cv::Mat& image) {
    Logger::startOperation("QR detection from image");
    total_detections_++;

    if (image.empty()) {
        Logger::error("Cannot detect QR codes in empty image");
        return {false, "", {}, 0.0, cv::Mat(), "Empty input image"};
    }

    cv::Mat processed_image = image.clone();

    DetectionResult original_result = processDetection(processed_image);

    if (!original_result.success && preprocessing_enabled_) {
        Logger::debug("Trying with image enhancement...");
        cv::Mat enhanced_image = ImageProcessor::enhanceForQRDetection(image);
        DetectionResult enhanced_result = processDetection(enhanced_image);

        if (enhanced_result.success) {
            enhanced_result.processed_image = enhanced_image;
            Logger::info("QR found after enhancement!");
            successful_detections_++;
            Logger::endOperation("QR detection from image");
            return enhanced_result;
        }

        cv::imwrite("debug_enhanced.png", enhanced_image);
    }

    if (original_result.success) {
        successful_detections_++;
        original_result.processed_image = processed_image;
        Logger::info("QR detection successful: " + original_result.data);
    } else {
        Logger::warning("QR detection failed");
        // Сохраняем оригинал для отладки
        cv::imwrite("debug_original.png", processed_image);
    }

    Logger::endOperation("QR detection from image");
    return original_result;
}

QRDetector::DetectionResult QRDetector::detectFromWebcam() {
    Logger::info("Attempting QR detection from webcam");
    return DetectionResult{false, "", {}, 0.0, cv::Mat(), "Webcam detection not implemented yet"};
}

void QRDetector::setPreprocessingEnabled(bool enabled) {
    preprocessing_enabled_ = enabled;
    Logger::debug("Preprocessing " + std::string(enabled ? "enabled" : "disabled"));
}

void QRDetector::setMultipleQRDetection(bool enabled) {
    multiple_qr_enabled_ = enabled;
    Logger::debug("Multiple QR detection " + std::string(enabled ? "enabled" : "disabled"));
}

int QRDetector::getTotalDetections() const {
    return total_detections_;
}

int QRDetector::getSuccessfulDetections() const {
    return successful_detections_;
}

double QRDetector::getSuccessRate() const {
    if (total_detections_ == 0) return 0.0;
    return static_cast<double>(successful_detections_) / total_detections_;
}

QRDetector::DetectionResult QRDetector::processDetection(const cv::Mat& image) {
    DetectionResult result;

    try {
        std::vector<cv::Point> points;
        std::string data = qr_detector_.detectAndDecode(image, points);

        Logger::debug("QR detection attempted, data length: " + std::to_string(data.length()));
        Logger::debug("Found points: " + std::to_string(points.size()));

        if (!data.empty()) {
            Logger::debug("Raw QR data: " + data);
        }

        if (!data.empty() && validateQRData(data)) {
            result.success = true;
            result.data = data;
            result.bounding_box = points;
            result.confidence = calculateConfidence(points, image);
            Logger::debug("QR validation passed");
        } else {
            result.success = false;
            if (data.empty()) {
                result.error_message = "No QR code detected in image";
            } else {
                result.error_message = "QR code found but data validation failed";
            }
        }
    }
    catch (const cv::Exception& e) {
        result.success = false;
        result.error_message = "OpenCV error: " + std::string(e.what());
        Logger::error("OpenCV exception: " + std::string(e.what()));
    }

    return result;
}

bool QRDetector::validateQRData(const std::string& data) {
    if (data.empty()) return false;

    if (data.length() < 1) return false;

    for (char c : data) {
        if (c < 32 && c != 10 && c != 13) {
            return false;
        }
    }

    return true;
}

double QRDetector::calculateConfidence(const std::vector<cv::Point>& bbox, const cv::Mat& image) {
    if (bbox.size() != 4) return 0.0;

    double confidence = 0.0;

    double qr_area = cv::contourArea(bbox);
    double image_area = image.cols * image.rows;
    double size_ratio = qr_area / image_area;

    if (size_ratio > 0.1 && size_ratio < 0.8) {
        confidence += 0.4;
    } else if (size_ratio > 0.05 && size_ratio < 0.9) {
        confidence += 0.2;
    }

    double quality_score = ImageProcessor::calculateQualityScore(image);
    if (quality_score > 100) {
        confidence += 0.3;
    } else if (quality_score > 50) {
        confidence += 0.15;
    }

    std::vector<double> sides;
    for (int i = 0; i < 4; i++) {
        cv::Point p1 = bbox[i];
        cv::Point p2 = bbox[(i + 1) % 4];
        double side_length = cv::norm(p1 - p2);
        sides.push_back(side_length);
    }

    double avg_side = (sides[0] + sides[1] + sides[2] + sides[3]) / 4.0;
    double variance = 0.0;
    for (double side : sides) {
        variance += std::abs(side - avg_side);
    }
    variance /= avg_side;

    if (variance < 0.1) {
        confidence += 0.3;
    } else if (variance < 0.2) {
        confidence += 0.15;
    }

    return std::min(confidence, 1.0);
}
