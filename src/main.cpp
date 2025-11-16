#include <iostream>
#include <string>
#include "utils/logger.h"
#include "io/image_loader.h"
#include "core/qr_detector.h"
#include "io/result_writer.h"

int main() {
    std::cout << "=== QR Reader Complete System Test ===" << std::endl;

    Logger::setLogLevel(Logger::INFO);

    Logger::info("Step 1: Loading image...");

    std::vector<ImageLoader::LoadResult> load_results;

    std::vector<std::string> paths = {
        "../test_images/qr1.png",
        "../test_images/qr2.jpg",
        "../test_images/qr3.jpg",
        "../test_images/qr4.jpg",
        "qr_code.png"
    };

    for (const auto& path : paths) {
        auto load_result = ImageLoader::loadFromFile(path);
        if (load_result.success) {
            Logger::info("Successfully loaded from: " + path);
            load_results.push_back(load_result);
        } else {
            Logger::error("No test image found by path: " + path + " Please add a QR code image to the project.");
        }
    }

    if (load_results.size() == 0) {
        Logger::error("No test image found. Please add a QR code image to the project.");
        return 1;
    }

    Logger::info("Step 2: Initializing QR detector...");
    QRDetector detector;
    detector.setPreprocessingEnabled(true);

    Logger::info("Step 3: Detecting QR code...");
    std::vector<QRDetector::DetectionResult> detection_results;
    for (const auto& load_result : load_results) {
       auto detection_result = detector.detectFromImage(load_result.image);
       detection_results.push_back(detection_result);
    }

    int writer_counter = 1;
    Logger::info("Step 4: Outputting results...");
    for (const auto& detection : detection_results) {
        ResultWriter::printToConsole(detection);
        if (detection.success) {
            ResultWriter::saveToTextFile(detection, "qr_result_" + std::to_string(writer_counter) + ".txt");
            ResultWriter::saveVisualization(detection, "qr_visualization_" + std::to_string(writer_counter) + ".png");
            Logger::info("Results saved to qr_result.txt and qr_visualization_" + std::to_string(writer_counter) + ".png");
            writer_counter++;
        }
    }

    Logger::info("Detection statistics:");
    Logger::info("  Total detections: " + std::to_string(detector.getTotalDetections()));
    Logger::info("  Successful: " + std::to_string(detector.getSuccessfulDetections()));
    Logger::info("  Success rate: " + std::to_string(static_cast<int>(detector.getSuccessRate() * 100)) + "%");

    std::cout << "\n=== Test Completed ===" << std::endl;
    return 0;
}
