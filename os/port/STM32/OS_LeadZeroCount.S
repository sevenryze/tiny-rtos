
@********************************************************************************************************
@
@                                            CPU PORT FILE
@
@                                            ARM-Cortex-M3
@                                            GNU C Compiler
@
@ Filename      :
@ Version       : V0.0.0
@ Programmer(s) :
@********************************************************************************************************


@********************************************************************************************************
@                                           PUBLIC FUNCTIONS
@********************************************************************************************************

.global  CPU_count_lead_zeros
.global  CPU_count_trail_zeros
.global  CPU_reverse_bits


@********************************************************************************************************
@                                      CODE GENERATION DIRECTIVES
@********************************************************************************************************

.text
.align 2
.thumb
.syntax unified

@********************************************************************************************************
@                                        CPU_count_lead_zeros()
@                                        COUNT LEADING ZEROS
@
@ Description : Counts the number of contiguous, most-significant, leading zero bits before the 
@                   first binary one bit in a data value.
@
@ Prototype   : OS_TYPE_Priority  	CPU_count_lead_zeros(OS_TYPE_CPUDataWidth  val);
@
@ Argument(s) : val         	Data value to count leading zero bits.
@
@ Return(s)   : Number of contiguous, most-significant, leading zero bits in 'val'.
@
@ Caller(s)   : Application.
@
@               This function is an INTERNAL CPU module function but MAY be called by application 
@               function(s).
@
@ Note(s)     : (1) (a) Supports 32-bit data value size as configured by 'OS_TYPE_Priority_TABLE_UNIT'.
@
@                   (b) For 32-bit values :
@
@                             b31  b30  b29  ...  b04  b03  b02  b01  b00    # Leading Zeros
@                             ---  ---  ---       ---  ---  ---  ---  ---    ---------------
@                              1    x    x         x    x    x    x    x            0
@                              0    1    x         x    x    x    x    x            1
@                              0    0    1         x    x    x    x    x            2
@                              :    :    :         :    :    :    :    :            :
@                              :    :    :         :    :    :    :    :            :
@                              0    0    0         1    x    x    x    x           27
@                              0    0    0         0    1    x    x    x           28
@                              0    0    0         0    0    1    x    x           29
@                              0    0    0         0    0    0    1    x           30
@                              0    0    0         0    0    0    0    1           31
@                              0    0    0         0    0    0    0    0           32
@
@********************************************************************************************************
.thumb_func
CPU_count_lead_zeros:
        CLZ     R0, R0                          @ Count leading zeros
        BX      LR


@********************************************************************************************************
@                                       	CPU_count_trail_zeros()
@                                        	COUNT TRAILING ZEROS
@
@ Description : Counts the number of contiguous, least-significant, trailing zero bits before the 
@                   first binary one bit in a data value.
@
@ Prototype   : OS_TYPE_Priority  	CPU_count_trail_zeros( OS_TYPE_CPUDataWidth  val );
@
@ Argument(s) : val         	Data value to count trailing zero bits.
@
@ Return(s)   : Number of contiguous, least-significant, trailing zero bits in 'val'.
@
@ Caller(s)   : Application.
@
@               This function is an INTERNAL CPU module function but MAY be called by application 
@               function(s).
@
@ Note(s)     : (1) (a) Supports 32-bit data value size as configured by 'OS_TYPE_Priority'.
@
@                   (b) For 32-bit values :
@
@                             b31  b30  b29  b28  b27  ...  b02  b01  b00    # Trailing Zeros
@                             ---  ---  ---  ---  ---       ---  ---  ---    ----------------
@                              x    x    x    x    x         x    x    1            0
@                              x    x    x    x    x         x    1    0            1
@                              x    x    x    x    x         1    0    0            2
@                              :    :    :    :    :         :    :    :            :
@                              :    :    :    :    :         :    :    :            :
@                              x    x    x    x    1         0    0    0           27
@                              x    x    x    1    0         0    0    0           28
@                              x    x    1    0    0         0    0    0           29
@                              x    1    0    0    0         0    0    0           30
@                              1    0    0    0    0         0    0    0           31
@                              0    0    0    0    0         0    0    0           32
@
@********************************************************************************************************
.thumb_func
CPU_count_trail_zeros:
        RBIT    R0, R0                          @ Reverse bits
        CLZ     R0, R0                          @ Count leading zeros
        BX      LR


@********************************************************************************************************
@                                            CPU_reverse_bits()
@                                            REVERSE BITS
@
@ Description : Reverses the bits in a data value.
@
@ Prototypes  : OS_TYPE_Priority  	CPU_reverse_bits(OS_TYPE_Priority  val);
@
@ Argument(s) : val         	Data value to reverse bits.
@
@ Return(s)   : Value with all bits in 'val' reversed (see Note #1).
@
@ Caller(s)   : Application.
@
@ Note(s)     : (1) The final, reversed data value for 'val' is such that :
@
@                       'val's final bit  0       =  'val's original bit  N
@                       'val's final bit  1       =  'val's original bit (N - 1)
@                       'val's final bit  2       =  'val's original bit (N - 2)
@
@                               ...                           ...
@
@                       'val's final bit (N - 2)  =  'val's original bit  2
@                       'val's final bit (N - 1)  =  'val's original bit  1
@                       'val's final bit  N       =  'val's original bit  0
@********************************************************************************************************
.thumb_func
CPU_reverse_bits:
        RBIT    R0, R0                          @ Reverse bits
        BX      LR


@********************************************************************************************************
@                                     FILE END
@********************************************************************************************************

.end

