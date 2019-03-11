#ifndef VM_STD_OUT
#define VM_STD_OUT
#include "stdint.h"

template <typename Char>
static inline void putChar (Char *buf, Char c, int index, int size)
{
    if (index < size) {
        buf[index] = c;
    }
}

namespace std_printer {
template <typename Char, Char EOS, Char NL, Char ZERO, Char SIGN> /*EOS - end of string character*/
	class  Printer {
        private :
            int cursor;
            int textSize;
            Char *text;
            char buffer[16];
            bool cleanUp;

            
            int str_len (Char *str)
            {
                uint32_t i = 0;
                while (str[i++] != EOS) {}
                return i - 1;
            }
            Char *reverseChars (Char *charSequence, int e)
            {
                int s = 0;
                Char c = EOS;
                for (;s < e; s++, e--) {
                    c = charSequence[s];
                    charSequence[s] = charSequence[e];
                    charSequence[e] = c;
                }
                return charSequence;
			}    
			Char *stringifyInt (Char *buffer, int32_t value, int32_t base)
			{
				int i, sign = value;
				if (sign < 0) {              
					value = -value;      
                }                    
				i = 0;
				do {                               
					buffer[i++] = value % base + ZERO; 
					value = value / base;											
				} while (value > 0);       
				if (sign < 0) {
					buffer[i++] = SIGN;
                }
				buffer[i] = EOS;
				return reverseChars(buffer, i - 1);
			}
			char *stringifyUInt (char *buffer, uint32_t value, uint32_t base)
			{
				int i;                  
				i = 0;
				do {                               
					buffer[i++] = value % base + ZERO; 
					value = value / base;
				} while (value > 0);       
				buffer[i] = EOS;
				return reverseChars(buffer, i - 1);
			}

            void print (Char *str, int position)
            {
                this->cursor = position;
                int i = 0;
                while (str[i] != EOS) {
                    putChar(this->text, str[i++], this->cursor++, this->textSize - 1);
                }
            }
        public :
            Printer (int textSize)
            {
                this->cursor = 0;
                if (textSize == 0) {
                    this->textSize = 0;
                    this->text = nullptr;
                    return;
                }
                this->text = new Char[textSize];
                if (this->text == nullptr) {
                    this->textSize = 0;
                } else {
                    this->textSize = textSize;
                    this->clearText();
                    this->cleanUp = true;
                }
            }
            
            Printer (Char *text, int textSize)
            {
                this->cursor = 0;
                this->text = text;
                this->textSize = textSize;
            }
                    
            ~Printer ()
            {
				if (this->cleanUp == true) {
                    delete[] this->text;
                }                    
            }
            
            void clearText ()
            {
                this->cursor = 0;
                if (this->text == nullptr) {
                    return;
                }
                for (int  i = 0; i < this->textSize; i++) {
                    this->text[i] = EOS;
                }
            }
            
            void setText (Char *text)
            {
                this->clearText();
                this->print(text, 0);
            }
            
            void apendText (Char *text)
            {
                this->print(text, this->cursor);
                this->text[this->cursor] = EOS;
            }
					
            void print (Char *str)
			{
				this->print(str, this->cursor);
			}
            
			void printLn (Char *str)
			{
                putChar(this->text, NL, this->cursor++, this->textSize - 1);
				this->print(str, this->cursor);
			}
            
            void printInt (int32_t value)
			{
				this->print ( this->stringifyInt(this->buffer, value, 10), this->cursor );
			}
			void printIntLn (int32_t value)
			{
				this->printLn ( this->stringifyInt(this->buffer, value, 10));
            }
			void printUintLn (uint32_t value)
			{
				this->printLn ( this->stringifyUInt(this->buffer, value, 10) );
			}

            void printHex (int32_t value)
			{
				this->print ( "0x", this->cursor );
				this->print ( this->stringifyUInt(this->buffer, value, 16) ,this->cursor);
			}
			void printLnHex (int32_t value)
			{
				this->printLn ( "0x" );
				this->print ( this->stringifyInt(this->buffer, value, 16) ,this->cursor);
			}
            
            void removeLastChar ()
            {
                if (this->cursor == 0) {
                    return;
                }
                putChar(this->text, EOS, --this->cursor, this->textSize - 1);
            }
            
            void insertChar (int position, Char c)
            {
                if (position > (this->cursor + 1)) {
                    return;
                }
                if (position == this->cursor) {
                    putChar(this->text, c, this->cursor++, this->textSize - 1);
                    return;
                }
                Char _c;
                _c = this->text[position];
                this->text[position] = c;
                position++;
                while (position != this->cursor) {
                    this->text[position] = _c;
                    position++;
                    _c = this->text[position];       
                }
            }
            
            Char *getText ()
            {
                return this->text;
            }
            
            int getTextBufferRemind ()
            {
                return this->textSize - this->cursor;
            }
            
            int getTextSize ()
            {
                return this->textSize;
            }
					
	}; /*class printer*/
	
} /*namespace std_printer*/


#endif


/*End of file*/

