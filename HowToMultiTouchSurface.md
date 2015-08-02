# Multitouch Surface Methods #

## Basic Setup ##

> - Connect a Kinect camera before LKB starts.

> - Click on the "2d Viewer and touch settings"->"Camera x" button to begin camera x configuration.

The main idea is to calculate distances from something (tip of a finger for example) to a known surface.

In order to calibrate a surface we have 2 different methods:

  * Background Method

  * Plane Method

Before each method it is recommended that you perform the "Select Valid Zone" procedure (after clicking you will see the instructions on the lower left)

### Background Method ###

  * Select Background in method

  * Move the Image - Depth slider so you can get an idea of what you are receiving.

  * Click on "Calibrate Background" Button and wait for the process to finish.

  * You will be able to see the background and mask in the two lower viewers

  * Adjust the setting to get good blobs. In order to do this you can :

> - Adjust Min and Max distance until you get good blobs (put a finger in the surface and move min and max distances until you see almost nothing else but your finger). The distances are roughly in mm.

> - Use the "Dilate" - > "Erode" - > "Dilate" operations

> - Reduce the number of ghost blobs by clicking in "Start Refining Mask" (after be sure to "Stop Refining Mask")

  * Adjust the min Blobs size (this is roughly the 2d area of the blobs)

> If you get good blobs we are done and you can close the Settings Dialog.

### Plane Method ###

  * Select Plane in method

  * You must select a valid zone. Click on "select valid zone" and follow the instructions.

  * Move the Image - Depth slider so you can get an idea of what you are receiving.

  * Click on "Calculate Plane" Button and wait for the process to finish.

  * You will get a Dialog with an error and some parameters. The error depends a lot on the distance from the Kinect to the surface. If close an error below 1.5 is good. Else (as a touch floor) an error of 6.0 in good.

  * You will be able to see the mask in the lower viewer.

  * Adjust the setting to get good blobs. In order to do this you can :

> - Adjust Min and Max distance until you get good blobs (put a finger in the surface and move min and max distances until you see almost nothing else but your finger). The distances are roughly in mm.

> - Use the "Dilate" - > "Erode" - > "Dilate" operations

> - Reduce the number of ghost blobs by clicking in "Start Refining Mask" (after be sure to "Stop Refining Mask")

  * Adjust the min Blobs size (this is roughly the 2d area of the blobs)

> If you get good blobs we are done and you can close the Settings Dialog.

## What Next ##

If you are using just one camera click on "Touch Extractor and TUIO Sender"

Should be self explanatory.

If you are using two cameras:

  * Follow the same procedure for both cameras (also be sure that both cameras share a zone - Left in one - Right in the other.) From now on it is assumed that both cameras are pointing at a surface that if being projected with the computer image you are using.

  * After performing the basic Setup for both cameras and closing the Settings Dialogs click on "2-Kinect Base Touch Calibration" (you can also use a chessboard to perform calibration, see HowToMulti3dViewer wiki for reference)

  * A fullscreen black window will appear where, after 2 seconds, some concentric circles will appear and a number in the upper left corner will hopefully increase (if it doesn't check that both cameras share the zone where at least some circles appear). You can end this calibration by clicking your mouse at any time. It automatically closes at 300 but it is recommended to close it at about 120.

  * You will get a RANSAC dialog with some parameters and an error. A good error is close to 1.0 .

  * When done (close to one, or not...) both cameras will have a calibration and you can now continue.

  * You can check how well is the calibration by clicking on "2-Kinect Blob Viewer" and watching the results. Close it after (or else you will loose frame rate)

  * When done click on "Touch Extractor and TUIO Sender" - Should be self explanatory.