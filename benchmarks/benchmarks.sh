#!/bin/sh
###################################################
#     Copyright (C) 2011 Arnaud Malapert.
#
#     This program is free software: you can redistribute it and/or modify                    
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://ww.gnu.org/licenses/>.
###################################################

PROG=`basename $0 .sh`
version() {

    cat <<EOF  
$PROG 0.1
Copyright (C) 2011 Arnaud Malapert.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.
This  is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the extent permitted by law.
     
Written by A. Malapert."
EOF
}

help() {
cat <<EOF
$PROG does nothing interesting except serve as an example for help2man.
     
Usage: $PROG [OPTION]...
     
Options:
  -a, --option      an option
  -b, --another-option[=VALUE]
                         another option
     
  --help        display this help and exit
  --version     output version information and exit
     
Examples:
  foo               do nothing
  foo --option      the same thing, giving --option
     
Report bugs to <arnaud (dot) malapert (at) unice (dot) fr>."
EOF
}

#--------------------------------------------------------------------
# Setup Global Variables
#--------------------------------------------------------------------

N=20

EXEC=`readlink -f ../bin/Release/opossum-0.1`
OBJ_ALIASES="./objectives.sh"
source $OBJ_ALIASES ;
OBJECTIVES="-lex[-pserv] -lex[$cost_pserv] -lex[-local]]  -lex[$sum_bandw]"
#--------------------------------------------------------------------
# Test for prerequisites
#--------------------------------------------------------------------

if [ $# -eq 0 ] || [ $1 = "--help" ]; then
    help
    exit 0
elif [ $1 = "--version" ]; then
    version
    exit 0
fi


#--------------------------------------------------------------------
# Do something
#--------------------------------------------------------------------


for GEN in $*; do
    if [ -r $GEN ]; then
	GEN=`readlink -f $GEN`
     O=1
     dirname=`basename $GEN .dat`
     mkdir $dirname;
     echo "Entering in $dirname ..."
     for OBJ in $OBJECTIVES; do
	 echo "$OBJECTIVES"
	 for ((I=1; I <= N ; I++)) # Double parenthèses, et "N" sans "$".
	 do
	     name=`printf '%s/%s-%02d-%02d.sh\n'  $dirname $dirname $O $I`
	     echo "#!/bin/sh" > $name
	     echo "$EXEC -v1 -s$I  -i $GEN  $OBJ" >> $name
	     chmod +x $name
	 done
	 O=`expr $O + 1` 
     done
    else
	echo "$GEN is not a readable file"
	exit -1;
    fi
done



