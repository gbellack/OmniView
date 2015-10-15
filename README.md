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
