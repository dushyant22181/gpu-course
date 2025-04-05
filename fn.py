import os
import cv2
import numpy as np
from pathlib import Path
import multiprocessing as mp
from tqdm import tqdm  # For progress bar

# Function to process a single image
def process_image(image_path, output_dir):
    try:
        # Read the image in grayscale
        img = cv2.imread(str(image_path), cv2.IMREAD_GRAYSCALE)
        if img is None:
            print(f"Failed to load {image_path}")
            return
        
        # Apply Canny edge detection
        edges = cv2.Canny(img, 100, 200)  # Thresholds can be adjusted
        
        # Define output path
        output_path = output_dir / f"{image_path.stem}_edges{image_path.suffix}"
        
        # Save the processed image
        cv2.imwrite(str(output_path), edges)
    except Exception as e:
        print(f"Error processing {image_path}: {e}")

# Main function to handle batch processing
def batch_process_images(input_dir, output_dir, num_workers=4):
    # Ensure directories are Path objects
    input_dir = Path(input_dir)
    output_dir = Path(output_dir)
    
    # Create output directory if it doesn't exist
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # Get list of image files (assuming .jpg, .png, etc.)
    image_extensions = ('.jpg', '.jpeg', '.png', '.bmp')
    image_paths = [f for f in input_dir.glob("*") if f.suffix.lower() in image_extensions]
    
    if not image_paths:
        print("No images found in the input directory!")
        return
    
    print(f"Found {len(image_paths)} images to process.")
    
    # Use multiprocessing to speed up processing
    with mp.Pool(processes=num_workers) as pool:
        # Prepare arguments for each process
        args = [(img_path, output_dir) for img_path in image_paths]
        
        # Process images with a progress bar
        list(tqdm(pool.starmap(process_image, args), total=len(image_paths)))
    
    print("Processing complete!")

# Example usage
if __name__ == "__main__":
    # Define input and output directories
    INPUT_DIR = "input_images"  # Folder with your images
    OUTPUT_DIR = "output_edges"  # Where processed images will go
    
    # Number of CPU cores to use (adjust based on your machine)
    NUM_WORKERS = mp.cpu_count() - 1  # Leave one core free
    
    # Run the batch processing
    batch_process_images(INPUT_DIR, OUTPUT_DIR, NUM_WORKERS)
