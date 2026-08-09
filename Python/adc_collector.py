import csv
import re
import time
from pathlib import Path

import serial


PORT = "COM3"
BAUDRATE = 115200
TIMEOUT = 3

NUM_POINTS = 5

PROJECT_ROOT = Path(__file__).resolve().parent.parent
RESULTS_DIR = PROJECT_ROOT / "Results"

OUTPUT_FILE = RESULTS_DIR / "adc_characterization.csv"


def read_adc(ser):
    ser.reset_input_buffer()

    ser.write(b"TEST ADC\r")
    ser.flush()

    deadline = time.time() + TIMEOUT

    while time.time() < deadline:

        raw = ser.readline()

        if not raw:
            continue

        line = raw.decode(
            "utf-8",
            errors="ignore"
        ).strip()

        match = re.search(
            r"\[ADC:AVG:(\d+)\]"
            r"\[MIN:(\d+)\]"
            r"\[MAX:(\d+)\]"
            r"\[MV:(\d+)\]",
            line
        )

        if match:

            avg, minimum, maximum, voltage = map(
                int,
                match.groups()
            )

            return avg, minimum, maximum, voltage

    return None


def main():

    RESULTS_DIR.mkdir(
        parents=True,
        exist_ok=True
    )

    print("=" * 45)
    print("STM32 ADC CHARACTERIZATION")
    print("=" * 45)
    print(f"Port     : {PORT}")
    print(f"Baudrate : {BAUDRATE}")
    print(f"Points   : {NUM_POINTS}")
    print()

    ser = serial.Serial(
        PORT,
        BAUDRATE,
        timeout=0.2
    )

    time.sleep(1)
    ser.reset_input_buffer()

    points = []

    for index in range(NUM_POINTS):

        print(
            f"Point {index + 1}/{NUM_POINTS}"
        )

        if index > 0:
            input(
                "Adjust potentiometer, "
                "then press ENTER..."
            )

        result = read_adc(ser)

        if result is None:

            print(
                "ERROR: ADC response not received."
            )

            ser.close()
            return

        avg, minimum, maximum, voltage = result

        print(
            f"ADC={avg} | "
            f"MIN={minimum} | "
            f"MAX={maximum} | "
            f"Voltage={voltage} mV"
        )

        points.append(
            {
                "point": index + 1,
                "adc_average": avg,
                "adc_min": minimum,
                "adc_max": maximum,
                "voltage_mV": voltage
            }
        )

        print()

    ser.close()

    with open(
        OUTPUT_FILE,
        "w",
        newline="",
        encoding="utf-8"
    ) as file:

        writer = csv.DictWriter(
            file,
            fieldnames=[
                "point",
                "adc_average",
                "adc_min",
                "adc_max",
                "voltage_mV"
            ]
        )

        writer.writeheader()
        writer.writerows(points)

    print("=" * 45)
    print("CHARACTERIZATION COMPLETE")
    print("=" * 45)
    print(f"CSV: {OUTPUT_FILE}")
    print(f"Points collected: {len(points)}")


if __name__ == "__main__":
    main()