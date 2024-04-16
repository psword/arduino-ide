# Important Note

#### Huzzah feather (8266)

The code in this folder uses dynamic sizing of the SenseIterations buffer. I tried to load this on the Huzzah Feather, and while it does upload successfully, it crashes when the class is used to create measurements.

The FireBeetle doesn't mind this type of implementation, but I will update the sketch for the Feather and modify how that buffer is created. See the tds_class, which was loaded onto a FireBeetle (8266) without issue.
