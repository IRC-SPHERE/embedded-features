This is a test of feature extraction performance (speed, RAM usage, ROM usage) on ARM Cortex-M3 and M4 cores.

Please cite the following paper if you use this repository:

* A. Elsts, R. McConville, X. Fafoutis, N. Twomey, R. Piechocki, R. Santos-Rodriguez and I. Craddock. On-Board Feature Extraction from Acceleration Data for Activity Recognition, EWSN 2018.

### Features

Importing data:

    cd import
    ./load_sphere_challenge_files.py

This creates a number of `.c` files in the `data` directory, each containing 15000 samples of 3-axis acceleration data. The data is expressed as 8-bit signed integers.

### Running

Running natively (the same architecture as on the machine it is compiled on):

    cd feature-test
    make run ARCHITECTURE=native


Running on an emulator - needs Zephyr OS and Zephyr SDK to be installed:

    cd feature-test
    make run ARCHITECTURE=zephyr


Running on hardware (a SPHERE board) - needs Contiki OS and ARM compiler to be installed:

    cd feature-test
    make run ARCHITECTURE=sphere
    
Other supported targets are: `z1` (for Zolertia Z1 with msp430), `zoul` (for Zolertia Zoul with CC2538) and `nrf52dk` (for Nordic NRF52DK with Cortex-M4F).


### Features

Note: for increased performance, some of the FFT-based feature results are not normalized!
To get the correct result, they should be divided either by `FREQUENCY_FEATURE_WINDOW_SIZE` or by `FREQUENCY_FEATURE_WINDOW_SIZE^2` depending on the feature.


### Generating results for comparison

Go to the top directory and run

    ./generate_features.py

This will generate a number of CSV files in the `export` directory.
Each file is based on 25-minute data from a single participant and includes all of the supported features.

This will run the feature test natively. Prerequisites:

* GNU make
* GCC
* Python3

### Results

* The `export` directory will contain the `.csv` files with the different features after running `generate_features.py`.
* The `result` directory contains the processing duration evaluation results on the different hardware platforms.
