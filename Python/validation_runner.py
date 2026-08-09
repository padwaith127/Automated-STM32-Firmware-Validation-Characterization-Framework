import csv
import re
import time
from datetime import datetime

import serial


PORT = "COM3"
BAUDRATE = 115200
TIMEOUT = 3.0

TESTS = [
    ("UART", "TEST UART"),
    ("GPIO", "TEST GPIO"),
    ("ADC", "TEST ADC"),
    ("I2C", "TEST I2C"),
    ("SPI", "TEST SPI"),
]


def read_test_response(ser, test_name):
    deadline = time.time() + TIMEOUT
    captured = []
    result_received = False

    while time.time() < deadline:
        raw = ser.readline()

        if not raw:
            continue

        line = raw.decode("utf-8", errors="ignore").strip()

        if not line:
            continue

        captured.append(line)

        if f"[TEST:{test_name}]" in line:
            result_received = True

        # ADC sends its measurement immediately after the PASS line.
        if test_name == "ADC" and result_received:
            if "[ADC:AVG:" in line:
                return captured

        elif test_name != "ADC" and result_received:
            return captured

    return captured


def parse_result(test_name, lines):
    result = "FAIL"
    measurement = ""

    for line in lines:

        if f"[TEST:{test_name}]" in line:

            if "[RESULT:PASS]" in line:
                result = "PASS"

            elif "[RESULT:FAIL]" in line:
                result = "FAIL"

            if test_name == "I2C":

                match = re.search(
                    r"\[ADDR:(0x[0-9A-Fa-f]+)\]",
                    line
                )

                if match:
                    measurement = f"ADDR={match.group(1)}"

            elif test_name == "SPI":

                match = re.search(
                    r"\[DEVID:(0x[0-9A-Fa-f]+)\]",
                    line
                )

                if match:
                    measurement = f"DEVID={match.group(1)}"

        if test_name == "ADC":

            match = re.search(
                r"\[ADC:AVG:(\d+)\]"
                r"\[MIN:(\d+)\]"
                r"\[MAX:(\d+)\]"
                r"\[MV:(\d+)\]",
                line
            )

            if match:

                avg, minimum, maximum, mv = match.groups()

                measurement = (
                    f"AVG={avg}, "
                    f"MIN={minimum}, "
                    f"MAX={maximum}, "
                    f"MV={mv}"
                )

    return result, measurement


def main():

    print("=" * 45)
    print("STM32 VALIDATION AUTOMATION")
    print("=" * 45)

    print(f"Port     : {PORT}")
    print(f"Baudrate : {BAUDRATE}")
    print()

    try:

        ser = serial.Serial(
            port=PORT,
            baudrate=BAUDRATE,
            timeout=0.2
        )

    except serial.SerialException as exc:

        print(f"ERROR: Could not open {PORT}")
        print(exc)
        return

    time.sleep(1)

    ser.reset_input_buffer()

    results = []

    for test_name, command in TESTS:

        print(f"Running {test_name}...")

        ser.reset_input_buffer()

        # STM32 command parser uses CR as the command terminator.
        ser.write(
            (command + "\r").encode("ascii")
        )

        ser.flush()

        lines = read_test_response(
            ser,
            test_name
        )

        result, measurement = parse_result(
            test_name,
            lines
        )

        for line in lines:
            print(f"  STM32: {line}")

        results.append(
            {
                "timestamp":
                    datetime.now().isoformat(
                        timespec="seconds"
                    ),

                "test_id":
                    f"TEST_{test_name}",

                "test_name":
                    test_name,

                "command":
                    command,

                "expected":
                    "PASS",

                "actual":
                    result,

                "measurement":
                    measurement,

                "error":
                    ""
                    if result == "PASS"
                    else
                    "No valid PASS response"
            }
        )

        print(f"  Result: {result}")

        if measurement:
            print(f"  Data  : {measurement}")

        print()

        time.sleep(0.5)

    ser.close()

    csv_file = "validation_results.csv"

    with open(
        csv_file,
        "w",
        newline="",
        encoding="utf-8"
    ) as file:

        writer = csv.DictWriter(
            file,
            fieldnames=[
                "timestamp",
                "test_id",
                "test_name",
                "command",
                "expected",
                "actual",
                "measurement",
                "error"
            ]
        )

        writer.writeheader()
        writer.writerows(results)

    overall = all(
        item["actual"] == "PASS"
        for item in results
    )

    print("=" * 45)
    print("STM32 VALIDATION TEST REPORT")
    print("=" * 45)

    for item in results:

        print(
            f"{item['test_name']:<10}"
            f"{item['actual']}"
        )

    print("-" * 45)

    if overall:
        print("OVERALL RESULT: PASS")
    else:
        print("OVERALL RESULT: FAIL")

    print("-" * 45)
    print(f"CSV LOG: {csv_file}")


if __name__ == "__main__":
    main()