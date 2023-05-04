# PICO DEV M3

![20221123_113742_1](https://user-images.githubusercontent.com/44074914/203675239-e666f68e-3007-4da6-83f4-6276f0353171.jpg)


The **Pico Dev M3** is a development board based on Raspberry Pi's **RP2040 microcontrolle**r chip, the board is also a high-resolution **magnetic rotary encoder** when paired with a few 3D printed parts, which makes this the perfect board for applications and projects that can benefit from having a rotary encoder, a good example is making macro pads, and other similar secondary controllers.

## **Board Features**
- RP2040 chip, Dual-core Arm Cortex-M0+ processor at 133 MHz
- 264kB on-chip SRAM
- 128Mb on-board QSPI flash
- 19 multifunction GPIO pins, with 2 analog inputs
- 2 UARTs
- 2 SPI controllers
- 2 I2C controllers
- 16 PWM channels
- USB 1.1 controller with host and device support
- 8 PIO state machines
- USB type C
- Lipo charging
- onboard Button input
- Addressable Led ring (18 LEDs)

## **Encoder Features**

- The onboard encoder features a base resolution of **36 steps, extending up to 36000** steps per revolution.
- There’s also a ring of **18 individually addressable LEDs** on board.
- Features a ball bearing which gives the encoder a significantly smoother feel compared to traditional rotary encoders.
- Customizable 3d printed encoder. 
- Implementing the encoder is also really straightforward using the dedicated Libraries and Sample Codes
- Easy mounting options

### **Programming Language:**

- **RP2040 C/C++ SDK** 
- **Micropython**

![pico dev m3 pinout](https://user-images.githubusercontent.com/44074914/210262617-3780ce25-4f13-4e42-bd19-e370b2e2f230.jpg)

### **Sample Designs Based on the PICO DEV M3:**

![ZVE00216 (3)](https://user-images.githubusercontent.com/44074914/236347845-b50baea9-03aa-4949-9894-49093bf09b79.JPG)

- **Pico Dev M3 Keypad** 

![ZVE00234 (2)](https://user-images.githubusercontent.com/44074914/236347506-14e3ddcd-3a1c-400d-a589-a3466eb049b3.JPG)

This is a 12-key macropod that uses the Pico dev m3 as its controller, the breakout board features backlit keys, headers for connecting I2C, SPI, and UART peripherals, and a header for connecting a joystick module to the Pico dev m3, you can power the keypad from the type c port on the pico dev m3 board or with a Lipo battery connected to the BAT header, taking advantage of the Lipo charge controller built into the pico dev m3.
- **PIO Stepper Controller**

![ZVE00231 (2)](https://user-images.githubusercontent.com/44074914/236347603-74c1dc2a-cb5c-43a8-9ec1-ef19d36aa6cb.JPG)

This breakout board allows you to use this Programmable IO (PIO) on the RP2040 chip on Pico dev m3 to individually control 3 stepper motors. This board can be used to control any 3-axis motion system, robot, or custom mechanism.

## BUILD INSTRUCTIONS
 - https://www.ahmsvillelabs.io/magnetic-rotary-encoderbuild/pico-dev-m3

### **Encoder Assembly:**

- Sandwich the pico dev m3 board between the **BOARD BUTTON** and **BOARD TOP** 3D printed parts, and secure them together using 3 **M1.4x6mm** self-tapping screws.

![Capture](https://user-images.githubusercontent.com/44074914/209444073-41d47224-8d91-4deb-9c0e-4a108ad13419.PNG)

![CaptureXCXC](https://user-images.githubusercontent.com/44074914/209444082-f9c58404-2e73-40fc-9cee-b183f6893c2f.PNG)

- Attach **608 bearing (8x22x7)** to the assembly and secure it with the **BEARING HOLDER** part and 1 **M2x8mm** self-tapping screw.

![CapturZDCe](https://user-images.githubusercontent.com/44074914/209444088-0e20827a-11f1-461a-a567-2288d2ee2eeb.PNG)

- Install **(18pcs) 3x1mm** neodymium magnets to the KNOB in an alternating pole configuration.
Depending on your 3D printer's tolerance you may use super glue to secure the magnets, but if the holes turn out as the perfect size, a simple press-fit is sufficient.

![CaptuGGre](https://user-images.githubusercontent.com/44074914/209444094-9ce0751e-0a48-4a63-bfd8-c88e86bd2d27.PNG)

- Attach the knob to the bearing.

![CaptFHNFure](https://user-images.githubusercontent.com/44074914/209444101-c25a82e6-112c-44c5-989e-ac1bcc06dd92.PNG)

- For Flexibility, you may print the **BUTTON BASE** file this allows the pico dev m3 encoder and button to be used conviniently, the button base attaches to the pico dev encoder via 2 **M2x8mm** selftapping screws.

![Capgfgture](https://user-images.githubusercontent.com/44074914/209444385-965c41aa-b64a-484c-81ef-a5a061b4b158.PNG)


