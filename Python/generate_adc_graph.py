import csv
from pathlib import Path

import matplotlib.pyplot as plt


PROJECT_ROOT = Path(__file__).resolve().parent.parent
RESULTS_DIR = PROJECT_ROOT / "Results"

INPUT_FILE = RESULTS_DIR / "adc_characterization.csv"
OUTPUT_FILE = RESULTS_DIR / "adc_characterization.png"


adc_values = []
voltage_values = []


with open(INPUT_FILE, "r", encoding="utf-8") as file:

    reader = csv.DictReader(file)

    for row in reader:

        adc_values.append(
            int(row["adc_average"])
        )

        voltage_values.append(
            int(row["voltage_mV"]) / 1000.0
        )


pairs = sorted(
    zip(voltage_values, adc_values)
)

voltage_values = [
    pair[0]
    for pair in pairs
]

adc_values = [
    pair[1]
    for pair in pairs
]


plt.figure(figsize=(8, 5))

plt.plot(
    voltage_values,
    adc_values,
    marker="o"
)

plt.xlabel("Input Voltage (V)")
plt.ylabel("ADC Code")

plt.title(
    "STM32F103 ADC Characterization"
)

plt.grid(True)

plt.tight_layout()

plt.savefig(
    OUTPUT_FILE,
    dpi=200
)

plt.close()

print("ADC characterization graph generated.")
print(f"Input : {INPUT_FILE}")
print(f"Output: {OUTPUT_FILE}")
print(f"Points: {len(adc_values)}")