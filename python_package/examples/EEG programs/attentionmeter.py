import time
import numpy as np
from brainflow.board_shim import BoardShim, BrainFlowInputParams
from brainflow.data_filter import DataFilter, FilterTypes, WindowOperations

# -------- SETUP PARAMETERS --------
board_id = 57  # NeuroPawn Knightboard
params = BrainFlowInputParams()
params.serial_port = "COM3"  # Change this to match your actual COM port

# Start session
BoardShim.enable_dev_board_logger()
board = BoardShim(57, params)
board.prepare_session()
board.start_stream()

# Get channel and sampling info
eeg_channels = BoardShim.get_eeg_channels(57)
sampling_rate = BoardShim.get_sampling_rate(57)
window_size = 30  # seconds
num_samples = window_size * sampling_rate

# -------- FUNCTION TO COMPUTE ALPHA/BETA RATIO --------
def compute_attention(channel_data):
    def band_power(data, low, high):
        psd, freqs = DataFilter.get_psd_welch(
            data, nfft=256, overlap=128, sampling_rate=sampling_rate,
            window=WindowOperations.HANNING.value
        )
        band_indices = np.where((freqs >= low) & (freqs <= high))
        return np.sum(psd[band_indices])

    alpha = band_power(channel_data, 8.0, 13.0)
    beta = band_power(channel_data, 13.0, 30.0)

    # Prevent division by zero
    return beta / alpha if alpha > 0 else 0

# -------- REAL-TIME LOOP --------
print("Starting real-time attention monitoring...\nPress Ctrl+C to stop.")
try:
    while True:
        # Wait to accumulate enough data
        time.sleep(window_size)

        # Get recent data window
        data = board.get_current_board_data(num_samples)
        attention_levels = []

        # Compute attention level for each EEG channel
        for ch in eeg_channels:
            ch_data = data[ch]
            attention = compute_attention(ch_data)
            attention_levels.append(attention)

        # You can take the average, or use just one channel
        avg_attention = np.mean(attention_levels)
        print(f"🧠 Attention Level (beta/alpha): {avg_attention:.2f}")

        # TODO: Send `avg_attention` to AR headset software (e.g., over WebSocket, MQTT, shared file, etc.)

except KeyboardInterrupt:
    print("\nStopping...")
finally:
    board.stop_stream()
    board.release_session()
