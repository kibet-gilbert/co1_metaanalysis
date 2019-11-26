                           ARLECORE (v 3.5.2)
                          ==========
                          
ARLECORE is the console version of Arlequin"

Usage:
------
                          
arlecore <project file name (*.arp)> | <settings file (*.ars)>
          | run_silent\n

The three arguments are optional. If no argument is provided
arlecore will look for the file arl_run.txt where the name of a
project file should be indicated. If it does not find the project
file defined in arl_run.txt, the present message is shown.

The argument <project file name> needs to be put within quotes if 
it contains whitepaces

The argument <settings file (*.ars)> allows one to use
a specific setting file for the computations. 
If no settings file is specified, it will use the default setting file
arl_run.ars.

run_silent: if present on the command line, arlecore will not output
            computation progress in the console

Examples:
  arlecore test.arp test.ars
     This will analyse the file 'test.arp' using the settings defined in the
     file 'test.ars'.

  arlecore
     This will read the file 'arl_run.txt' to see which project(s) need(s) to
     be analysed, and which setting file to use (arl_run.ars) or <project name>.ars.

What is needed to run arlecore:
------------------------------
- arlecore executable file under windows or linux
- a settings file (e.g. arl_run.ars) created with the windows version of arlequin (see below)

What is needed to properly view the html or xml result files:
- All the gif, js, xsl, and R files provided in the archive file arlecore*.zip



Different flavors of arlecore:
------------------------------
We provide these different versions of arlecore:
- arlecore64.exe    : 64 bit windows executable
- arlecore32.exe    : 32 bit windows executable
- arlecore352_64bit : 64 bit Linux version
- arlecoremac_64bit : 64 bit MacOS version


How to set up the setting file arl_run.ars:
-------------------------------------------
- Launch the Windows version or Arlequin
- Open a project file of the type you want to analyse
- Choose the computations you want to perform in the settings tab
- Press on the Save button above the Arlequin settings pane. Choose a place and a 
  name (arl_run.ars is the default name) for the settings you have just selected.
- If you close the project without explicitly saving your settings, they will be 
  saved by default in the file arl_run.ars.
  These settings file contain all necessary information about which computations 
  to perform and which parameters to use for those computations.
- Copy this file in the directory of the arlecore executable file, 
  or else you will need to specify its full path when invoking arlecore (see above)
- Note that the same settings file can be used under both windows and linux.



Bash script to launch arlecore
------------------------------
We provide with arlecore a basic bash script called LaunchArlecore.sh, 
allowing one to run arlecore on all the files present in a given directory.
It assumes that this directory also contains arlecore, as well as a setting file.
This bash script will run on Linux by just typing the command
  ./LaunchArlecore.sh
and on windows by typing the command
  bash LaunchArlecore.sh
assuming you have installed the cygwin package (available on http://www.cygwin.com/), or the 
msys package (http://www.mingw.org/wiki/msys) and that the 
cygwin/bin or MinGW\msys\1.0\bin directory is on your path environment variable.
Note that you'll need to edit LaunchArlecore.sh (with any text editor - not Word) to specify:
- the version of arlecore you are using
- the name of the settings file you want to use



Bash script LaunchArecore.sh
-------------------------------
   #!/bin/bash
   
   #Laurent Excoffier November 2009
   #
   #The script will launch arlecore on all arlequin project files in turn
   #It assumes that it is launched in a directory containing:
   #         - a series of *.arp files to be analysed
   #         - a settings file containing the settings specifying which 
   #           computations are to be performed (usually obtained through the WinArl35.exe
   #           graphical interface).
   
   #Modify the following line to state which version of arlsumstat you are using
   arlecore=arlecore35_64bit
   #Modify this if you wan to use another setting file
   settingsFile=arl_run.ars
   
   if [ -f $settingsFile ]; then
   
      #This loop will analyse all files with the same settings file
      for file in *.arp
      do     
         echo "Processing file $file"
         #Launch arlecore with the same settings for all files
         ./$arlecore  $file $settingsFile   run_silent
      done
   
      #The following loop would be an alternative to perform specific computations on each file
      #assuming that there is a different setting file with the same name as the project file, but with 
      #the extension *.ars 
   
      #for file in *.arp
      #do     
      #   echo "Processing file $file"
      #   #Launch arlecore with project specific settings
      #   ./$arlecore  $file ${file%.*}.ars run_silent
      #done
   else 
      echo "Settings file $settingsFile does not exist. Aborting script."
   fi



Running a single instance of arlecore on the command line
-----------------------------------------------------------

Examples under windows:

The following command will analyse the file test.arp 
using the default arl_run.ars setting file

  ./arlecore64.exe test.arp
  
The following command will analyse the file test.arp 
using the setting file test.ars

  ./arlecore_32bit test.arp test.ars
  
  This will read the file 'arl_run.txt' to see which project(s) need(s) to
  be analysed, and which setting file to use (arl_run.ars) or <project name>.ars
  
  ./arlecore64.exe
  
This will read the file 'arl_run.txt' to see which project(s) need(s) to
be analysed, and  the settings file test.ars will be used for these analyses
(assuming that files will be analyses using arlequin interface settings
i.e. keyword 'use_interf_settings' presnet on the 2nd line of arl_run.txt)

  ./arlecore64.exe test.ars


-----
Laurent Excoffier, April 2015
Email: laurent.excoffier@iee.unibe.ch