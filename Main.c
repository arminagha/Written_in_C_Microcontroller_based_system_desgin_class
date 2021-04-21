#include "msp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "keypad.h"
#include "LCD.h"

#ifndef CPU_FREQ
#define CPU_FREQ 3000000
#define __delay_us(t_us) (__delay_cycles((((uint64_t)t_us)*CPU_FREQ) / 1000000))
#endif

#define RGB_LED 0x07
char key_char[];

int password[] = {1,2,3,4};
int userpassword[] = {0,0,0,0};
int i;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;   // Stop watchdog timer
    uint8_t keypressed;

    init_LCD();

 typedef enum                                      // Define states
  {
 WAIT,                                             // welcome screen
 LOCK,                                             // Lock screen type pins
 CHECK,                                            // Check pins
 UNLOCK,                                           // Unlock state
 NEWPIN                                            // Enter new pins state
  }
 state_type;

 state_type state = WAIT;                          // Initialize the locked state

while(1)
{
   switch(state)                                   // Start of switch statement
   {
       case WAIT:                                  // Think as the welcome screen
       {
           lcd_clear();                            // Clear the welcome Screen

           P2->OUT &= ~(RGB_LED);                  // Resets led
           P2->OUT |= (1 & (RGB_LED));             // Set RGB LED to output red

           lcd_place_cursor( 0x00 );               // Set cursor at top line

           lcd_write_str("LOCKED");                // write LOCKED on top line

           lcd_place_cursor( 0x40 );               // Set cursor next line

           lcd_write_str("ENTER KEY:");            // Write  Enter key on screen

           state = LOCK;                           // set next state to lock
           break;
       }

       case LOCK:
       {

          for (i=0; i<4; i++)                      // For loop to input four pins
          {
              keypressed = readkey();

              while(keypressed > 12)               // Test if a Button is pressed
              {
                  keypressed = readkey();          // Searching for Button pressed
              }

              if (keypressed == 10)                // If * is pressed reset pin
              {
                  state = WAIT;
                  __delay_us(1000000);             // Delay 1 second
                  break;                           // leave loop
              }



              char key_char = keypressed + 0x30;   // Change keypressed into char
              lcd_write(key_char);                 // Write character on screen
              userpassword[i] = keypressed;        // Save input as userpassword

              while(keypressed < 12)               // Test if Button is released
              {
              keypressed = readkey();              // Searching for Button released
              }
              __delay_us(1000000);                 // Delay 1 sec so doesn't flash
              state = CHECK;
          }                                        // End of for loop
          break;                                   // Break out of case

       }                                           // End of case lock

       case CHECK:                                 // Case Check password
       {

           if(userpassword[0] == password[0] &&    // Check if userinput match password
                   userpassword[1] == password[1] &&
                   userpassword[2] == password[2] &&
                   userpassword[3] == password[3])
           {                                       // Check each pin individually
              state = UNLOCK;                      // If correct Password entered
              break;                               // Send to UNLOCK state
           }
           else                                    // if any pins don't match its wrong
           {

              lcd_place_cursor( 0x00 );            // Set cursor to top line

              lcd_write_str("STILL lOCK");         // Write STILL LOCKED on screen

              __delay_us(2000000);                 // Wait 2.0 seconds to display message

              state = WAIT;
              break;
           }

      }                                            // end of case check

       case UNLOCK:
       {
           lcd_clear();                            // Clear LCD screen

           P2->OUT &= ~(RGB_LED);                  // Resets RGB LED
           P2->OUT |= (2 & (RGB_LED));             // Set RGB LED to output red

           lcd_place_cursor( 0x00 );               // Set cursor at top line

           lcd_write_str("UNLOCK, PRESS");         // Write Unlock, press on screen

           lcd_place_cursor( 0x40 );               // Set cursor to bottom line

           lcd_write_str("#-LOCK,*-NEW PIN");      // Display message

           while(keypressed != 10 &&               // Test if # or * is only pressed
                 keypressed != 12)
           {
               keypressed = readkey();             // Keep Searching Until
           }                                       // # or * is only pressed

           if (keypressed == 10)                   // If * is pressed
           {
               state = NEWPIN;                     // Send to New pin State
           }

           else if (keypressed == 12)              // If # is pressed
           {
               state = WAIT;                       // Send to wait state
           }                                       // To Relock box
           __delay_us(1500000);                    // Delay 1.5 seconds
           break;

       }

       case NEWPIN:
       {
           lcd_clear();                            // Clear LCD screen
           lcd_place_cursor(0x00);                 // Set cursor at top line

           lcd_write_str("New Pins State");        // write new pins state

           lcd_place_cursor( 0x40 );               // Set cursor to bottom line

           lcd_write_str("type pins:");            // Display type pins on screen

           __delay_us(1000000);                    // Delay 1.0 seconds

           for (i=0; i<4; i++)                     // For loop to input four pins
           {
               keypressed = readkey();             // Read key pressed

               while(keypressed > 12)              // Test if a Button is pressed
               {
                   keypressed = readkey();         // Searching for Button pressed
               }

               if (keypressed == 10)               // If * is pressed reset pin entered
               {
                   state = NEWPIN;                 // Go back to Newpin state
                   break;
               }

               char key_char = keypressed + 0x30;  // Change keypressed into char
               lcd_write(key_char);                // Write character on screen
               password[i] = keypressed;           // Save input as password

               while(keypressed < 12)              // Test if Button is released
               {
               keypressed = readkey();             // Searching for Button released
               }
               __delay_us(1500000);
               state = WAIT;
           }                                       // End of for loop
           break;                                  // Break back into switch
       }                                           // End of case new pins
    }                                              // End of Switch
   }                                               // End of while loop
}                                                  // End of main
