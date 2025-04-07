# Image and Signal Processing Program

A high-performance C++ application for batch processing of images and signal data files. This program can efficiently handle hundreds of small images or dozens of large images, as well as CSV-based signal data.

## Features

### Image Processing
- **Gaussian Blur**: Apply configurable blur effect
- **Contrast Enhancement**: Adjust image contrast and brightness
- **Edge Detection**: Detect and blend edges with original images
- **Batch Processing**: Process multiple images with different parameter combinations
- **Statistics**: Analyze image dimensions and file sizes

### Signal Processing
- **Moving Average Filter**: Smooth signal data with adjustable window size
- **Peak Detection**: Find peaks in signal data based on configurable threshold
- **CSV Processing**: Handle and transform CSV-based signal data

### General Features
- **Command Line Interface**: Flexible parameter configuration
- **Performance Metrics**: Track processing time and efficiency
- **Verbose Output Option**: Detailed progress reporting
- **Multiple Output Formats**: Organized results storage

## Requirements

- C++17 compatible compiler
- OpenCV library (4.0 or higher recommended)
- File system support (std::filesystem)

## Installation

1. Clone this repository:
   ```
   git clone https://github.com/yourusername/image-signal-processor.git
   cd image-signal-processor
   ```

2. Install dependencies:
   ```
   # Ubuntu/Debian
   sudo apt-get install libopencv-dev

   # macOS
   brew install opencv

   # Windows (using vcpkg)
   vcpkg install opencv
   ```

3. Build the application:
   ```
   g++ -std=c++17 main.cpp -o image_signal_processor -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
   ```

## Usage

### Basic Usage

Process images in a directory:
```
./image_signal_processor --mode image --input ./input_images --output ./processed_images
```

Process signal data:
```
./image_signal_processor --mode signal --input ./input_signals --output ./processed_signals
```

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--mode <image\|signal>` | Processing mode | `image` |
| `--input <directory>` | Input directory containing files to process | `./input` |
| `--output <directory>` | Output directory for processed files | `./output` |
| `--blur <size>` | Blur kernel size for image processing | `5` |
| `--contrast <value>` | Contrast multiplier for image processing | `1.5` |
| `--window <size>` | Window size for signal processing | `10` |
| `--threshold <value>` | Threshold for peak detection | `0.5` |
| `--analysis` | Run analysis on input files without processing | |
| `--verbose` | Enable verbose output | |
| `--help` | Display help message | |

### Examples

Process images with custom parameters:
```
./image_signal_processor --mode image --input ./photos --output ./enhanced --blur 7 --contrast 1.8 --verbose
```

Analyze a collection of images:
```
./image_signal_processor --mode image --input ./dataset --analysis
```

Process signal data with custom window size:
```
./image_signal_processor --mode signal --input ./sensor_data --output ./filtered_data --window 15 --threshold 0.65
```

## Input Data Format

### Images
The program supports the following image formats:
- JPEG (.jpg, .jpeg)
- PNG (.png)
- BMP (.bmp)
- TIFF (.tif, .tiff)

### Signal Data
Signal data should be provided as CSV files with one value per line. If a header is present, the first line will be skipped.

## Output Format

### Images
Processed images are saved in the same format as the input with adjustments applied.

### Signal Data
Processed signal data is saved as CSV files with the following columns:
- `original`: Original signal value
- `filtered`: Filtered signal value
- `is_peak`: Binary indicator for detected peaks (1 = peak, 0 = not peak)

## Performance

The program is optimized for:
- Processing hundreds of small images (< 1MB each)
- Processing dozens of large images (> 10MB each)
- Handling large CSV signal data files

Performance metrics are displayed after processing, including:
- Total processing time
- Average time per file
- File size statistics (for analysis mode)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
