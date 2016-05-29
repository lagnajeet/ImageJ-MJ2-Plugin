# ImageJ-MJ2-Plugin
Open MJ2 file in imagej and FIJI

Installation Instructions for the MJ2 plugin for ImageJ

1.	Copy the jar files inside the folder "jars" into ImageJ's plugin folder
2.	Copy the dll files inside the "dlls" folder to the "bin" folder of the jre that is uused to run ImageJ. 
	If in doubt copy those dll files into "windows/system32"


This is a platform dependent mj2 file decoder using ffmpeg library. This plugin makes use of "HandleExtraFileTypes" plugin. If you have a modified version of "HandleExtraFileTypes" installed for your ImageJ then copy all the other jar files into the plugin directory except "HandleExtraFileTypes.jar" and then add the following line to your custom "HandleExtraFileTypes" file to have mj2 capabilities.

if (name.endsWith(".mj2")) <br>
{<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return tryPlugIn("HandleMJ2Files_", path);<br>
}<br>

The reading speed for this plug-in on an i7-4790 CPU running at 3.6GHz was about 30 to 35 MBPS i.e. it will read a 250MB file in about 7 to 8 seconds.

Lagnajeet Pradhan

