#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="gcvs"

# remove unwanted 'macros' folder
#   The original path 'macros' is used for cvsgui macros. 
#   It was easier to use an differnt path for gnome macros,
#   than to move all cvsgui macros to a new place
echo "remove unwanted 'macros' folder"
rm -rf $srcdir/macros

echo "link 'am_macros' to 'macros' folder"
ln -s $srcdir/am_macros $srcdir/macros

. $srcdir/macros/autogen.sh
