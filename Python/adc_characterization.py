import csv
import re
from pathlib import Path

import matplotlib.pyplot as plt


PROJECT_ROOT = Path(__file__).resolve().parent.parent
RESULTS_DIR = PROJECT_ROOT / "Results"

INPUT_FILE = Path(__file__).resolve().parent / "validation_results.csv"

OUTPUT_CSV = RESULTS_DIR / "adc_characterization.csv"
OUTPUT_PNG = RESULTS_DIR / "adc_characterization.png"


def main():
    RESULTS_DIR.mkdir(parents=True, exist_ok=True)

    points = []

    with open(INPUT_FILE, "r", newline="", encoding="utf-8") as file:
        reader = csv.DictReader(file)

        for row in reader:

            if row["test_name"] != "ADC":
                continue

            measurement = row["measurement"]

            match = re.search(
                r"AVG=(\d+), MIN=(\d+), MAX=(\d+), MV=(\d+)",
                measurement
            )

            if not match:
                continue

            avg, minimum, maximum, voltage = map(
                int,
                match.groups()
            )

            points.append(
                {
                    "timestamp": row["timestamp"],
                    "adc_average": avg,
                    "adc_min": minimum,
                    "adc_max": maximum,
                    "voltage_mV": voltage,
                }
            )

    if not points:
        print("No ADC characterization data found.")
        return

    with open(
        OUTPUT_CSV,
        "w",
        newline="",
        encoding="utf-8"
    ) as file:

        fieldnames = [
            "timestamp",
            "adc_average",
            "adc_min",
            "adc_max",
            "voltage_mV",
        ]

        writer = csv.DictWriter(
            file,
            fieldnames=fieldnames
        )

        writer.writeheader()
        writer.writerows(points)

    adc_values = [
        point["adc_average"]
        for point in points
    ]

    voltage_values = [
        point["voltage_mV"] / 1000.0
        for point in points
    ]

    plt.figure(figsize=(8, 5))

    plt.plot(
        voltage_values,
        adc_values,
        marker="o"
    )

    plt.xlabel("Measured Voltage (V)")
    plt.ylabel("ADC Average Code")
    plt.title("STM32F103 ADC Characterization")
    plt.grid(True)

    plt.tight_layout()

    plt.savefig(
        OUTPUT_PNG,
        dpi=200
    )

    plt.close()

    print("ADC characterization complete.")
    print(f"CSV : {OUTPUT_CSV}")
    print(f"Graph: {OUTPUT_PNG}")
    print(f"Points: {len(points)}")


if __name__ == "__main__":
    main()