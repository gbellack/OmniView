#OmniView

##Hardware
--------
1. Texas Instruments CC3200 MCU

##Installation
1. You will need Code Composer Studio (CCS) version 6.1.1 installed on your computer. You can download this from TI's website. You will need to enter some credentials and get approved to be able to download the software.
2. You will be prompted to select modules that you want CCS to download. Please install the CC3200 package under SimpleLink Wifi Support tools.
3. After installing CCS, please install the CC3200 SDK from Texas Instruments.
4. If you launch Code Composer Studio for the first time, it will prompt you for a workspace directory. Feel free to pick a directory that you would like to work off.
5. Here comes the fun part. Please install GitHub for Windows. You will need this to pull the CCS project from this repository.  
// TODO

You should be able to build and run the project once all these are set up. 

###Flashing the program via UniFlash

If you would like to flash the program onto the Launchpad, you will need a seaparate program called Uniflash by Texas Instruments. Go ahead and download it from here and continue to follow the steps below. If you would not like to do so, you may skip them.
Credits to: [Texas Instruments Uniflash Wiki page](http://processors.wiki.ti.com/index.php/UniFlash_Quick_Start_Guide)

### Folder/File description
1. main.c : This is where the main program lies. All things related to scheduling is done here.
2. pinmux.c/h : This is where we specify which pins are being used for certain protocols by the program.
3. modules folder: This stores all the modules/component libraries. More on that later...
4. modules/display/display_driver.c/h: This file implements the driver for the display. It provides a good interface for the OLED display that we are using.
5. modules/display/display.c/h: This file contains the freeRTOS display task, as well as the functions related to how the OLED is being used in this application.
6. modules/camera/camera_driver.c/h: This file implements the driver for the camera. It provides a good interface for the camera.
7. modules/camera/camera.c/h: This file contains the freeRTOS camera task, as well as the functions related to how the camera is being used in this application.
8. modules/camera/camera_network.c/h: This file will implement everything to do with how the image is being sent over the network.
9. modules/mcu/mcu.c/h: This file implements power modes for the CC3200MOD microcontroller that we are using.

###Speech Recognition
Please READ:
1. Make sure you have PyAudio installed.
2. All the examples are in the speech_recognition file.

###Version Control for Code Composer Studio Project
The .gitignore file on this repository should do the trick, but please:  
  
DO commit and push:  
1. .ccsproject  
2. .cproject  
3. .project  
4. .settings folder  
5. makefile.defs (if using SYS/BIOS)  

DO NOT commit and push:    
1. Release/*  
2. config/*  
3. .launches/  
4. .xdchelp  

Credits to: [Texas Instruments Wiki page](http://processors.wiki.ti.com/index.php/Source_control_with_CCS)  

##Support
Please feel free to contact any of us if you have any questions or concerns.
