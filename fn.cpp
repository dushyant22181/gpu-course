#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace fs = std::filesystem;

class ImageProcessor {
private:
    std::vector<std::string> image_paths;
    std::string output_dir;
    bool verbose;
    
    // Image processing parameters
    int blur_size;
    double contrast_alpha;
    int contrast_beta;
    bool apply_edge_detection;

public:
    ImageProcessor(const std::string& input_dir, const std::string& output_dir, 
                  bool verbose = false) 
        : output_dir(output_dir), verbose(verbose) {
        
        // Default processing parameters
        blur_size = 5;
        contrast_alpha = 1.5;
        contrast_beta = 10;
        apply_edge_detection = true;
        
        // Create output directory if it doesn't exist
        if (!fs::exists(output_dir)) {
            fs::create_directories(output_dir);
        }
        
        // Load all image paths from the input directory
        loadImagePaths(input_dir);
        
        if (verbose) {
            std::cout << "Found " << image_paths.size() << " images for processing" << std::endl;
        }
    }
    
    void loadImagePaths(const std::string& input_dir) {
        // Clear existing paths
        image_paths.clear();
        
        // Supported image extensions
        std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff"};
        
        // Collect all image files in the directory
        for (const auto& entry : fs::recursive_directory_iterator(input_dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                
                // Convert extension to lowercase for comparison
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                if (std::find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
                    image_paths.push_back(entry.path().string());
                }
            }
        }
    }
    
    void setProcessingParameters(int blur_size, double contrast_alpha, 
                                int contrast_beta, bool apply_edge_detection) {
        this->blur_size = blur_size;
        this->contrast_alpha = contrast_alpha;
        this->contrast_beta = contrast_beta;
        this->apply_edge_detection = apply_edge_detection;
    }
    
    void processImage(const std::string& image_path) {
        // Load image
        cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
        
        if (image.empty()) {
            std::cerr << "Error: Could not read image " << image_path << std::endl;
            return;
        }
        
        // Apply Gaussian blur
        cv::Mat blurred;
        cv::GaussianBlur(image, blurred, cv::Size(blur_size, blur_size), 0);
        
        // Adjust contrast and brightness
        cv::Mat contrast_adjusted;
        blurred.convertTo(contrast_adjusted, -1, contrast_alpha, contrast_beta);
        
        // Apply edge detection if enabled
        cv::Mat processed;
        if (apply_edge_detection) {
            cv::Mat edges;
            cv::Canny(contrast_adjusted, edges, 50, 150);
            
            // Combine original with edges
            cv::cvtColor(edges, edges, cv::COLOR_GRAY2BGR);
            cv::addWeighted(contrast_adjusted, 0.7, edges, 0.3, 0, processed);
        } else {
            processed = contrast_adjusted;
        }
        
        // Get output filename
        fs::path path(image_path);
        std::string filename = path.filename().string();
        std::string output_path = output_dir + "/" + filename;
        
        // Save processed image
        cv::imwrite(output_path, processed);
        
        if (verbose) {
            std::cout << "Processed: " << filename << std::endl;
        }
    }
    
    void processAllImages() {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Process each image
        for (size_t i = 0; i < image_paths.size(); ++i) {
            processImage(image_paths[i]);
            
            // Print progress
            if (verbose && (i + 1) % 10 == 0) {
                std::cout << "Progress: " << (i + 1) << "/" << image_paths.size() << std::endl;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        
        std::cout << "Processed " << image_paths.size() << " images in " 
                  << duration.count() << " seconds" << std::endl;
        std::cout << "Average time per image: " 
                  << (duration.count() / image_paths.size()) << " seconds" << std::endl;
    }
    
    // Function to handle batch processing with different parameters
    void batchProcess(const std::vector<int>& blur_sizes, 
                     const std::vector<double>& contrast_values) {
        for (int blur : blur_sizes) {
            for (double contrast : contrast_values) {
                // Create subfolder for this parameter combination
                std::string param_dir = output_dir + "/blur" + std::to_string(blur) + 
                                      "_contrast" + std::to_string(static_cast<int>(contrast * 10));
                
                if (!fs::exists(param_dir)) {
                    fs::create_directories(param_dir);
                }
                
                // Update parameters
                setProcessingParameters(blur, contrast, contrast_beta, apply_edge_detection);
                
                // Store original output directory
                std::string original_output_dir = output_dir;
                output_dir = param_dir;
                
                // Process images
                if (verbose) {
                    std::cout << "Processing with blur=" << blur << ", contrast=" << contrast << std::endl;
                }
                
                processAllImages();
                
                // Restore original output directory
                output_dir = original_output_dir;
            }
        }
    }
    
    // Statistics on images
    void analyzeImages() {
        if (image_paths.empty()) {
            std::cout << "No images to analyze" << std::endl;
            return;
        }
        
        size_t total_size = 0;
        int min_width = INT_MAX, max_width = 0;
        int min_height = INT_MAX, max_height = 0;
        
        for (const auto& path : image_paths) {
            // Get file size
            total_size += fs::file_size(path);
            
            // Get image dimensions
            cv::Mat img = cv::imread(path);
            if (!img.empty()) {
                min_width = std::min(min_width, img.cols);
                max_width = std::max(max_width, img.cols);
                min_height = std::min(min_height, img.rows);
                max_height = std::max(max_height, img.rows);
            }
        }
        
        std::cout << "Image Analysis:" << std::endl;
        std::cout << "Number of images: " << image_paths.size() << std::endl;
        std::cout << "Total size: " << (total_size / (1024.0 * 1024.0)) << " MB" << std::endl;
        std::cout << "Average size: " << (total_size / (1024.0 * image_paths.size())) << " KB/image" << std::endl;
        std::cout << "Width range: " << min_width << " - " << max_width << " pixels" << std::endl;
        std::cout << "Height range: " << min_height << " - " << max_height << " pixels" << std::endl;
    }
};

class SignalProcessor {
private:
    std::vector<std::string> signal_files;
    std::string output_dir;
    bool verbose;
    
    // Signal processing parameters
    int window_size;
    double threshold;

public:
    SignalProcessor(const std::string& input_dir, const std::string& output_dir, 
                   bool verbose = false) 
        : output_dir(output_dir), verbose(verbose) {
        
        // Default parameters
        window_size = 10;
        threshold = 0.5;
        
        // Create output directory if it doesn't exist
        if (!fs::exists(output_dir)) {
            fs::create_directories(output_dir);
        }
        
        // Load all CSV files from the input directory
        loadSignalFiles(input_dir);
        
        if (verbose) {
            std::cout << "Found " << signal_files.size() << " signal files for processing" << std::endl;
        }
    }
    
    void loadSignalFiles(const std::string& input_dir) {
        // Clear existing paths
        signal_files.clear();
        
        // Find all CSV files
        for (const auto& entry : fs::recursive_directory_iterator(input_dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".csv") {
                signal_files.push_back(entry.path().string());
            }
        }
    }
    
    void setProcessingParameters(int window_size, double threshold) {
        this->window_size = window_size;
        this->threshold = threshold;
    }
    
    // Apply moving average filter to signal data
    std::vector<double> movingAverage(const std::vector<double>& signal) {
        std::vector<double> result(signal.size());
        
        for (size_t i = 0; i < signal.size(); ++i) {
            double sum = 0.0;
            int count = 0;
            
            // Calculate window boundaries
            int start = std::max(0, static_cast<int>(i) - window_size / 2);
            int end = std::min(static_cast<int>(signal.size() - 1), 
                             static_cast<int>(i) + window_size / 2);
            
            // Calculate average
            for (int j = start; j <= end; ++j) {
                sum += signal[j];
                count++;
            }
            
            result[i] = sum / count;
        }
        
        return result;
    }
    
    // Find peaks in signal data
    std::vector<size_t> findPeaks(const std::vector<double>& signal) {
        std::vector<size_t> peaks;
        
        // Skip first and last points
        for (size_t i = 1; i < signal.size() - 1; ++i) {
            if (signal[i] > signal[i - 1] && signal[i] > signal[i + 1] && 
                signal[i] > threshold) {
                peaks.push_back(i);
            }
        }
        
        return peaks;
    }
    
    void processSignal(const std::string& file_path) {
        // Read CSV file
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << file_path << std::endl;
            return;
        }
        
        std::vector<double> signal;
        std::string line;
        
        // Skip header if exists
        std::getline(file, line);
        
        // Read signal data
        while (std::getline(file, line)) {
            try {
                double value = std::stod(line);
                signal.push_back(value);
            } catch (...) {
                // Skip lines that can't be converted
                continue;
            }
        }
        
        // Process signal
        std::vector<double> filtered_signal = movingAverage(signal);
        std::vector<size_t> peaks = findPeaks(filtered_signal);
        
        // Create output file
        fs::path path(file_path);
        std::string filename = path.filename().string();
        std::string output_path = output_dir + "/processed_" + filename;
        
        std::ofstream outfile(output_path);
        if (!outfile.is_open()) {
            std::cerr << "Error: Could not create output file " << output_path << std::endl;
            return;
        }
        
        // Write header and processed data
        outfile << "original,filtered,is_peak\n";
        
        for (size_t i = 0; i < signal.size(); ++i) {
            bool is_peak = std::find(peaks.begin(), peaks.end(), i) != peaks.end();
            outfile << signal[i] << "," << filtered_signal[i] << "," << (is_peak ? "1" : "0") << "\n";
        }
        
        outfile.close();
        
        if (verbose) {
            std::cout << "Processed: " << filename << " (Found " << peaks.size() << " peaks)" << std::endl;
        }
    }
    
    void processAllSignals() {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Process each signal file
        for (size_t i = 0; i < signal_files.size(); ++i) {
            processSignal(signal_files[i]);
            
            // Print progress
            if (verbose && (i + 1) % 5 == 0) {
                std::cout << "Progress: " << (i + 1) << "/" << signal_files.size() << std::endl;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        
        std::cout << "Processed " << signal_files.size() << " signal files in " 
                  << duration.count() << " seconds" << std::endl;
    }
};

void print_usage() {
    std::cout << "Usage: image_signal_processor [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --mode <image|signal>      Processing mode (default: image)" << std::endl;
    std::cout << "  --input <directory>        Input directory containing files to process" << std::endl;
    std::cout << "  --output <directory>       Output directory for processed files" << std::endl;
    std::cout << "  --blur <size>              Blur kernel size for image processing (default: 5)" << std::endl;
    std::cout << "  --contrast <value>         Contrast multiplier for image processing (default: 1.5)" << std::endl;
    std::cout << "  --window <size>            Window size for signal processing (default: 10)" << std::endl;
    std::cout << "  --threshold <value>        Threshold for peak detection (default: 0.5)" << std::endl;
    std::cout << "  --analysis                 Run analysis on input files" << std::endl;
    std::cout << "  --verbose                  Enable verbose output" << std::endl;
    std::cout << "  --help                     Display this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    // Default parameters
    std::string mode = "image";
    std::string input_dir = "./input";
    std::string output_dir = "./output";
    int blur_size = 5;
    double contrast = 1.5;
    int window_size = 10;
    double threshold = 0.5;
    bool verbose = false;
    bool run_analysis = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "--input" && i + 1 < argc) {
            input_dir = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            output_dir = argv[++i];
        } else if (arg == "--blur" && i + 1 < argc) {
            blur_size = std::stoi(argv[++i]);
        } else if (arg == "--contrast" && i + 1 < argc) {
            contrast = std::stod(argv[++i]);
        } else if (arg == "--window" && i + 1 < argc) {
            window_size = std::stoi(argv[++i]);
        } else if (arg == "--threshold" && i + 1 < argc) {
            threshold = std::stod(argv[++i]);
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--analysis") {
            run_analysis = true;
        } else if (arg == "--help") {
            print_usage();
            return 0;
        }
    }
    
    try {
        std::cout << "Starting processing in " << mode << " mode" << std::endl;
        
        if (mode == "image") {
            ImageProcessor processor(input_dir, output_dir, verbose);
            
            if (run_analysis) {
                processor.analyzeImages();
            }
            
            processor.setProcessingParameters(blur_size, contrast, 10, true);
            processor.processAllImages();
            
        } else if (mode == "signal") {
            SignalProcessor processor(input_dir, output_dir, verbose);
            processor.setProcessingParameters(window_size, threshold);
            processor.processAllSignals();
            
        } else {
            std::cerr << "Error: Invalid mode. Use 'image' or 'signal'." << std::endl;
            print_usage();
            return 1;
        }
        
        std::cout << "Processing completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
