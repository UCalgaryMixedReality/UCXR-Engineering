import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from openpyxl import Workbook
from brainflow.board_shim import BoardShim, BrainFlowInputParams
from brainflow.data_filter import DataFilter, FilterTypes, WindowOperations

# ---- 1. Initialize the Board and Start Streaming ----

board_id = 57  # Neuro Pawn Board ID
params = BrainFlowInputParams()
params.serial_port = "COM3"  # Change this to match your device

board = BoardShim(board_id, params)
board.prepare_session()
board.start_stream()

# ---- 2. Obtain Raw EEG Data ----

num_samples = 1000  # Number of samples to collect
data = board.get_current_board_data(num_samples)

# Get EEG channel indices
eeg_channels = BoardShim.get_eeg_channels(board_id)
sampling_rate = BoardShim.get_sampling_rate(board_id)

# Extract EEG data
eeg_data = data[eeg_channels, :]

# ---- 3. Apply Bandpass Filters for Different EEG Bands ----

BANDS = {
    "theta": (4.0, 8.0),
    "alpha": (8.0, 13.0),
    "beta": (13.0, 30.0)
}

filtered_signals = {}

for band, (low, high) in BANDS.items():
    filtered_signals[band] = eeg_data.copy()
    for i in range(filtered_signals[band].shape[0]):
        DataFilter.perform_bandpass(
            filtered_signals[band][i], sampling_rate, low, high, 
            order=4, filter_type=FilterTypes.BUTTERWORTH.value, ripple=0.5
        )

# ---- 4. Compute Power Spectral Density (PSD) ----

def get_band_power(data, band_range, sampling_rate):
    psd, freqs = DataFilter.get_psd_welch(
        data, nfft=256, overlap=128, sampling_rate=sampling_rate, 
        window=WindowOperations.HANNING.value
    )
    indices = np.where((freqs >= band_range[0]) & (freqs <= band_range[1]))
    return np.sum(psd[indices])

theta_power = get_band_power(filtered_signals['theta'], BANDS['theta'], sampling_rate)
alpha_power = get_band_power(filtered_signals['alpha'], BANDS['alpha'], sampling_rate)
beta_power = get_band_power(filtered_signals['beta'], BANDS['beta'], sampling_rate)

print(f"Theta Power: {theta_power}, Alpha Power: {alpha_power}, Beta Power: {beta_power}")

# ---- 5. Save Data to Excel ----

# Create a Pandas DataFrame
df = pd.DataFrame({
    "Time (s)": np.arange(num_samples) / sampling_rate,
    "Theta": filtered_signals['theta'][0],
    "Alpha": filtered_signals['alpha'][0],
    "Beta": filtered_signals['beta'][0]
})

# Save to Excel
excel_file = "EEG_Band_Power.xlsx"
df.to_excel(excel_file, index=False)

print(f"Data saved to {excel_file}")

# ---- 6. Plot EEG Data ----

plt.figure(figsize=(12, 6))
plt.plot(df["Time (s)"], df["Theta"], label="Theta (4-8 Hz)")
plt.plot(df["Time (s)"], df["Alpha"], label="Alpha (8-13 Hz)")
plt.plot(df["Time (s)"], df["Beta"], label="Beta (13-30 Hz)")
plt.xlabel("Time (s)")
plt.ylabel("EEG Signal Amplitude")
plt.title("Filtered EEG Data")
plt.legend()
plt.grid()
plt.show()
