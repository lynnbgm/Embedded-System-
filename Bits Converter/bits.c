#include "bits.h"
#include <limits.h>
#include <stddef.h>
#include <stdbool.h>

unsigned int BinaryMirror(unsigned int value)
{
    //create a new value by bitwise ORing the least significant bit of the original value to the new value, 
    //shifting the new value left and trimming the least significant bit from the old value
    //1U is unsigned 1, CHAR_BIT is equal to number of bits in a char which is generally 8 bits
    unsigned int val = value;
    size_t k = sizeof (value) * CHAR_BIT - 1;
    while ((value >>= 1U) != 0) {
    val = (val << 1U) | (value & 1U);
    --k;
    
  }
  return val << k;
};

unsigned int SequenceCount(unsigned int value)
{
    bool foundOne = false;
    unsigned int foundInstance = 0,i,num=0;
    //do for each bit, shift left and take MSB everytime to get every bit
    for(i=0;i<32;i++){
        num = value&(2147483648); // bitwise AND with 2^31 to get the MSB

        if(num == 0 && foundOne == false){
          //we found "0" or "00" in binary representation
           value=value<<1;
           }
        else if(num != 0 && foundOne == true){
	  //we found "11" in binary representation
            value=value<<1;

        }
        else if(num != 0 && foundOne == false){
	  //we found "1" in binary representation
            foundOne = true;
            value=value<<1;
           }
        else if(num == 0 && foundOne == true){
	  //we found "10" instance in binary representation
            foundInstance++;
            foundOne = false;
            value=value<<1;
            }
    }
   return foundInstance;

};
