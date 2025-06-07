import os
import serial
import csv

PORT = "COM4"  # Replace as needed
BAUD = 115200

print("Logging Start")

def main():
    ser = serial.Serial(PORT, BAUD)
    with open("C:/Users/peach/OneDrive/Documents/Gyro_Output_Data.csv", "w", newline="") as csvfile:
        csv_writer = None
        try:
            while True:
                line = ser.readline().decode('utf-8').strip()
                print(line)
                if not csv_writer:
                    csv_writer = csv.writer(csvfile)
                    csv_writer.writerow(["Date & Time", "Accel_x (m/s^2)", "Accel_y (m/s^2)", "Accel_z (m/s^2)", 
                                         "Gyro_x (deg/s)", "Gyro_y (deg/s)", "Gyro_z (deg/s)", "Temp (C)"])
                else:
                    csv_writer.writerow(line.split(","))
        except KeyboardInterrupt:
            print("Logging stopped.")
        finally:
            ser.close()

if __name__ == "__main__":
    main()
