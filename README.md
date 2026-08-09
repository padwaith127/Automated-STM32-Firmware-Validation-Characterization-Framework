\# Automated STM32 Firmware Validation \& Characterization Framework



An automated embedded firmware validation and characterization framework built around the \*\*STM32F103C8T6 (Blue Pill)\*\*.



The framework combines STM32 firmware-based peripheral validation with a Python automation layer to execute tests, capture structured results, perform ADC characterization, and generate validation evidence.



\---



\## Overview



This project demonstrates a practical firmware validation workflow for an STM32-based embedded system.



The STM32 firmware provides command-driven validation of:



\- UART

\- GPIO

\- 12-bit ADC

\- I2C

\- SPI



A Python automation layer communicates with the STM32 through UART and automatically:



1\. Sends validation commands

2\. Captures STM32 responses

3\. Determines PASS/FAIL status

4\. Extracts measurement data

5\. Generates structured CSV logs

6\. Performs ADC characterization

7\. Generates ADC characterization plots



\---



\## System Architecture



```text

&#x20;                   ┌─────────────────────────┐

&#x20;                   │      Python Runner      │

&#x20;                   │                         │

&#x20;                   │  validation\_runner.py   │

&#x20;                   └────────────┬────────────┘

&#x20;                                │

&#x20;                          USB-UART / COM3

&#x20;                                │

&#x20;                                ▼

&#x20;                   ┌─────────────────────────┐

&#x20;                   │      STM32F103C8T6      │

&#x20;                   │       Blue Pill         │

&#x20;                   │                         │

&#x20;                   │  Firmware Validation    │

&#x20;                   │        Core             │

&#x20;                   └────────────┬────────────┘

&#x20;                                │

&#x20;         ┌──────────────────────┼──────────────────────┐

&#x20;         │                      │                      │

&#x20;         ▼                      ▼                      ▼

&#x20;      UART/GPIO               ADC                    I2C

&#x20;                                                     │

&#x20;                                                     ▼

&#x20;                                                 ADS1115



&#x20;                                │

&#x20;                                ▼

&#x20;                               SPI

&#x20;                                │

&#x20;                                ▼

&#x20;                             ADXL345

```



\---



\## Hardware



\### Main Controller



\- STM32F103C8T6 Blue Pill

\- ARM Cortex-M3

\- 12-bit internal ADC

\- SPI

\- I2C

\- USART



\### Validation Hardware



\- CP2102 USB-to-TTL converter

\- Potentiometer for ADC characterization

\- ADS1115 I2C ADC module

\- ADXL345 accelerometer module

\- Breadboard

\- Jumper wires



\---



\## Peripheral Configuration



| Peripheral | STM32 Interface | Validation Target |

|---|---|---|

| UART | USART1 | USB-UART communication |

| ADC | PA0 / ADC1 Channel 0 | Potentiometer |

| I2C | I2C1 | ADS1115 |

| SPI | SPI1 | ADXL345 |

| GPIO | PC13 | On-board LED |



\---



\## UART Configuration



```text

Baudrate : 115200

Format   : 8-N-1

```



\---



\## SPI / ADXL345



The ADXL345 is connected to SPI1.



| STM32 | ADXL345 |

|---|---|

| PA5 | SCLK |

| PA6 | SDO / MISO |

| PA7 | SDI / MOSI |

| PB0 | CS |

| 3.3V | VCC |

| GND | GND |



The ADXL345 device ID register is validated against:



```text

0xE5

```



Example validation result:



```text

\[TEST:SPI]\[RESULT:PASS]\[DEVID:0xE5]

```



\---



\## I2C / ADS1115



The ADS1115 is connected to the STM32 I2C interface.



The validation framework detects the device at:



```text

0x48

```



Example:



```text

\[TEST:I2C]\[RESULT:PASS]\[ADDR:0x48]

```



\---



\# Firmware Validation



The STM32 firmware provides a command-line validation interface through UART.



\## Available Commands



```text

HELP

TEST UART

TEST GPIO

TEST ADC

TEST I2C

TEST SPI

TEST ALL

```



\---



\## Example Firmware Validation



```text

\[TEST:UART]\[RESULT:PASS]

\[TEST:GPIO]\[RESULT:PASS]

\[TEST:ADC]\[RESULT:PASS]

\[ADC:AVG:3344]\[MIN:3337]\[MAX:3356]\[MV:2694]

\[TEST:I2C]\[RESULT:PASS]\[ADDR:0x48]

\[TEST:SPI]\[RESULT:PASS]\[DEVID:0xE5]

\[TEST:ALL]\[RESULT:PASS]

```



\---



\# ADC Validation



The internal STM32 ADC is configured as:



```text

ADC        : ADC1

Channel    : Channel 0

Pin        : PA0

Resolution : 12-bit

Reference  : 3.3 V

```



The firmware collects multiple ADC samples and reports:



\- Average ADC code

\- Minimum ADC code

\- Maximum ADC code

\- Calculated voltage



Example:



```text

\[TEST:ADC]\[RESULT:PASS]

\[ADC:AVG:3347]\[MIN:3339]\[MAX:3358]\[MV:2697]

```



\---



\# ADC Characterization



A potentiometer connected to PA0 was used to generate multiple input-voltage levels.



The Python characterization collector captured five real measurement points from the STM32 ADC.



\## Measured Data



| Point | ADC Average | ADC Min | ADC Max | Voltage |

|---:|---:|---:|---:|---:|

| 1 | 0 | 0 | 0 | 0 mV |

| 2 | 940 | 938 | 949 | 757 mV |

| 3 | 1841 | 1838 | 1846 | 1483 mV |

| 4 | 2524 | 2512 | 2535 | 2033 mV |

| 5 | 3177 | 3171 | 3183 | 2560 mV |



The characterization dataset is stored at:



```text

Results/adc\_characterization.csv

```



The characterization graph is stored at:



```text

Results/stm32\_adc\_characterization.png

```



\---



\# Automated Python Validation



The Python automation layer uses \*\*PySerial\*\* to communicate with the STM32 through COM3.



The main validation script is:



```text

Python/validation\_runner.py

```



It executes:



```text

TEST UART

TEST GPIO

TEST ADC

TEST I2C

TEST SPI

```



and determines the overall validation result.



\## Example Automated Run



```text

STM32 VALIDATION AUTOMATION



Port     : COM3

Baudrate : 115200



Running UART...

STM32: \[TEST:UART]\[RESULT:PASS]

Result: PASS



Running GPIO...

STM32: \[TEST:GPIO]\[RESULT:PASS]

Result: PASS



Running ADC...

STM32: \[TEST:ADC]\[RESULT:PASS]

STM32: \[ADC:AVG:3347]\[MIN:3339]\[MAX:3358]\[MV:2697]

Result: PASS

Data  : AVG=3347, MIN=3339, MAX=3358, MV=2697



Running I2C...

STM32: \[TEST:I2C]\[RESULT:PASS]\[ADDR:0x48]

Result: PASS

Data  : ADDR=0x48



Running SPI...

STM32: \[TEST:SPI]\[RESULT:PASS]\[DEVID:0xE5]

Result: PASS

Data  : DEVID=0xE5



\## OVERALL RESULT: PASS

```



\---



\# Structured Validation Logging



Validation results are stored as CSV records containing:



\- Timestamp

\- Test ID

\- Test name

\- Command

\- Expected result

\- Actual result

\- Measurement

\- Error information



Example:



```csv

timestamp,test\_id,test\_name,command,expected,actual,measurement,error

2026-08-09T23:01:07,TEST\_UART,UART,TEST UART,PASS,PASS,,

2026-08-09T23:01:07,TEST\_GPIO,GPIO,TEST GPIO,PASS,PASS,,

2026-08-09T23:01:08,TEST\_ADC,ADC,TEST ADC,PASS,PASS,"AVG=3339, MIN=3297, MAX=3346, MV=2690",

2026-08-09T23:01:08,TEST\_I2C,I2C,TEST I2C,PASS,PASS,ADDR=0x48,

2026-08-09T23:01:09,TEST\_SPI,SPI,TEST SPI,PASS,PASS,DEVID=0xE5,

```



\---



\# Python Tools



\## Validation Runner



```text

Python/validation\_runner.py

```



Runs the complete UART, GPIO, ADC, I2C and SPI validation sequence.



\---



\## ADC Collector



```text

Python/adc\_collector.py

```



Collects real ADC measurements from the STM32 through UART and stores the measurements in CSV format.



\---



\## ADC Characterization



```text

Python/adc\_characterization.py

```



Processes ADC characterization data.



\---



\## ADC Graph Generator



```text

Python/generate\_adc\_graph.py

```



Generates the ADC characterization graph from the collected CSV data.



\---



\# Validation Evidence



The repository contains captured evidence for the implemented validation and characterization workflow.



\## STM32CubeIDE Build



The firmware was successfully built with:



```text

0 errors

0 warnings

```



Evidence:



```text

Results/stm32\_cubeide\_build\_success.png

```



\## UART / Peripheral Validation



Evidence:



```text

Results/stm32\_uart\_validation\_pass.png

```



\## Python Automated Validation



Evidence:



```text

Results/python\_automated\_validation\_pass.png

```



The automated validation demonstrated:



```text

OVERALL RESULT: PASS

```



\## ADC Characterization



Evidence:



```text

Results/stm32\_adc\_characterization.png

```



\## GitHub Repository



Repository evidence:



```text

Results/github\_repository.png

```



\---



\# Project Structure



```text

Automated-STM32-Firmware-Validation-Characterization-Framework/

│

├── Firmware/

│   └── STM32\_Validation\_Framework/

│       └── STM32\_Validation\_Framework/

│           ├── Core/

│           │   ├── Inc/

│           │   └── Src/

│           │

│           ├── Drivers/

│           │

│           └── STM32\_Validation\_Framework.ioc

│

├── Python/

│   ├── validation\_runner.py

│   ├── adc\_collector.py

│   ├── adc\_characterization.py

│   └── generate\_adc\_graph.py

│

├── Results/

│   ├── adc\_characterization.csv

│   ├── github\_repository.png

│   ├── python\_automated\_validation\_pass.png

│   ├── stm32\_adc\_characterization.png

│   ├── stm32\_cubeide\_build\_success.png

│   └── stm32\_uart\_validation\_pass.png

│

├── .gitignore

└── README.md

```



\---



\# Development Workflow



The project follows a firmware validation workflow:



```text

Hardware Setup

&#x20;     ↓

STM32 Firmware Configuration

&#x20;     ↓

Peripheral Initialization

&#x20;     ↓

Individual Peripheral Tests

&#x20;     ↓

UART Command Interface

&#x20;     ↓

Python Automation

&#x20;     ↓

PASS / FAIL Evaluation

&#x20;     ↓

CSV Logging

&#x20;     ↓

ADC Characterization

&#x20;     ↓

Graph Generation

&#x20;     ↓

Git Version Control

&#x20;     ↓

GitHub Repository

```



\---



\# Technologies Used



\## Firmware



\- Embedded C

\- STM32 HAL

\- STM32F103C8T6

\- ARM Cortex-M3

\- STM32CubeMX

\- STM32CubeIDE

\- STM32CubeProgrammer



\## Communication Protocols



\- UART

\- I2C

\- SPI



\## Python Automation



\- Python 3.11

\- PySerial

\- CSV logging

\- ADC characterization

\- Data visualization



\## Development and Validation Tools



\- Git

\- GitHub

\- Tera Term

\- STM32CubeIDE

\- STM32CubeProgrammer



\---



\# Git Development History



The framework was developed incrementally using Git.



```text

202d063  Add validation evidence and ADC characterization results

499989c  Complete automated STM32 validation and characterization

9fd46d0  Add I2C and SPI peripheral validation

1d2c46f  Add STM32 UART GPIO and ADC validation core

2f1db9d  Initial STM32 validation framework baseline

```



\---



\# Future Improvements



Potential extensions include:



\- Automated GPIO electrical characterization

\- ADC linearity and error analysis

\- ADC INL/DNL characterization

\- Automated SPI/I2C timing validation

\- Sensor register-level validation

\- Oscilloscope and logic-analyzer integration

\- Automated HTML/PDF test reports

\- Hardware-in-the-loop regression testing

\- Additional STM32 peripheral validation

\- Timer and PWM validation

\- CAN validation

\- DAC validation

\- Automated regression-test execution



These items are planned extensions and are not represented as currently implemented features.



\---



\# Project Outcome



The current implementation demonstrates:



\- STM32 firmware bring-up

\- UART command-driven validation

\- GPIO validation

\- Internal ADC validation

\- I2C device detection

\- SPI device identification

\- Python-based automated testing

\- Structured validation logging

\- Real ADC characterization

\- ADC data visualization

\- Git-based version control

\- GitHub-based project management



The framework achieved a complete automated validation result of:



```text

OVERALL RESULT: PASS

```



\---



\# Author



\*\*Adwaith P\*\*



B.Tech Electrical \& Electronics Engineering  

NIT Calicut



GitHub:



https://github.com/padwaith127

