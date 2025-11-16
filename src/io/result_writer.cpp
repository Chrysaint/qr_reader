#include "result_writer.h"
#include "../utils/logger.h"
#include <fstream>
#include <iomanip>

bool ResultWriter::saveToTextFile(const QRDetector::DetectionResult& result,
                                 const std::string& filename) {
    Logger::startOperation("Saving results to text file: " + filename);

    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::error("Failed to open file for writing: " + filename);
        return false;
    }

    file << formatResult(result);
    file.close();

    Logger::info("Results saved to: " + filename);
    Logger::endOperation("Saving results to text file");

    return true;
}

bool ResultWriter::saveVisualization(const QRDetector::DetectionResult& result,
                                   const std::string& filename) {
    if (!result.success || result.processed_image.empty()) {
        Logger::warning("Cannot save visualization - no successful result or empty image");
        return false;
    }

    Logger::startOperation("Saving visualization: " + filename);

    cv::Mat visualization = result.processed_image.clone();

    if (result.bounding_box.size() == 4) {
        drawBoundingBox(visualization, result.bounding_box);
    }

    drawInfoText(visualization, result);

    bool success = cv::imwrite(filename, visualization);

    if (success) {
        Logger::info("Visualization saved to: " + filename);
    } else {
        Logger::error("Failed to save visualization: " + filename);
    }

    Logger::endOperation("Saving visualization");

    return success;
}

void ResultWriter::printToConsole(const QRDetector::DetectionResult& result) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "QR CODE DETECTION RESULT" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    if (result.success) {
        std::cout << "Status: SUCCESS" << std::endl;
        std::cout << "Data: " << result.data << std::endl;
        std::cout << "Confidence: " << std::fixed << std::setprecision(2)
                  << (result.confidence * 100) << "%" << std::endl;

        if (!result.bounding_box.empty()) {
            std::cout << "Bounding Box: ";
            for (const auto& point : result.bounding_box) {
                std::cout << "(" << point.x << "," << point.y << ") ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "Status: FAILED" << std::endl;
        std::cout << "Error: " << result.error_message << std::endl;
    }

    std::cout << std::string(50, '=') << std::endl;
}

bool ResultWriter::saveBatchResults(const std::vector<QRDetector::DetectionResult>& results,
                                  const std::string& base_filename) {
    Logger::startOperation("Saving batch results");

    std::ofstream file(base_filename + "_batch.txt");
    if (!file.is_open()) {
        Logger::error("Failed to create batch results file");
        return false;
    }

    file << "BATCH QR CODE DETECTION RESULTS" << std::endl;
    file << "Generated: " << __DATE__ << " " << __TIME__ << std::endl;
    file << "Total files processed: " << results.size() << std::endl;
    file << std::string(40, '-') << std::endl;

    int success_count = 0;
    for (size_t i = 0; i < results.size(); ++i) {
        file << "Result " << (i + 1) << ":" << std::endl;
        file << formatResult(results[i]) << std::endl;

        if (results[i].success) {
            success_count++;
        }
    }

    file << std::string(40, '-') << std::endl;
    file << "Successful detections: " << success_count << "/" << results.size()
         << " (" << std::fixed << std::setprecision(1)
         << (static_cast<double>(success_count) / results.size() * 100) << "%)" << std::endl;

    file.close();

    Logger::info("Batch results saved: " + base_filename + "_batch.txt");
    Logger::endOperation("Saving batch results");

    return true;
}

void ResultWriter::generateReport(const std::vector<QRDetector::DetectionResult>& results,
                                const std::string& filename) {
    saveBatchResults(results, filename);
}

void ResultWriter::drawBoundingBox(cv::Mat& image, const std::vector<cv::Point>& bbox) {
    const cv::Scalar COLOR_GREEN(0, 255, 0);
    const cv::Scalar COLOR_RED(0, 0, 255);
    const int THICKNESS = 3;

    for (int i = 0; i < 4; i++) {
        cv::line(image, bbox[i], bbox[(i + 1) % 4], COLOR_GREEN, THICKNESS);
    }

    for (const auto& point : bbox) {
        cv::circle(image, point, 8, COLOR_RED, -1);
        cv::circle(image, point, 4, COLOR_GREEN, -1);
    }
}

void ResultWriter::drawInfoText(cv::Mat& image, const QRDetector::DetectionResult& result) {
    const cv::Scalar COLOR_WHITE(255, 255, 255);
    const cv::Scalar COLOR_BLACK(0, 0, 0);
    const cv::Scalar COLOR_BG(0, 0, 0);

    int baseline = 0;
    double font_scale = 0.6;
    int thickness = 2;
    cv::Size text_size;

    std::string status_text = result.success ? "QR DETECTED" : "NOT DETECTED";
    std::string confidence_text = result.success ?
        "Confidence: " + std::to_string(static_cast<int>(result.confidence * 100)) + "%" : "";

    text_size = cv::getTextSize(status_text, cv::FONT_HERSHEY_SIMPLEX, font_scale, thickness, &baseline);

    cv::Point text_org(10, 30);
    cv::rectangle(image,
                 cv::Point(text_org.x - 5, text_org.y - text_size.height - 5),
                 cv::Point(text_org.x + text_size.width + 5, text_org.y + baseline + 5),
                 COLOR_BG, -1);

    cv::putText(image, status_text, text_org,
               cv::FONT_HERSHEY_SIMPLEX, font_scale, COLOR_WHITE, thickness);

    if (result.success) {
        text_org.y += text_size.height + 15;
        text_size = cv::getTextSize(confidence_text, cv::FONT_HERSHEY_SIMPLEX, font_scale, thickness, &baseline);

        cv::rectangle(image,
                     cv::Point(text_org.x - 5, text_org.y - text_size.height - 5),
                     cv::Point(text_org.x + text_size.width + 5, text_org.y + baseline + 5),
                     COLOR_BG, -1);

        cv::putText(image, confidence_text, text_org,
                   cv::FONT_HERSHEY_SIMPLEX, font_scale, COLOR_WHITE, thickness);

        std::string display_data = result.data;
        if (display_data.length() > 50) {
            display_data = display_data.substr(0, 47) + "...";
        }

        text_org.y += text_size.height + 15;
        std::string data_text = "Data: " + display_data;
        text_size = cv::getTextSize(data_text, cv::FONT_HERSHEY_SIMPLEX, font_scale, thickness, &baseline);

        cv::rectangle(image,
                     cv::Point(text_org.x - 5, text_org.y - text_size.height - 5),
                     cv::Point(text_org.x + text_size.width + 5, text_org.y + baseline + 5),
                     COLOR_BG, -1);

        cv::putText(image, data_text, text_org,
                   cv::FONT_HERSHEY_SIMPLEX, font_scale, COLOR_WHITE, thickness);
    }
}

std::string ResultWriter::formatResult(const QRDetector::DetectionResult& result) {
    std::stringstream ss;

    ss << "Detection Result:" << std::endl;
    ss << "  Success: " << (result.success ? "YES" : "NO") << std::endl;

    if (result.success) {
        ss << "  Data: " << result.data << std::endl;
        ss << "  Confidence: " << std::fixed << std::setprecision(1)
           << (result.confidence * 100) << "%" << std::endl;

        if (!result.bounding_box.empty()) {
            ss << "  Bounding Box: ";
            for (const auto& point : result.bounding_box) {
                ss << "(" << point.x << "," << point.y << ") ";
            }
            ss << std::endl;
        }
    } else {
        ss << "  Error: " << result.error_message << std::endl;
    }

    ss << "  Timestamp: " << __DATE__ << " " << __TIME__ << std::endl;

    return ss.str();
}
