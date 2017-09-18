#!/bin/sh
#   ============================================================================
#   @file   runall.sh
#
#   @brief  Script to run the examples
#
#   ============================================================================

# [-p release] [-m mapfile ]

profile=debug
mapfile=""
#base="$(cd "$(dirname "$0")" && pwd)"
base=`pwd`

while [[ $# -gt 0 ]]; do
    if [[ "$1" == "-p" ]]; then
        profile="$2"
        shift 2
    elif [[ "$1" == "-m" ]]; then
        mapfile="-m $2"
        shift 2
    else
        echo "unknown option: $1"
        echo "usage: runall [-p release] [-m mapfile]"
        exit 1
    fi
done

\ls -1 ex*/load*.sh | {
    while read dir; do
        echo "##########################################################"
        echo "# Loading modules in $dir"
        ./$dir
        echo "##########################################################"
        echo ""
    done
}

#find ex* -type f -wholename "*/$profile/*run*" -printf "%h\n" | {
\ls -1 -d ex*/$profile | {
    while read dir; do
        echo "##########################################################"
        echo "# Running example in $dir"
        echo "##########################################################"
        cd $dir
        ./run.sh $mapfile
        echo "##########################################################"
        echo "# Completed example run in $dir"
        echo "##########################################################"
        echo ""
        cd $base
    done
}

\ls -1 ex*/unload*.sh | {
    while read dir; do
        echo "##########################################################"
        echo "# Unloading modules in $dir"
        ./$dir
        echo "##########################################################"
        echo ""
    done
}
