#
#  Copyright 2013 by Texas Instruments Incorporated.
#  
#

#!/bin/sh
#
unset INCLUDE
unset LIB
unset CL

dir=${1%/*}
cmdbin=/cygdrive/${dir%%:*}${dir#?:}
export PATH=$cmdbin:$cmdbin/../../common/msdev98/bin

"$@"
#
#  @(#) microsoft.targets; 1, 0, 2, 0,630; 12-19-2013 15:12:17; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

#

