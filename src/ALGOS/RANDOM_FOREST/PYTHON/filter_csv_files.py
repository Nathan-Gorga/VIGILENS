import os
import glob
import numpy as np
import pandas as pd
from scipy.signal import butter, filtfilt

# -----------------------------
# Bandpass filter function
# -----------------------------
def butter_bandpass(lowcut: float, highcut: float, fs: float, order: int = 4):
    nyq = 0.5 * fs  # Nyquist frequency
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def apply_bandpass_filter(data: np.ndarray, lowcut: float, highcut: float, fs: float, order: int = 4) -> np.ndarray:
    b, a = butter_bandpass(lowcut, highcut, fs, order)
    y = filtfilt(b, a, data)
    return y

# -----------------------------
# Main processing
# -----------------------------
def process_csv_folder(folder: str, fs: float, lowcut: float, highcut: float, order: int = 4):
    csv_files = glob.glob(os.path.join(folder, "*.csv"))
    
    for file in csv_files:
        print(f"Processing {file} ...")
        
        # Load CSV (expects two columns: time, value)
        df = pd.read_csv(file)
        
        if df.shape[1] < 2:
            print(f"Skipping {file}: not enough columns (expected time, value)")
            continue
        
        time_data = df.iloc[:, 0].values   # first column = time
        value_data = df.iloc[:, 1].values  # second column = signal values
        
        # Apply bandpass filter
        filtered_value = apply_bandpass_filter(value_data, lowcut, highcut, fs, order)
        
        # Combine time and filtered value into new DataFrame
        new_df = pd.DataFrame({
            "time": time_data,
            "value": filtered_value
        })
        
        # Save in current working directory instead of original folder
        base_name = os.path.basename(file)  # filename only, no path
        new_filename = os.path.splitext(base_name)[0] + f"_filtered_{lowcut}_{highcut}_{order}.csv"
        new_path = os.path.join(os.getcwd(), new_filename)
        
        new_df.to_csv(new_path, index=False)
        print(f"Saved {new_path}")

# -----------------------------
# Example usage
# -----------------------------
if __name__ == "__main__":
    folder = "../data/csv/"  # <- Change this to where your input CSVs are
    fs = 250.0       # Sampling frequency (Hz) – set to your data’s fs
    lowcut = 0.5     # Low cutoff frequency (Hz)
    highcut = 10.0   # High cutoff frequency (Hz)
    order = 4        # Filter order

    process_csv_folder(folder, fs, lowcut, highcut, order)
