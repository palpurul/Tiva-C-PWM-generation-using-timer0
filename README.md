# Tiva-C-PWM-generation-using-timer0

PWM signal generated and outputed through the pin PF1 which is connected to the red LED. If you run the program on TM4C123GXL board you will see the red LED dimming. Timer0 interrupt is used for PWM generation. Each time interrupt occurs interrupt handler is switched for transition from high-to-low or low-to-high. FPU is enabled for calculation regard to PWM's period and duty cycle

The project was compiled using keil uVision 4, and following library from Texas Instruments' website used

Link to the library: http://software-dl.ti.com/tiva-c/SW-TM4C/latest/index_FDS.html

Links to useful documents:

http://www.ti.com/lit/ug/spmu298a/spmu298a.pdf (library refrence)

http://www.ti.com/general/docs/lit/getliterature.tsp?genericPartNumber=tm4c1230h6pm&fileType=pdf (datasheet)
