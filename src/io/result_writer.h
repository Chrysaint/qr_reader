#ifndef QR_READER_RESULT_WRITER_H
#define QR_READER_RESULT_WRITER_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "../core/qr_detector.h"

class ResultWriter {
public:
    static bool saveToTextFile(const QRDetector::DetectionResult& result,
                              const std::string& filename);

    static bool saveVisualization(const QRDetector::DetectionResult& result,
                                 const std::string& filename);

    static void printToConsole(const QRDetector::DetectionResult& result);

    static bool saveBatchResults(const std::vector<QRDetector::DetectionResult>& results,
                                const std::string& base_filename);

    static void generateReport(const std::vector<QRDetector::DetectionResult>& results,
                              const std::string& filename);

private:
    static void drawBoundingBox(cv::Mat& image, const std::vector<cv::Point>& bbox);
    static void drawInfoText(cv::Mat& image, const QRDetector::DetectionResult& result);
    static std::string formatResult(const QRDetector::DetectionResult& result);
};

#endif // QR_READER_RESULT_WRITER_H
