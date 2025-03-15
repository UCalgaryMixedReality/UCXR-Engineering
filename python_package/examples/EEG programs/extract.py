import numpy as np
from brainflow.board_shim import BoardShim, BrainFlowInputParams
from brainflow.data_filter import DataFilter, FilterTypes, WindowOperations

#Initialize pawn board
# Set board parameters
board_id = 57  # Neuro Pawn Board ID
params = BrainFlowInputParams()
params.serial_port = "COM3"  # Change this to your device's serial port

# Create board object
board = BoardShim(board_id, params)

# Prepare and start streaming
board.prepare_session()
board.start_stream()

#Obtain raw EEG data

# Get latest board data (1000 samples)
data = board.get_current_board_data(1000)

# Get EEG channel indices
eeg_channels = BoardShim.get_eeg_channels(board_id)

# Extract only EEG data
eeg_data = data[eeg_channels, :]

#Apply filters

sampling_rate = BoardShim.get_sampling_rate(board_id)

BANDS = {
    "theta": (4.0, 8.0),
    "alpha": (8.0, 13.0),
    "beta": (13.0, 30.0)
}

filtered_signals = {}

for band, (low, high) in BANDS.items():
    filtered_signals[band] = eeg_data.copy()
    for i in range(filtered_signals[band].shape[0]):
        DataFilter.perform_bandpass(filtered_signals[band][i], sampling_rate, low, high, 
                                    order=4, filter_type=FilterTypes.BUTTERWORTH.value, ripple=0.5)

#Compute Power Spectral Density (PSD)
def get_band_power(data, band_range, sampling_rate):
    psd, freqs = DataFilter.get_psd_welch(data, nfft=256, overlap=128, sampling_rate=sampling_rate, window=WindowOperations.HANNING.value)
    
    # Extract only power within the band
    indices = np.where((freqs >= band_range[0]) & (freqs <= band_range[1]))
    return np.sum(psd[indices])

# Compute band powers
theta_power = get_band_power(filtered_signals['theta'], BANDS['theta'], sampling_rate)
alpha_power = get_band_power(filtered_signals['alpha'], BANDS['alpha'], sampling_rate)
beta_power = get_band_power(filtered_signals['beta'], BANDS['beta'], sampling_rate)

print(f"Theta Power: {theta_power}, Alpha Power: {alpha_power}, Beta Power: {beta_power}")

