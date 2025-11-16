#ifndef QR_READER_QR_DETECTOR_H
#define QR_READER_QR_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class QRDetector {
public:
    struct DetectionResult {
        bool success = false;
        std::string data;
        std::vector<cv::Point> bounding_box;
        double confidence = 0.0;
        cv::Mat processed_image;
        std::string error_message;
    };

    QRDetector();

    DetectionResult detectFromImage(const cv::Mat& image);
    DetectionResult detectFromWebcam();

    void setPreprocessingEnabled(bool enabled);
    void setMultipleQRDetection(bool enabled);

    int getTotalDetections() const;
    int getSuccessfulDetections() const;
    double getSuccessRate() const;

private:
    cv::QRCodeDetector qr_detector_;
    bool preprocessing_enabled_ = true;
    bool multiple_qr_enabled_ = false;

    int total_detections_ = 0;
    int successful_detections_ = 0;

    DetectionResult processDetection(const cv::Mat& image);
    bool validateQRData(const std::string& data);
    double calculateConfidence(const std::vector<cv::Point>& bbox, const cv::Mat& image);
};

#endif // QR_READER_QR_DETECTOR_H
