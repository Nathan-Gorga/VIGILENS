import pyxdf
import numpy as np

def extract_segment_to_csv(xdf_file: str, out_file: str,
                           start_idx: int, stop_idx: int,
                           channel: int = 0):
    """
    Extracts a segment [start_idx:stop_idx] from an EEG XDF file
    and saves time (x) and signal (y) as CSV.
    """
    # Load XDF
    data, _ = pyxdf.load_xdf(xdf_file)

    # Find EEG stream
    eeg_stream = None
    for s in data:
        if s['info']['type'][0] == 'EEG':
            eeg_stream = s
            break
    if eeg_stream is None:
        raise ValueError("No EEG stream found in file")

    signal = eeg_stream['time_series'][:, channel]  # pick one channel
    times = range(len(signal))


    # Extract segment
    x_seg = times[start_idx:stop_idx]
    y_seg = signal[start_idx:stop_idx]

    # Save as CSV
    arr = np.column_stack((x_seg, y_seg))
    np.savetxt(out_file, arr, delimiter=",", header="time,value", comments="")

    print(f"Saved {len(x_seg)} samples to {out_file}")


extract_segment_to_csv("../data/sub-P001/ses-S008/lslRecording_30_blinks_filter_bessel_0.1-15.xdf", "../filtered_blinks73.csv",22224, 22366, channel=0)
