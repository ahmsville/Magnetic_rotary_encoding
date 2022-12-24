# Pico Dev M3 Micropython Setup

The board will come with a default firmware loaded, so you should be able to just connect it to a computer and see the output from a serial terminal like putty or arduino's serial monitor. The default firmware is in C/C++ and you can download the UF2 file [**here.**](https://github.com/ahmsville/Magnetic_rotary_encoding/tree/master/PICO%20DEV%20M3/C-C%2B%2B%20SDK/PICO%20DEV%20M3/build) 

## Step 1

Download the micropython uf2 firmware for the raspberry pi pico [**here**](https://micropython.org/download/rp2-pico/), and copy it into the pico dev m3.

To get the pico dev m3 to show up as a storage device on the computer, connect to the board via the usb type c port while holding the BOOTSEL button at the back of the board, reference the pinout diagram.

## Step 2

Download the python files on this page to a folder on your computer.

## Step 3

Open **MagneticRotaryEncoder.py** in thonny ide, go to file, save copy, thonny will ask for a location to save the file, select Raspberry Pi Pico, save the file on the pico dev m3 with the name **"MagneticRotaryEncoder.py"**

![Capturesds](https://user-images.githubusercontent.com/44074914/209449876-2beeba95-669e-4c17-94fd-d73cc3cc3cc7.PNG)

## Step 4

Open **MagEncoder Example.py** in a new thonny ide window or tab, click on run script, the pico dev m3 led ring should turn on and you will see the encoder's counting value in thonny's terminal/shell as you rotate the knob on the pico dev m3. 

![Capturxfdfe](https://user-images.githubusercontent.com/44074914/209449919-84f3bfc9-86c3-4b74-a000-f64df2c11143.PNG)

## Changing the encoding method used on the board.

Please see the main [Magnetic rotary encoder](https://github.com/ahmsville/Magnetic_rotary_encoding) page for details on the differences between the modes listed below.

To switch modes, simply uncomment one of this lines in the example code

```python
#ENCMODE = 1 # BASE RESOLUTION ANALOG ONLY
#ENCMODE  = 2 # BASE RESOLUTION ANALOG ONLY ABSOLUTE POSITIONING
#ENCMODE  = 3 # EXTENDED RESOLUTION ANALOG ONLY
#ENCMODE  = 4 # BASE RESOLUTION ANALOG AND DIGITAL
#ENCMODE  = 5 # BASE RESOLUTION ANALOG AND DIGITAL ABSOLUTE POSITIONING
#ENCMODE  = 6 # EXTENDED RESOLUTION ANALOG AND DIGITAL
ENCMODE  = 7 # BASE RESOLUTION DIGITAL ONLY
#ENCMODE  = 8 # BASE RESOLUTION DIGITAL ONLY ABSOLUTE POSITIONING
```
