# stm32-dma-spi-blackbox

Bare-metal STM32F446 DMA-driven SPI data logger with custom W25Q128 NOR flash driver

\# Solid-State Flight Data Recorder ("Black Box") — Bare-Metal STM32F4



An aerospace-inspired, crash-resilient \*\*Solid-State Flight Data Recorder (SSFDR)\*\* written in pure \*\*bare-metal Embedded C\*\* (register-level CMSIS, zero HAL/RTOS dependencies) on an \*\*STM32F411RE\*\* (ARM Cortex-M4). 



Modeled after commercial aviation flight recorders, this system captures high-frequency 6-axis inertial telemetry (`±16g` acceleration, `±2000 dps` angular rate), buffers pre- and post-impact samples in lock-free ring buffers, and commits \*\*256-byte page-aligned binary frames\*\* protected by \*\*hardware CRC-32\*\* checksums into non-volatile SPI NOR flash.



> \*\*Project Status:\*\* Active Development (Core packet protocol, lock-free ring buffers, hardware CRC-32 driver, and MPU-6500 register map complete; SPI/DMA bus arbitration and low-power `STOP` mode state machine in progress.)



\---



\## 1. System Use Cases \& Operational Modes



Designed for high-vibration, power-uncertain environments (UAVs, model rocketry, and high-value freight impact monitoring) where SD cards fail due to connector bounce or FAT filesystem corruption during sudden power loss:



1\. \*\*Ground Standby (Low-Power Mode):\*\* The STM32 enters deep \*\*`STOP` mode\*\* while the MPU-6500 operates in low-power accelerometer \*\*Wake-on-Motion (`WoM`)\*\* mode, drawing microamps until a takeoff or impact acceleration threshold is exceeded.

2\. \*\*Active Flight Telemetry (Continuous Circular Recording):\*\* Upon wake-up, the system streams 6-axis IMU readings at up to \*\*1,000 Hz\*\* via burst SPI/DMA into a circular pre-trigger ring buffer and flushes atomic 256-byte pages to NOR flash in an endless loop (overwriting oldest sectors when full).

3\. \*\*Crash / Hard-Landing Freeze \& Power-Loss Recovery:\*\* If a severe impact spike is detected, post-impact telemetry is recorded before locking memory. On reboot after a power yank, a \*\*boot-time recovery scanner\*\* validates page sync headers (`0xA5`) and CRC-32 checksums to reject half-written pages and locate the exact resume address.

4\. \*\*Ground-Station Diagnostic Offload:\*\* Streams raw binary flash pages over `USART2` (115,200 baud) to a Python ground-station script for CRC auditing, physical unit conversion, and multi-axis flight/impact visualization.



\---



\## 2. Hardware Bill of Materials (BOM)



| Component | Part Number / Module | Interface | Role in System |

| :--- | :--- | :--- | :--- |

| \*\*Microcontroller\*\* | STMicroelectronics \*\*STM32F411RETx\*\* (Nucleo-F411RE) | ARM Cortex-M4 (FPU) | Host MCU, hardware CRC-32 engine, DMA controller |

| \*\*Inertial Sensor (IMU)\*\* | TDK InvenSense \*\*MPU-6500\*\* (6-Axis Gyro + Accel) | `SPI1` + `EXTI` (`INT`) | High-G (`±16g`) motion sensing \& hardware Wake-on-Motion |

| \*\*Non-Volatile Memory\*\* | Winbond \*\*W25Q128JV\*\* (128-Mbit / 16 MB NOR Flash) | `SPI1` (Shared Bus) | Crash-survivable circular telemetry storage (65,536 pages) |

| \*\*Host Interface\*\* | Onboard ST-Link V2-1 Virtual COM Port (`USART2`) | UART (`PA2`/`PA3`) | Telemetry offloading and live fault diagnostics |



\### Hardware Pinout Mapping (`STM32F411RE`)

\* \*\*`SPI1` Shared Data Bus:\*\* `PA5` (`SCK`), `PA6` (`MISO`), `PA7` (`MOSI`) — Configured in Alternate Function `AF5`, Very High Speed.

\* \*\*Chip Select Lines (Active-Low):\*\* `PB6` (`W25Q128 /CS`), `PB0` (`MPU-6500 /CS`) — Controlled via atomic `GPIOB->BSRR` writes.

\* \*\*Interrupt Line:\*\* `EXTI` GPIO connected to MPU-6500 `INT` pin (routes `RAW\_RDY` and `WOM\_INT` hardware triggers).

\* \*\*Debug / Offload UART:\*\* `PA2` (`USART2\_TX`), `PA3` (`USART2\_RX`).



\---



\## 3. Key Engineering \& Design Constraints



\* \*\*Zero HAL / Zero Dynamic Allocation:\*\* Built strictly against the \*\*STM32F411 Reference Manual (`RM0383`)\*\* and \*\*CMSIS device headers\*\* with zero `malloc()` calls—all buffers are statically allocated at compile time (`< 3 KB` total SRAM footprint) to eliminate heap fragmentation and guarantee deterministic execution.

\* \*\*Physical Flash Page Alignment (`256 Bytes`):\*\* Every `Log\_Page\_t` struct is `\_\_attribute\_\_((packed))` and explicitly padded to \*\*exact 256 bytes\*\* (`8B Header` + `17 × 14B IMU Samples` + `6B Padding` + `4B CRC-32`) to match the physical page-program boundary of the W25Q128JV, preventing multi-page write splits and minimizing flash sector wear.

\* \*\*ARM Cortex-M4 32-Bit Memory Alignment:\*\* Explicit `reserved` and `padding` fields align `uint32\_t` sequence counters (offset `4`) and checksums (offset `252`) onto 4-byte word boundaries for single-cycle bus access and fault-free hardware `CRC->DR` feeding.

\* \*\*Shared-Bus Fault Detection \& Recovery (FDIR):\*\* All SPI transfers enforce bounded countdown timeouts, monitor `SPI\_SR\_OVR` (Overrun) flags, release `/CS` high on fault paths, and verify sensor health via `WHO\_AM\_I` (`0x75` = `0x70`) heartbeat checks.



\---



\## 4. Toolchain \& Compiler Configuration



\* \*\*IDE \& Build System:\*\* STM32CubeIDE (`make` build pipeline)

\* \*\*Compiler:\*\* `arm-none-eabi-gcc` (`-std=gnu11`)

\* \*\*Target Architecture Flags:\*\* `-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard`

\* \*\*Optimization \& Static Analysis:\*\* `-O0` (Debug) / `-O2` (Release), `-Wall -ffunction-sections -fdata-sections -fstack-usage -fcyclomatic-complexity`

\* \*\*Hardware Documentation Used:\*\*

&#x20; \* STMicroelectronics `RM0383` (STM32F411xC/E Reference Manual)

&#x20; \* TDK InvenSense `PS-MPU-6500A-01` \& `RM-MPU-6500A-00` (Product Spec \& Register Map)

&#x20; \* Winbond `W25Q128JV` Serial Flash Datasheet



\---



\## 5. Repository Architecture



```text

stm32-dma-spi-blackbox/

├── stm32-dma-spi-blackbox-firmware/

│   ├── core/

│   │   ├── log\_packet.h        # 256-byte packed Log\_Page\_t \& 14-byte IMU\_Sample\_t definitions

│   │   ├── ring\_buffer.h       # Lock-free power-of-two circular ring buffer interfaces

│   │   └── ring\_buffer.c       # Bitwise-masked FIFO read/write implementations

│   ├── drivers/

│   │   ├── mcu/

│   │   │   ├── crc.h / .c      # Bare-metal STM32 hardware CRC-32 peripheral driver

│   │   │   ├── spi.h / .c      # Register-level SPI1 master \& atomic BSRR chip-select driver

│   │   │   └── dma.h / .c      # \[Planned] Zero-copy DMA2 SPI burst controller

│   │   └── devices/

│   │       ├── mpu\_driver.h/.c # MPU-6500 register bitfields, burst-read, \& WoM driver

│   │       └── w25q128.h / .c  # \[Planned] Winbond NOR flash page/sector \& deep sleep driver

│   └── Src/

│       └── main.c              # System state machine \& hardware bring-up entry point

└── README.md

```



\---



\## 6. Build \& Flash Instructions



1\. Clone the repository:

&#x20;  ```bash

&#x20;  git clone https://github.com

&#x20;  ```

2\. Open \*\*STM32CubeIDE\*\* and import `stm32-dma-spi-blackbox-firmware` via \*\*File → Open Projects from File System...\*\*

3\. Verify Include Paths (`core`, `drivers/mcu`, `drivers/devices`) and Source Locations (`drivers`, `core`) are active under \*\*Project Properties → C/C++ General → Paths and Symbols\*\*.

4\. Build the `.elf` binary (\*\*Ctrl + B\*\*) and flash over \*\*SWD\*\* via the onboard ST-Link debugger.



