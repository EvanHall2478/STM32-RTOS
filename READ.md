# Simple RTOS for STM32-Nucleo

## Overview

Simple RTOS is a lightweight Real-Time Operating System (RTOS) designed for STM32-Nucleo development boards using the STM32Cube Integrated Development Environment (IDE). This RTOS provides a simple and efficient way to manage concurrent tasks in embedded systems.

## Features

- **Task Management:** Easily create and manage tasks with varying priorities.
- **Synchronization:** Mutex for synchronization between tasks.
- **Time Management:** Time delays and timeouts for efficient task scheduling using a modified round-robin scheduler.
- **Event Handling:** Events to facilitate communication between tasks.
- **Low Overhead:** Designed for resource-constrained embedded systems.

## Getting Started

### Prerequisites

- STM32Cube IDE installed.
- STM32-Nucleo board (compatible with STM32Cube IDE).
- USB cable for board connection.

### Installation

1. Clone this repository to your local machine:

   ```bash
   git clone https://github.com/your-username/SimpleRTOS-STM32.git
