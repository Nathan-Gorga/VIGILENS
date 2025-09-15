import pyxdf

# Load file
data, header = pyxdf.load_xdf("../data/sub-P001/ses-S008/lslRecording_30_blinks_filter_bessel_0.1-15.xdf")

print("Number of streams:", len(data))

# Print info about streams
for i, stream in enumerate(data):
    print(f"\nStream {i}:")
    print("  Name:", stream['info']['name'][0])
    print("  Type:", stream['info']['type'][0])
    print("  Sample count:", len(stream['time_series']))
    print("  Channel count:", stream['info']['channel_count'][0])


# Find EEG stream
eeg_stream = None
for stream in data:
    if stream['info']['type'][0] == 'EEG':
        eeg_stream = stream
        break

if eeg_stream:
    eeg = eeg_stream['time_series']   # shape: (n_samples, n_channels)
    times = eeg_stream['time_stamps'] # timestamps
    print("EEG shape:", eeg.shape)
else:
    print("No EEG stream found!")


import matplotlib.pyplot as plt

x = range(len(times))

plt.plot(x, eeg[:, 0])  # first EEG channel
plt.xlabel("Time (s)")
plt.ylabel("EEG (µV)")
plt.title("EEG channel 1")
plt.show()


