# gpu-course

Below is a README file tailored for the image processing program I provided. It includes setup instructions, usage details, customization options, and additional notes, formatted in Markdown for clarity and compatibility with platforms like GitHub.

---

# Batch Image Edge Detection

This Python program processes a large number of images (hundreds of small or tens of large) by applying edge detection using the Canny algorithm. It leverages multiprocessing for efficiency and includes a progress bar to track the operation.

## Features
- Processes hundreds of small images (e.g., 512x512) or tens of large images.
- Applies Canny edge detection to each image.
- Uses multiprocessing to utilize multiple CPU cores.
- Displays a progress bar during processing.
- Saves processed images with `_edges` appended to the filename.

## Requirements
- Python 3.6 or higher
- Required libraries:
  - `opencv-python` (for image processing)
  - `numpy` (for array operations)
  - `tqdm` (for progress bar)

## Setup
1. **Install Dependencies**:
   Run the following command to install the required libraries:
   ```bash
   pip install opencv-python numpy tqdm
   ```

2. **Prepare Input Images**:
   - Create a folder (e.g., `input_images`).
   - Place your images (e.g., JPEG, PNG) in this folder. The program supports `.jpg`, `.jpeg`, `.png`, and `.bmp` formats.
   - Example: 100s of 512x512 JPEGs or 10s of larger images.

3. **Output Directory**:
   - The processed images will be saved in a folder named `output_edges` (created automatically if it doesn’t exist).

## Usage
1. **Edit the Script** (optional):
   - Open the script (e.g., `batch_edge_detection.py`).
   - Modify `INPUT_DIR` and `OUTPUT_DIR` to match your folder paths:
     ```python
     INPUT_DIR = "your_input_folder"  # e.g., "input_images"
     OUTPUT_DIR = "your_output_folder"  # e.g., "output_edges"
     ```
   - Adjust `NUM_WORKERS` based on your CPU (default is one less than total cores):
     ```python
     NUM_WORKERS = 4  # Example for a quad-core CPU
     ```

2. **Run the Program**:
   Execute the script from the command line:
   ```bash
   python batch_edge_detection.py
   ```
   - The program will process all images in `INPUT_DIR` and save results to `OUTPUT_DIR`.
   - A progress bar will show the status.

## What It Does
- Loads each image in grayscale.
- Applies Canny edge detection with tunable thresholds (default: 100, 200).
- Saves the edge-detected images with `_edges` appended (e.g., `image1.jpg` → `image1_edges.jpg`).
- Uses multiprocessing for efficient handling of large datasets.
- Displays progress with `tqdm`.

## Customization
- **Input/Output Paths**:
  Change `INPUT_DIR` and `OUTPUT_DIR` in the script to your desired folders.
- **Processing Function**:
  Replace `cv2.Canny` with another OpenCV function for different processing:
  - Example: Gaussian blur with `cv2.blur(img, (5, 5))`.
  - Modify in the `process_image` function.
- **Image Size**:
  - For tens of large images (e.g., 4K), add resizing to avoid memory issues:
    ```python
    img = cv2.resize(img, (512, 512))  # Add this line in process_image
    ```
  - Works out-of-the-box for hundreds of small images (e.g., 512x512).
- **Thresholds**:
  Adjust Canny thresholds in `cv2.Canny(img, 100, 200)` for different edge sensitivity.
- **Signal Processing**:
  For audio or other signals, contact the developer for a modified version using SciPy.

## Notes
- **Scalability**: Multiprocessing makes it suitable for 100s of small images. For very large images (e.g., 10s of 4K), resize or batch them to manage memory.
- **Error Handling**: Skips corrupt files and logs errors to the console.
- **Performance**: Set `NUM_WORKERS` to match your CPU cores (e.g., 4 for quad-core). Too many workers may slow down due to overhead.
- **Supported Formats**: `.jpg`, `.jpeg`, `.png`, `.bmp`. Add more extensions in `image_extensions` if needed.

## Example
- Input: `input_images/image1.jpg`, `input_images/image2.png`
- Output: `output_edges/image1_edges.jpg`, `output_edges/image2_edges.png`

## Troubleshooting
- **No images found**: Ensure `INPUT_DIR` contains valid image files.
- **Memory errors**: Reduce `NUM_WORKERS` or resize large images.
- **Missing libraries**: Re-run `pip install` command.

## License
This project is open-source under the MIT License. Feel free to modify and distribute.

---
