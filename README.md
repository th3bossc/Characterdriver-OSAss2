# CHARACTER DRIVER - OS Theory Assignment 2

This is the solution to the assignment 2 of OS Theory course (S5).

## Table of contents
- [Overview](#overview)
- [Installation](#installation)
- [Usage](#usage)
- [Author](#author)


## Overview
- This is a character driver created as a part of Operating Ssytems course, in S5
- The driver takes in kernel version and a timer (seconds) as parameter and installs successfully if the inputted kernel version is the same as the installed kernel version, install failes otherwise.
- Upoon removing the driver, if the following operations are done in proper order within the timer given as input, it exits with success message, otherwise returns failure message.
    - Read from the driver from a user process
    - Write the username of the current process to the buffer of the driver

## Installation

1. Clone the repo
```
git clone https://github.com/th3bossc/Characterdriver-OSAss2.git
```

2. Navigate to src directory
```
cd Characterdriver_OSAss2/src/
```

3. Compile the driver
```
make
```

4. Install the driver
```
insmod driver.ko kernel_version=<your_kernel_version> timer=<timer>
e.g. insmod driver.ko kernel_version=6,5,10 timer=1000
```

## Usage

1. Navigate to tests directory
```
cd ../tests
```

2. Compile the read and write files
```
gcc read.c -o read
gcc write.c -o write
```

3. Run the read and write programs
```
./read
./write
```

4. Remove the kernel
```
rmmod driver
```

5. Checking driver stats
```
dmesg
```

Note : if any of the following commands fail to run, use sudo prefix

## Author
- [Diljith P D](https://github.com/th3bossc)