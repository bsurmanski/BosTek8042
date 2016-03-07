# Instructions

Instructions are called using their mnemonic, plus some arguments.

    ADD B

Any instruction that requires an address can substitute a label

    JMP MYLABEL

# Numbers

Numbers can be represented in hexadecimal, decimal, or ASCII.

## Hexadecimal

Hexadecimal numbers must be prefixed by '$' (exception .DAT directive)

    JMP $FF00

If a single hexadecimal number has an odd number of nibbles, it will be right
justified (filled on the left with zeros). Note: this is in contrast to the .DAT
directive which is left justified. The following are equivalent:  

    JMP $F
    JMP $0F
    JMP $000F

## Decimal

Decimal numbers need no prefix, or may be prefixed by '#'

    ADD 55
    ADD #55

## ASCII

ASCII represented numbers require a prefixed single quote

    ADD 'A

ASCII characters may also have an optional closing single quote.
The following are equivalent:

    SUB '0
    SUB '0'

## Negative

If a number is prefixed with a '-', it will be converted to a twos compliment
negative number.

    ADD -1   ; equivalent to ADD $FF
    ADD -$0A ; valid, eqivalent to ADD $F5
    ADD -'0' ; valid, but may not be useful

# Comments

Anything following a semi-colon on a line is ignored

    JMP LABEL ; this comment is ignored
    ADD B     ; this is also ignored

# Labels

    .LAB <LABEL NAME>

Represent 16-bit address at label site. Registers keyword, can be used in jump
commands. Label names can be at most 8 characters

Labels can also be short-handed with a leading colon:

    :<LABEL NAME>

Any label that begins with an underscore ('\_') is considered 'local'.
The purpose of local labels is to avoid clashing with multiple macro
expansions.

# Directives
    
    .ORG <ADDRESS>

Set the origin of the program

    .SET SYM <VALUE>

The set directive creates a value alias. This directive does not take up any
memory in the target program. The value must be a numeric value (See 'Numbers').
The value will be inserted into it's usage, and must fit the context.

    .SET VAL1 123
    .SET VAL1 123
    .SET VAL2 $FFFF
    ADD VAL1 ; OK
    ADD VAL2 ; 

## Data Directives

Data directives are often used with labels.

### Byte Directive

    .BYT <V1> <V2> <V3> ...

Inserts the specified bytes into the program. See the section on 'Numbers' to
see what value formats are valid. If any value is greater than 255 ($FF), there
is an error. A byte string may contain a variable alias:

    .SET XYZ 123
    .BYT XYZ XYZ 255

### Word Directive

    .WOR <V1> <V2> <V3> ...

Inserts the specified words (2 bytes) into the program. Word values are
little-endian.

### String Directive

    .STR "<STRING>

Inserts an ASCII string into the program. The string may have an optional
closing quote.

    .STR "This is a string"

### Data Directive

    .DAT DEADBEEF1337

Insert a string of bytes, encoded as hexadecimal. This value may have a prefixed
'$'. Any spaces between bytes are ignored. All three of the following are
equivalent:

    .DAT $B1 AA

    .DAT B1AA

    .DAT B 1AA

All data is byte aligned. If a .DAT definition has an odd number nibbles,
it is padded with a zero nibble:

    .DAT 32 1

Is equivalent to:

    .DAT 32 10

Commas will end the current byte. If only the high nibble has been defined, then
the byte will be padded with a zero nibble. The following are equivalent:

    .DAT F,F,A
    .DAT F0F0A0

All other characters found in the data that are not hexadecimal are an error.
Data definitions cannot contain aliases or variables.

    .DAT FF 'X' ; ERROR, unexpected character \'
    .DAT #123   ; ERROR, unexpected character #
    SET XYZ $FF
    .DAT XYZ    ; ERROR, unexpected character X

# Macro Directives

## Defining a Macro

    .MAC <NAME> <ARG1> <ARG2> <ARG3> ...

Define the beginning of a macro. A macro can have at most 8 arguments. The macro
name and the argument names can be at most 8 characters.

    .MND

End the macro definition

## Calling a Macro

A macro is called similarly to an instruction:

    MYMACRO ARG1 ARG2 ARG3 ...

There can be optional commas anywhere there are spaces.

    ADDALL A, B, C

Or even:

    ADDALL,A,B,C

## Labels in Jump

JMP LABEL
