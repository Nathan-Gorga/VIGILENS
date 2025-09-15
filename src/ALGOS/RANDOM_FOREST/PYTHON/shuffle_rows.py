import pandas as pd
import os

# -----------------------------
# Parameters
# -----------------------------
input_file = "../eeg_blinks.csv"  # Replace with your CSV file
output_file = "new_eeg_data.csv"   # Output file name

# -----------------------------
# Load CSV
# -----------------------------
df = pd.read_csv(input_file)

# -----------------------------
# Shuffle rows
# -----------------------------
shuffled_df = df.sample(frac=1).reset_index(drop=True)

# -----------------------------
# Save shuffled CSV
# -----------------------------
shuffled_df.to_csv(output_file, index=False)
print(f"Shuffled CSV saved as {output_file}")
