# Required Dependencies
- python 3.x
- brainflow library (pip install brainflow)
- numpy (pip install numpy)
- pandas (pip install pandas)
- openpyxl (pip install openpyxl)

# Setup
For hardware setup requirements, view the *currently* internal documentation on the BCI created by Ferzam Mahmood, Farabi Hasan, and Tobenna Agu.

## Current Setup

1. The script sets up connection parameters (serial port `COMX`). Note that X will vary based on who is using it. If you are unsure a method of checking the port will be added here in the future. 

A file in the same directory exists (board_id.py). This is for retrieving board numbers from BrainFlow. Running it gives 57 for the Knight Board and should be the same for everyone. If this error is encountered, run this file and find the Knight Board ID and replace the respective parameter in UCXR_BCI.py

2. The data starts to stream from the board for the specified time stamp (time.sleep(x)). Note that 'x' should be an integer and specifies the total timeframe data is collected.

3. The data recieved prior to processing and export to excel is a 2D NumPy array with dimensions that correspond to (channels x samples).

4.  Timestamps in seconds are created based on the sampling rate and prepended as the first row.








