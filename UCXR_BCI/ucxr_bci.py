from brainflow.board_shim import BoardShim, BrainFlowInputParams, LogLevels
import numpy as np
import pandas as pd
import time
import os

BoardShim.enable_dev_board_logger()

params = BrainFlowInputParams()
params.serial_port = "COM5"
NEUROPAWN_BOARD_ID = 57

board = BoardShim(NEUROPAWN_BOARD_ID, params)

try:
    board.prepare_session()
    board.start_stream()
    print("Recording data for 10 seconds...")
    time.sleep(10)

    data = board.get_board_data()  # shape: (channels, samples)
    sampling_rate = BoardShim.get_sampling_rate(NEUROPAWN_BOARD_ID)
    timestamps = np.arange(data.shape[1]) / sampling_rate
    data_with_time = np.vstack((timestamps, data))

    # Get channel labels from the board descriptor
    board_descr = BoardShim.get_board_descr(NEUROPAWN_BOARD_ID)
    channel_labels = board_descr.get('labels', [])

    # Add "timestamp" label at front to match data_with_time shape
    column_labels = ["timestamp"] + channel_labels

    # Safety check: if mismatch, truncate or pad labels to match data columns
    if len(column_labels) > data_with_time.shape[0]:
        column_labels = column_labels[:data_with_time.shape[0]]
    elif len(column_labels) < data_with_time.shape[0]:
        # pad with generic names if fewer labels
        column_labels += [f"channel_{i}" for i in range(len(column_labels), data_with_time.shape[0])]

    df = pd.DataFrame(data_with_time.T, columns=column_labels)

    save_path = "neuropawn_data_log.xlsx"
    df.to_excel(save_path, index=False)

    print(f"Data saved to: {os.path.abspath(save_path)}")

finally:
    board.stop_stream()
    board.release_session()
