# How To #

Kinect uses IR light to get distance info. So first of all you must check that your surface, with the rear projection film attached, can let the infrared light through. You can do this by simply starting the "2D Viewer and Camera Settings", point the kinect at your surface (best to point it from where you will be using it after) and move the Image-Depth slider to Depth. If your surface lets the IR light through you will see through the surface. Most foils we have tested don't let IR light through, we've only got transparent results from transparent film.

Next is the calibration step. If the surface is transparent then you will not get points to calibrate. In order to get points put a sheet of paper (doesn't matter if it doesn't cover the whole surface) over your surface. Select the valid zone over the sheet of paper and perform the plane calculation (you have to select the plane method first). After the plane has been calculated you can reselect the valid zone to cover the hole surface.

Now we have to select the valid distances. As you will be interacting from the other side of the surface (in most cases the interaction is on the same side as the Kinect) the Min and Max Distance parameters have to be negative values. Move them while someone puts their fingers in the surface until you get good finger detection. We recommend you to start from values: Min=-300 Max=-100.

After you have a good calibration you can close the 2D settings window. Now follow the general instructions in HowToMultitouchSurface .

Important : LKB send TUIO data, in order to use it as mouse or Windows multitouch you will have to use or install other apps (listed in this project's home). We will include Windows 8 multitouch events as well as mouse cursor manipulation (ideally for OSX, Linux and Win) inside LKB in the next version...