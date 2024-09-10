# ***Project Katuma***

- [Project History](#bookproject-history)
- [Primary Challenge](#heavy_check_markMain-Challenge)
- [Secondary Challenge](#heavy_check_markSecondary-Challenge)
- [Requirements](#boomWhat-the-design-must-do)
- [Limitations](#warningWhat-the-design-cannot-do)
- [Desired Features](#telescopeWhat-the-design-should-do)
- [Wishful Features](#telescopeWhat-I-would-enjoy-the-design-to-do-in-the-future)
- [Project Folders](#pencilProject-Folders)

![IMG_0815](https://github.com/user-attachments/assets/472342f3-6e41-4d11-b8a9-c19fa0a832b9)

*Katumajärvi syksyllä (marraskuu)*




## :book:Project history

I came across this project as part of a HAMK class collaboration between ICT/Bioeconomy and Sustainability Development programs. This concept was fascinating to me, 
and was linked to [Lake Katuma](https://katumajarvi.fi/jarvibarometri-uusi-digitaalinen-palvelu-katumajarvelle/) - also [here](https://www.jarviwiki.fi/wiki/Katumaj%C3%A4rvi_(35.236.1.001)/Katumaj%C3%A4rvi-info).
For 3 months, I was paired with two of my Finnish friends from the non-technical program - and we began!

Since this project has a real-world impact and was a concrete way for me to develop my skills further in IoT (which is part of the reason for studying at HAMK), I have pursued it. So, part of this is my experience in 
learning microcontroller platforms (whereas my work history always dealt with the IT System Admin side) and how to implement them. I had never studied C++ before this project, and I'm not an electrical engineer, but I am willing
to experiment and follow ideas to reality. I will include design ideas, prototypes, and concepts our team initially considered, including good and bad. This is about iterative design, and how it can be used to create something extremely applicable.

Below is a summary of qualifications that have evolved within the last year. These will naturally change as time progresses. You will find further content on other pages.


### :heavy_check_mark:Main Challenge:
- Find a way to help detect eutrophication using real-time parameter data, specifically for Katuma Lake.

### :heavy_check_mark:Secondary Challenge
- Develop this system so it could be used elsewhere in Finland.

  #### :boom:What the design must do

  - Be "real-time"
  - Be submerged
  - Be attached to a buoy or other fixed surface
  - Have an antennae in place to attain proper network signal
  - Measure pH
  - Measure Total Dissolved Solids
  - Measure Temperature
  - Measure direct and indirect parameters
  - Be accurate
  - Be precise
  - Account for bio-fouling and use preventative measures to protect sensor probes/ends
  - Maintain adequate power for the duration of deployment
  - Be deployed for at minimum 30 days
  - Implement deep-sleep functions to conserve power
  - Be water-tight

  #### :warning:What the design cannot do
  
  - Cannot sink
  - Cannot allow water to ingress housing
  - Cannot lose power while submerged
  - Cannot allow bio-fouling to interfere with the sensor measurements
  - Cannot allow end-users to damage the device while it is in operation (submerged)
  - Cannot be an unreasonable cost to produce

  #### :telescope:What the design should do

  - Allow deployment for 90 days
  - Remain about 1 m. in depth
  - Measure turbidity
  - Deliver data to a cloud platform usable to the end-users
  - Provide error handling and debug options
  - Provide reporting or notification mechanisms to the operators to determine equipment health
  - Include interior environment sensor to detect excessive moisture
  - Follow practices consistent with rapid prototyping, modularity, and additive manufacturing
  - Be reasonable in cost
  - If attached to a buoy, be anchored, to eliminate ballast considerations

  #### :telescope:What I would enjoy the design to do in the future

  - Use machine vision to enhance the concept of measuring turbidity
  - Implement machine learning to predict based on the small changes between photos
  - This could be a separate physical unit

### :pencil:Project Folders

- [Fritzing Images](https://github.com/psword/arduino-ide/tree/365733f2cc70094ce8d8d4dbf585f9bfd65574c7/Water%20Quality%20Measuring%20Project/Fritzing%20Images/version%201)
  This folder contains snapshots of the wiring for the project versions. As of now, version 1 is populated, and I am working on version 2.

- [Logical Diagrams](https://github.com/psword/arduino-ide/tree/365733f2cc70094ce8d8d4dbf585f9bfd65574c7/Water%20Quality%20Measuring%20Project/Logical%20Diagrams)
  This folder contains logical diagrams showing information flow. As of now, version 1 is populated, and I am working on version 2.

- [Physical Prototypes](https://github.com/psword/arduino-ide/tree/365733f2cc70094ce8d8d4dbf585f9bfd65574c7/Water%20Quality%20Measuring%20Project/Physical%20Prototypes)
  The physical prototypes folder contains images of how the housing has developed over time. It emphasizes the process of ideation.

- [version 1](https://github.com/psword/arduino-ide/tree/365733f2cc70094ce8d8d4dbf585f9bfd65574c7/Water%20Quality%20Measuring%20Project/version%201)
  This was completed based on required assignments in my degree program module. The emphasis for this version was on learning.

- [version 2](https://github.com/psword/arduino-ide/tree/365733f2cc70094ce8d8d4dbf585f9bfd65574c7/Water%20Quality%20Measuring%20Project/version%202)
  This version is in progress, and will develop into code that will support a working, deployable prototype.
  
--------------------------------------------------------------------------------------------
![IMG_9026](https://github.com/user-attachments/assets/60e6362b-5982-4703-bb23-79ff29d4b4cb)
