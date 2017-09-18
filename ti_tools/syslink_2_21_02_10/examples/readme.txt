The examples have intentionally been separated into their own stand-alone .zip
files and build independent of the SysLink makefiles.

Examples can be extracted by using the top-level Syslink goal or by issuing the
unzip command.  Remember to protect wildcard characters from the shell 
(unzip must see the wildcard).

Some examples:

1. Unpack all device/platform specific examples into the current folder.

% make extract


2. From the platform specific archive folder, unzip all examples to a
destination folder.

% cd archive/TI816X_*
% unzip \*.zip -d /home/examples

Also see readme.txt in the examples/archive directory for more information
regarding the examples provided with the product.
