.. _Dublin_Maker_2022:

Dublin Maker Badge 2022 (unofficial)
####################################

Overview
********

This project contains the code for a badge intended for use during
Dublin Maker in the summer of 2022.

Building and Running
********************

This application can be built as follows:

west build -b bbc_microbit_v2 DublinMaker2022 -p
The "-p" (pristine) option is only necessary if you build a different project in the current directory.
To write the output to the NRF52833 type:
west flash

Debug output is sent over the UART at 115200bps



