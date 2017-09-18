TI ARM C/C++ CODE GENERATION TOOLS
Release Notes
5.1.3  
December 2013  

Features
===============================================================================
1) Linker Generated ECC
1.1) Linker Command File Syntax
1.2) Error Injection
1.3) VFILL Specifier
2) New C language features
2.1) Hexadecimal floating-point numbers
2.2) Binary literals
2.3) Arrays of variable length
3) IEEE-754 Conformance
4) Silicon Exception in Burst Write to External Memory

===============================================================================
1) Linker Generated ECC
===============================================================================

Beginning with this release, the TI ARM C/C++ Compiler supports automatic
generation of Error Correction Codes compatible with the Flash ECC on various
TI microcontrollers.

ECC can be generated during the final link step of compilation. The ECC data
is included in the resulting object file, alongside code and data, as a data 
section located at the appropriate address. Therefore, no extra ECC generation
step is required after compilation, and the ECC can be uploaded to the device
along with everything else.

The Linker is configured to generate ECC using a new syntax in the linker
command file. The command file will specify a separate memory range for the
ECC inside the device's memory map, and it will indicate which other memory
range corresponds to the Flash data memory covered by the ECC. It will also
specify the parameters by which the ECC will be calculated, which can vary
somewhat between devices.

-------------------------------------------------------------------------------
1.1) Linker Command File Syntax
-------------------------------------------------------------------------------

The memory map for a device supporting Flash ECC may look something like this:

   MEMORY {
       VECTORS  : origin=0x00000000 length=0x000020 
       FLASH0   : origin=0x00000020 length=0x17FFE0
       FLASH1   : origin=0x00180000 length=0x180000
       STACKS   : origin=0x08000000 length=0x000500
       RAM      : origin=0x08000500 length=0x03FB00
       ECC_VEC  : origin=0xf0400000 length=0x000004 ECC={ input_range=VECTORS }
       ECC_FLA0 : origin=0xf0400004 length=0x02FFFC ECC={ input_range=FLASH0  }
       ECC_FLA1 : origin=0xf0430000 length=0x030000 ECC={ input_range=FLASH1  }
   }

The "ECC" specifier attached to the ECC memory ranges indicates the data memory
range that the ECC range covers. The ECC specifier supports the following
parameters:

    input_range=<memory range>   The data memory range covered by the ECC
                                 range; required.
    input_page =<page number>    The page number of the input range; required
                                 only if the input range's name is ambiguous.
    algorithm  =<ECC algo. name> The name of an ECC algorithm defined later in
                                 the command file; optional if only one
                                 algorithm is defined. (see below)
    fill       =<true/false>     Whether to generate ECC data for holes in the
                                 initialized data of the input range; default
                                 value is "true". Using fill=false will
                                 produce behavior similar to the nowECC tool.

In addition to specifying the ECC memory ranges in the memory map, the command
file must specify parameters for generating the ECC data. This is done with a
new top level directive in the command file:

   ECC {
       algo_name : address_mask = 0x003ffff8
                   hamming_mask = FMC
                   parity_mask  = 0xfc
                   mirroring    = F021
   }

This ECC algorithms directive accepts any number of definitions, which consist
of an identifier, a colon, and some number of algorithm attributes. The
following attributes are supported:

    address_mask=<32-bit mask>  This mask determines what address bits are used
                                in the calculation of ECC.
    hamming_mask=<FMC/R4>       This determines which data bits each ECC bit
                                encodes the parity for.
    parity_mask =<8-bit mask>   This determines which ECC bits encode even
                                parity, and which bits encode odd parity.
    mirroring   =<F021/F035>    This determines what pattern of duplication of
                                parity information is used in the ECC memory
                                for redundancy.

Each TI device supporting Flash ECC will have exactly one valid set of values
for these parameters.

The memory maps and ECC algorithm definitions for Flash ECC devices should be
included in subsequent releases of Code Composer Studio. Users will not
generally need to modify the preset values.

-------------------------------------------------------------------------------
1.2) Error Injection
-------------------------------------------------------------------------------

To enable users to test ECC error detection and handling, the compiler now
supports two new command line options that inject bit errors into the linked
executable. The compiler allows the user to specify an address where an error
will appear and a bitmask that specifies which bits in the code/data at that
address should be flipped. The address of the error can be specified
absolutely, or as an offset from a symbol. When a data error is injected, the
ECC for the data is calculated as if the error were not present. This is the
syntax for the error injection options:

    --ecc:data_error=(symbol+offset|address)[,page],bitmask
    --ecc:ecc_error=(symbol+offset|address)[,page],bitmask

The data_error version will inject errors into the load image at the specified
location. The ecc_error version will inject errors into the ECC that
corresponds to the specified location. Note that the ecc_error version can
therefore only specify locations inside ECC input ranges, whereas the
data_error version can also specify errors in the ECC output memory ranges.

For example, the following invocation of the compiler will flip the least
significant bit in the byte at the address 0x100, making it inconsistent with
the ECC for that byte:

    armcl test.c --ecc:data_error=0x100,0x01 -z -o test.out

This invocation will flip two bits in the third byte of the code for main():

    armcl test.c --ecc:data_error=main+2,0x42 -z -o test.out

This invocation will flip every bit in the ECC byte that contains the parity
information for the byte at 0x200:

    armcl test.c --ecc:ecc_error=0x200,0xff -z -o test.out

The compiler will disallow injecting errors into memory ranges that are
neither an ECC range nor the input range for an ECC range. The compiler can
only inject errors into initialized sections.

-------------------------------------------------------------------------------
1.3) VFILL Specifier
-------------------------------------------------------------------------------

A small extension has been made to the fill value specifier for memory ranges
in the linker command file. Normally, specifying a fill value for a memory
range will create initialized data sections to cover any previously 
uninitialized areas of memory. To generate ECC for an entire memory range, the
linker either needs to have initialized data for the entire range, or needs to
know what value uninitialized memory areas will have at run time. To
accommodate the case where the user wants to generate ECC for the entire memory
range, but does not want to initialize the entire range by specifying a fill
value, the new "vfill" specifier may be used instead of a "fill" specifier:

   MEMORY {
       FLASH : origin=0x0000  length=0x4000  vfill=0xffffffff
   }
   
The vfill specifier is functionally equivalent to omitting a fill specifier,
except that it allows ECC generation to occur for areas of the input memory 
range that remain uninitialized. This has the benefit of reducing the size of
the resulting object file.

The "vfill" specifier has no effect other than in ECC generation. It cannot be
specified along with a "fill" specifier, since that would introduce ambiguity.

===============================================================================
2) New C language features
===============================================================================

Beginning with this release, the TI ARM C/C++ Compiler supports the following
C language features.

-------------------------------------------------------------------------------
2.1) Hexadecimal floating-point numbers
-------------------------------------------------------------------------------

ISO C99 added support for floating-point numbers written in a hexadecimal
format. These numbers take the form of a floating point value followed by
a power of 2 to multiply by.

For example:

   1.55e1  = 15.5

   0x1.fp3 = (1 15/16) * 8 = 15.492

In the second example, the 0x1.f represents the base value, and the p3 means
"multiply by 2 to the power of 3".

Refer to section 6.4.4.2 in the C99 Standard for more information on this
feature.

Hexadecimal floating-point numbers are enabled as an extension in all modes,
but standard functions that work on floating point numbers such as atof and
strtod, as well as the %a and %A formatting specifiers for the printf family
of functions do not support hexadecimal floating-point numbers.

-------------------------------------------------------------------------------
2.2) Binary literals
-------------------------------------------------------------------------------

As an extension to C89 and C99, integer literals can be represented as a binary
number prefixed by '0b', much like the hexadecimal '0x'. Binary literals follow
all the same rules as other constants and can be suffixed by modifiers such as
'L'.

The following 4 constants are equivalent:
   
   42
   0x2a
   052
   0b101010

This feature is enabled as an extension when the --gcc option is used.

-------------------------------------------------------------------------------
2.3) Arrays of variable length
-------------------------------------------------------------------------------

ISO C99 added support for arrays whose lengths are not constant. Storage for
these arrays is allocated at the point of declaration and is automatically
deallocated when the array goes out of scope. As such, these arrays cannot be
used in the file scope.

For example, the array 'str' in the following function has a length that is
determined at runtime. Its storage will be allocated during runtime and be
deallocated automatically when it goes out of scope.
      
   FILE *concat_fopen(char *s1, char *s2, char *mode)
   {
      char str[strlen (s1) + strlen (s2) + 1];
      strcpy (str, s1);
      strcat (str, s2);
      return fopen (str, mode);
   }

Exiting a scope containing a variable length array will deallocate it, but
jumping into a scope that contains a variable length array from a scope which
does not contain that array is an error. The following is an example of code
which will result in this error.
   
   void count_by_ten(unsigned int val)
   {
      if (val % 10 == 0 && val != 0)
      {
         int cnt_arr[val/10];
         int i;
   print_it:
         for (i = 0; i < val/10; i++)
            cnt_arr[i] = 10*(i+1);
         for (i = 0; i < val/10; i++)
            printf("%u\n",cnt_arr[i]);
      }
      else
      {
         val = 10;
         goto print_it;
      }
   }

These arrays can also be used as arguments to functions. The lengths of the
array must be declared before the variable length array itself. Consider the
arguments in foo below. The second argument is a variable length array using
the argument 'len' as its dimensions.
   
   void foo(int len, char data[len][len])
   {
       /* ... */
   }

Refer to section 6.7.5.2 in the C99 spec for more information on variable
length arrays.

This feature is currently only enabled when using the --gcc option.

===============================================================================
3) IEEE-754 Conformance
===============================================================================

The TI ARM C/C++ Compiler supports 32-bit and 64-bit format binary floating 
point numbers conformant to that part of IEEE-754 (ISO/IEC/IEEE 60559) 
available in C89 features, with the following caveats: 

- The --fp_mode=strict option must be specified
- The --fp_reassoc=off option must be specified
- Only one NaN, qNaN (quiet NaN) is supported
- Only "round to nearest/even" mode is supported
- The compiler might not preserve the "literal meaning" of some 
  constant expressions which should raise a run-time FPE.  In 
  particular, "inexact" is not supported for constant expressions. 

The TI ARM C/C++ Compiler supports IEEE semantics for C operations and 
functions. Library functions such as sin and cos might not round perfectly, 
handle all special values, or raise all exceptions as specified in IEEE-754.
However, this does not impact IEEE-754 conformance.

The following are not currently supported:
- C99 float rounding mode manipulation features
- C99 FPE manipulation features

Note that neither the C89 nor C99 standard requires IEEE-754 conformance.

===============================================================================
4) Silicon Exception in Burst Write to External Memory
===============================================================================

To work around the silicon exception in burst write to external memory, you can use ?--no_stm? option. It will translate a single STMXXX family instruction to a sequence of STR or VSTR instructions.

This option will also direct all calls to memset/memcpy to a different version (no_stm_memset/no_stm_memcpy) in the RTS library.

The option is a hidden option at this time and is not available for configuration using CCS. It needs to be manually added to the compilation script/command.

