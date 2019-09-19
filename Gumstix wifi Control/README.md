# Embedded-System-
Project for Embedded System Development 

Using the actual Gumstix verdex xm4 with Bluetooth mother board with consoleLCD-vx and netmicroSD-vx expansion boards (instead of the the software emulator). 

Written a standalone kernel module to control the GPIO (general-purpose input/output) pins of the XScale processor. Programmed some pins as inputs (i.e., to read from buttons), and some other pins as outputs (to control LEDs).

i. Using Kermit or Ymodem over the serial connection (using minicom) 
ii. TFTP over the network