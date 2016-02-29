# BosTek8042 family micro architecture. 

8842 adds additional general purpose registers
80142 adds 16-bit operations and 6 new 16-bit only registers.
80143 adds 4 16-bit floating point registers.

# Registers

15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
|- -- -- PCH-- -- -- --|-- -- -- -- PCL-- -- -| PC
|- -- -- SPH-- -- -- --|-- -- -- -- SPL-- -- -| SP

|-- -- --FLAG- -- -- --|-- -- -- -- Acc-- -- -| FA
|- -- -- H- -- -- -- --|-- -- -- -- L- -- -- -| R0  [16] indirect

                    [8842]
|- -- -- B- -- -- -- --|-- -- -- -- C- -- -- -| R1  [16]
|- -- -- D- -- -- -- --|-- -- -- -- E- -- -- -| R2  [16]

                    [80142]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R3  [16]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R4  [16]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R5  [16]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R6  [16]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| R7  [16]

                    [80143]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| F0  [16]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| F1  [16]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| F2  [16]
|- -- -- -- -- -- -- --|-- -- -- -- -- -- -- -| F3  [16]

Flag Register Format:
S - Sign
Z - Zero
V - Overflow
I - Interrupt Enable
T - Trap flag 
P - Parity
H - Half Carry
C - Carry

# Instruction Formats:
bit format for instructions given inputs. For example, add r,r is an instruction
that takes 2 16-bit registers, therefore it will use the 16RR format.

X - 8-bit no register or constant specified
|o -o -o -o -o -o -o -o|
18
=18
    
R8 - 8-bit single register (often implicit Acc as well)
|o -o -o -o -o -r -r -r|
12
=96

K8 - 8-bit constant only
|o -o -o -o -o -o -o -o|-k -k -k -k -k -k -k -k|
9
=9

K16 - 16-bit constant only
|o -o -o -o -o -o -o -o|-k -k -k -k -k -k -k -k|-k -k -k -k -k -k -k -k|
27
=27

16RR - 2 16-bit registers
|o -o -o -o -o -o -o -o|r1 r1 r1 r1 r2 r2 r2 r2|
8 (16r/16r)
7 (f/f)
2 (16r/f)
=17

16RK - register, one 16-bit constant
|o -o -o -o -o -o -o -o|-- -- -- -- -- -r -r -r|-k -k -k -k -k -k -k -k|-k -k -k -k -k -k -k -k|
11 (16r/K)
=11

16FK - float register, 16-bit constant
|o -o -o -o -o -o -o -o|-- -- -- -- -- -- -f -f|-k -k -k -k -k -k -k -k|-k -k -k -k -k -k -k -k|
9
=9

16F - float register only
|o -o -o -o -o -o -o -o|-- -- -- -- -- -- -f -f|
10
=10

# Instruction Map (WIP)
For all 'r'-> replace with: AHLBCDEM or HLBCDEM depending on whether there are 8 or 7 spaces
For all 'f'-> replace with: SZVITPHC

+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|   | x0| x1| x2| x3| x4| x5| x6| x7| x8| x9| xA| xB| xC| xD| xE| xF|
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 0x|NOP|HLT|SFI|CFI|SFT|CFT| ? | ? |EXR|SWr|-> |   |   |   |   |   | nop, set/clear flag, swap
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 1x|LDA|-> |   |   |   |   |   |   |STA|-> |   |   |   |   |   |   |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 2x|TRI|TrA|-> |   |   |   |   |   | ? |TAr|-> |   |   |   |   |   | load, store, transfer: TrA, TAr
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 3x|PLr|-> |   |   |   |   |   |   |PHr|-> |   |   |   |   |   |   | pull, push
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 4x|LrK|SrK|PSH|PUL|LFK|SFK|PSH|PUL|TRR|TrR|TRr|TFF|TRF|TFR|TrF|TFr| 16-bit/float load/store/push/pul/transfer
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 5x|SWr|SWF|   |   |   |   |   |   |   |   |   |   |   |   |   |   | swap rr, swap f
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 6x|JfC|-> |   |   |   |   |   |   |JfS|-> |   |   |   |   |   |   | jump flag set, jump flag clear
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 7x|JMP|JMM|JNE|JEQ|JLT|JLE|JGT|JGE|JSR|JSM|   |RET|RFI|IRQ|NMI|WFI| jump, control
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 8x|ADI|ADr|-> |   |   |   |   |ADM|ACI|ACr|-> |   |   |   |   |   | Add (A <- A + R), Add with Carry (A <- A + R + C)
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| 9x|SUI|SUr|-> |   |   |   |   |   |SCI|SCr|-> |   |   |   |   |   | Sub (A <- A - R), Sub with Carry (A <- A - R - C)
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| Ax|CPI|CPr|-> |   |   |   |   |   |ANI|ANr|-> |   |   |   |   |   | Cmp, And (A <- A & R)
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| Bx|ORI|ORr|-> |   |   |   |   |   |XRI|XRr|-> |   |   |   |   |   | Or (A <- A | R), Xor (A <- A ^ R)
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| Cx|ASL|ASR|ROL|ROR|COM|NEG|   |   |ADD|ADC|SUB|SBC|CMP|AND|ORR|XOR| extended 8-bit arith, 16-bit rr arith
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| Dx|ASL|ASR|ROL|ROR|COM|NEG|   |   |ADD|ADC|SUB|SBC|CMP|AND|ORR|XOR| r arith, rk arith
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| Ex|MUL|MUK|DIV|DIK|MOD|MOK|   |   |ADD|MUL|SUB|DIV|CMP|MOD|   |   | rr mul/div, float ff
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
| Fx|INV|SRT|IRT|SIN|COS|TAN|ABS|NEG|ADD|MUL|SUB|DIV|CMP|MOD|POW|TRP| float f, float constant, trap
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+

# 8-bit mode instructions

## Arithmetic
All 8-bit arithmetic operations implicitly store to Acc and have Acc as their first operator
All 8-bit arithmetic operations that read from memory (end with M) will read byte from address at [H,L]

### Simple arithmetic
8 ADD ADC SUB SBC ORR XOR CMP

ADD R ; Add reg to Acc
ADC R ; Add reg with carry to Acc
SUB R ; Sub reg from Acc
SBC R ; Sub reg with carry from Acc
AND R ; and reg with Acc
ORR R ; Or reg with Acc
XOR R ; Xor reg with Acc
CMP R ; compare A with register

### Extended Arithmetic
5 NEG ASL ASR ROL ROR

ASL ; logical shift left Acc
ASR ; logical shift right Acc
ROL ; Rotate left Acc
ROR ; Rotate right Acc
NEG ; negate Acc

### Constant arithmetic

ADD k ; add register A with 8-bit constant
ADC k
SUB k
SBC k
AND k
ORR k
XOR k
CMP k ; compare register A with 8-bit constant

## Load Store

### 1 byte format
4
SFI ; Set flag I
SFT ; Set flag T
CFI ; Clear flag I
CFT ; Clear flag T

4
SWP R ; swap reg with Acc
PSH R ; push reg to stack
PUL R ; pull reg from stack (pop)
TRA R ; transfer register into Acc

### Constant Load
TRA k ; load immediate byte into Acc (8 bit instruction + 8 bit immediate)

### Memory

STM K ; store Acc to address k (see also TAM for HL addressing)
LDM K ; load Acc from address k (see also TMA for HL addressing)

## Jump

BDS ; sets CPU on fire????

(3-byte instructions)

k may be an integer constant -128 <= k <= 127, or k may be a label
if k is a label, the label must be defined within -128 to 127 bytes 
from the calling instruction

JbS K ; jump relative if flag bit set
    JSS JVS JZS JIS JTS JPS JHS JCS

KbC K ; branch relative if flag bit cleared 
    JSC JVC JZC JIC JTC JPC JHC JCC

JMP K ; jump to label or address
JMP M ; Jump to memory pointed to by HL. Copy address at HL to PC, effectively jumping to address

JNE K ; branch relative if compare results not equal
JEQ K ; branch relative if compare results equal
JLT K ; branch relative if compare results less-than
JLE K ; branch relative if compare results less than or equal
JGT K ; branch relative if compare results greater than
JGE K ; branch relative if compare results greater than or equal


JSR K ; call subroutine at k. k can be a label or an address
JSR M ; call subroutine pointed to by HL


## Control

NOP ; no operation
WFI ; wait for interrupt
IRQ ; software interrupt request (with value of Acc)
NMI ; non-maskable software interrupt request (with value of Acc)
HLT ; halt
RFI ; return from interrupt load SP from stack and re-enable interrupt flag
RET ; return from subroutine. load PC from stack


#16-bit mode instructions:

*Arithmetic*
All 16-bit arithmetic operators store to the first operand
Example ADD r1, r2 = r1 <- r1 + r2

## 1 byte format

TPC ; transfer PC to HL

## 2 byte format

TRA d,R ; copy 8-bit register to bottom half of 16-bit register (sign extended?)
TRA R,d ; copy lower bits of 16-bit register to 8-bit register

TRA d,s ; copy contents of register s to register d

ADD d,r ; add 
ADC d,r ; add with carry
SUB d,r ; subtract
SBC d,r ; subtract with carry
XOR d,r ; exclusive or
AND d,r ; and
ORR d,r ; inclusive or registers
CMP r,r ; compare registers

ASL r ; arithmetic shift left
ASR r ; arithmetic shift right
ROL r ; rotate left
ROR r ; rotate right
NEG r ; negate register (2's compliment)
COM r ; compliment d (1's compliment)

PSH r ; push word
PUL r ; pull word

*Load Store*
SWP r,r ; swap registers

STM K,r ; store register to word at address k
LDM r,K ; load word at address k into register
STM M,r ; store register to word at (HL)
LDM r,M ; load word at (HL) to register
TRA r,K ; load 16 bit immediate into r

ADD d,K ; add 
ADC d,K ; add with carry
SUB d,K ; subtract
SBC d,K ; subtract with carry
XOR d,K ; exclusive or
AND d,K ; and
ORR d,K ; inclusive or registers
CMP r,K ; compare registers


# extended instructions
EXR - exchange shadow registers. Does not affect SP or PC
EXF - exchange float shadow registers. Does not affect SP or PC

# float (16) instructions

TRA f, r ; transfer register r (16-bit) to float register f
TRA r, f ; transfer float register to int register
TRA f, f ; transfer between float registers
TRA f, R ; transfer 8-bit register to float
TRA R, f ; transfer float to 8-bit register

ADD f, f ; f1=f1+f2
MUL f, f ; f1=f1 * f2
DIV f, f ; f1=f1/f2
SUB f, f ; f1=f1-f2
CMP f, f ; compare float registers
SWP f, f ; swap float registers
MOD f, f ; f1=f1%f2
POW f, f ; f1 ^ f2

LDM f, K ; load from addr k
STM f, K ; store to addr k
TRA f, K ; load f with constant k

MUL f, K ; f=f * K
DIV f, K ; f=f/K
ADD f, K ; f=f+K
SUB f, K ; f=f-K
CMP f, K ; compare with constant K
SWP f, K ; swap with memory at (K)

PUL f
PSH f

INV f ; f=1.0/f
SRT f ; f=sqrt(f)
IRT f ; f=1.0/sqrt(f)
SIN f ; f=sin(f)
COS f ; f=cos(f)
TAN f ; f=tan(f)
ABS f ; f=abs(f)
NEG f ; f=-f
