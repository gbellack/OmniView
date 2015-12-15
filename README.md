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
1. omniview/main.c : This is where the main program lies. All things related to scheduling is done here.

2. omniview/mode.c: This file contains the two modes that the facial recognition is capable of doing, as well as module initialization functions. All modules (display, camera etc.) are included and used here. Primary mode basically takes an image every second and returns the name of the person in the image.

3. omniview/pinmux.c/h : This is where we specify which pins are being used for certain protocols by the program.

4. omniview/modules folder: This stores all the modules/component libraries. More on that later...

5. omniview/modules/display/display.c/h: This file implements the driver for the display. It provides a good interface for the OLED display that we are using.

6. omniview/modules/display/buffer.c: This file contains the display buffer. The reason why we have this in a separate file is because the display had initialization values that we wanted to keep, and by adding this huge buffer definition in our display function, it will make that file really big and hard to read. Hence, we decided to split it into a separate file and link it up.

7. omniview/modules/display/font.c: This file contains the font settings for text on our OLED display. For the same reason above, we decided to move it to a separate file.

8. omniview/modules/interrupts/button_interrupt.c/h: These files contain interfaces for triggering a GPIO interrupt via one of our buttons. Some of the macros are linked to other macros defined by Texas Instruments.

9. omniview/modules/interrupts/timer_interrupt.c/h: These files contain interfaces for triggering a timer interrupt. This is mainly used by the microphone interface for audio recording purposes during secondary mode.

10. omniview/modules/microphone/microphone.c/h: These files contain the interfaces for the microphone.

11. omniview/modules/networking/tcp_network.c/h: These files contain the TCP networking interface required to send data/files across the network (or in our case, interact with our server).

12. omniview/modules/camera/mt9d111.c/h: These files contain the driver for the camera model that we are currently using.

13. omniview/modules/face_software/faceRec.cpp: This file contains the main server loop as well as several functions that help it in running.

14. omniview/modules/face_software/tcpClient.c/h: Houses the server side networking code used to communicate (by TCP) with the embedded device.

15. omniview/modules/face_software/wav_transcribe.py: Called by the main facial recognition loop. Communicates with google’s speech to text recognition server. Sends a string to parseName and receives names back. Sends names back to facial recognition main.

16. omniview/modules/face_software/parseName.py: Uses a natural language processing toolkit to process the string. Returns the names identified by the algorithm to wav_transcribe.

17. omniview/modules/face_software/training: Database holding our folders of faces. Test pic gets compared to this database to find the most similar face. A folder is created for each unique person in our dataset. The folder, named after the person, houses the pictures of the person inside.
Eg. omniview/modules/face_software/training/john_connolly/john1.jpg  is the first picture of John Connolly in the database.

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
