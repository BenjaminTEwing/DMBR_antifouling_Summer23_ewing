README.txt
This folder contains the series of "master" programs and is the progression of programing in attempt to build.

ONCE A FILE IS UPLOADED TO THIS FOLDER IT SHALL NOT BE EDITED
If you wish the edit the program, download it, edit, and upload with the next number following the Master_VNUMBER naming scheme.

If would also help documentation if when you do so, you summarize the edits made/features added from the V (NUMBER - 1) iteration.  

V1-V6: documentation not available

V7:The last program written as a group.

V8: Program rewritten from scratch.  Composed of the intergration of a variety of test programs to validate motor control, writing to SD Card, and pressure sensing.  This version of the program is essentially non-functional as it concerns the expiriment.  This program functions by operating the motor in the FORWARD direction when LIMIT_SWITCH_1 is engaged and REVERSE when LIMIT_SWITCH_2 is engaged.  In all other cases the motor will not run.

V9:THIS PROGRAM IS STILL IN TESTING PHASE. This program now iterates data file based on what is read in from the SD card to ensure that no data is overwritten by the arduino. 
It then writes in a preamble to the csv file it creates giving details if expirimental parameters set in the CONSTANTS section.  
After finishing the preamble the program goes into loop where it drives forward (using the wipe_forward function) until LIMIT_SWITCH_2 is activated.  After LS2, the program pauses, then bumps the motor back in the opposite direction, this will be useful to disengage the wiper from the limit switch.  Then pause for 5 seconds, then operate in reverse via wipe_reverse function.  This function also performs the same end wipe operation as previous function to  disengage limit switch.  
Finally once the wipe reverse is complete the program is thrown into a while loop to prevent further action until the power is cycled.


V10A:
This Version is now CONTROL TEST READY.  THIS PROGRAM ONLY RECORDS PRESSURE OVER TIME.  All motor function is disabled and it is suggested that the serial monitor by checked while begining the test to ensure that the sd card intializes as there is no other cue on the board.

V10B: This Version is now WIPE TEST READY. THIS PROGRAM RECORDS PRESSURE AT SET INTERVAL AND PERFORMS WIPE FUNCTION ON MULTIPLE OF RECORD DATA.  NOTE - please check that limit switches are in their proper positions before testing.  - Mounts for limit switches may still need to be designed and made.  
This program does NOT include: Automated pump control or flow sensing. 
Flow measurements must be taken by hand.

Began inplementation of RTC but this is incomplete and unusable as of latest push. 