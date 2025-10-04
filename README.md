<h1>PROJECT VIGILENS</h1>

Project VIGILENS is a research project conducted at ISAE Supaero Neuroergonomics lab. It's goal is to develop and algorithm to detect mental and physical fatigue in jet fighter pilots and scuba divers using EOG data (Electro Oculo Graphy) : https://en.wikipedia.org/wiki/Electrooculography

This repository contains the code to run the device that will gather real-time data during experiments and apply different algorithms.





<h3>THE DEVICE</h3>

The device operates in two parts : 

- EOG data gathering (handled by an OpenBCI board) : https://openbci.com/

- Data processing (done on a Raspberry Pi 4)



<h3>OPENBCI BOARD (Ganglion or Cyton)</h3>

The OpenBCI boards are very powerful tools to gather EOG data, they are in fact, built for that purpose. However, they operate in a way which is not advanageous to our intended use.

Firstly, the boards only transmit their data to a computer using the bluetooth protocol. As we intend to build a robust device that can be used in extreme environments, using bluetooth transmition is far from ideal.

Also, the boards await a certain signal that can only be sent by the OpenBCI GUI to start streaming data. We do not have the luxury to click on a GUI interaction to enable the start of the stream.

For these reasons and others not mentioned, we decided to hack the firmware of these devices to modifiy it to our liking.



<h3>FIRMWARE HACK</h3>

As you will find in this repository, the modified firmware uploaded to our boards is not much different from the "factory settings" one. 

To bypass the bluetooth transmission, we redirected the data flow to some of it's unused pins and can now read useful data from it using the UART protocol.

Secondly, we allowed our Raspberry Pi to send the same exact start signal that the GUI would otherwise send, therefore bypassing the two roadblocks we previously had.



<h3>RASPBERRY PI 4</h3>

You will also find in this repository the code that runs on the raspberry pi. 

It has two main functions : 

- Data Intake

- Data Processing

Both of these functionnalities are run on different threads to allow for real-time data acquisition.



<h3>DATA INTAKE</h3>

The data intake thread, acts as a sort of "bodyguard" to the second thread if you will. As most of the data transmitted is baseline signal, it filters out all of useless data and sends only potential events that may be of interest to the data processing thread.

It's role is to interface with the OpenBCI board, to gather data from it, filter out useless data and prepare the useful events for the data processing thread.

This multithreaded approach was adopted to allow for real-time data processing. 

<h3>DATA PROCESSING</h3>

The data processing thread does the heavy lifting. It's role is to operate the heavy calculations and algorithms to identify different events, run fatigue detection algos and maybe run a neural network in the future. 

<h3>THE ALGORITHMS</h3>

\- TO BE WRITTEN -


