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
     
Usage: $PROG [OPTION] FILE... 
     
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
CRIT1="-pserv $cost_pserv"
CRIT2="$sum_bandw $dist_pserv $bad_pserv -leximax[$bad_pserv]"

PRINT=0
#--------------------------------------------------------------------
# Test for prerequisites
#--------------------------------------------------------------------

if [ $# -eq 0 ] || [ $1 = "--help" ]; then
    help
    exit 0
elif [ $1 = "--version" ]; then
    version
    exit 0
elif [ $1 = "--print" ]; then
    shift
    PRINT=1
fi


#--------------------------------------------------------------------
# Do something
#--------------------------------------------------------------------
##Read generator's paths
for file in $*; do
    if [ -r $file ]; then
	GENERATORS="$GENERATORS "`readlink -f $file`
    else
	echo "$file is not a readable file"
	exit -1;
    fi
done

##Generate objectives
for C1 in $CRIT1; do
    OBJECTIVES=$OBJECTIVES" -lex[$C1]"
    for C2 in $CRIT2; do
	OBJECTIVES=$OBJECTIVES" -lex[$C1,$C2]"
	for C3 in $CRIT2; do
	    if [ "$C2" != "$C3" ]; then
		OBJECTIVES=$OBJECTIVES" -lex[$C1,$C2,$C3]"
	    fi
	done
    done
done

##Generate shell scripts
I=1
for O in $OBJECTIVES; do
    dirname=`printf 'obj-%02d' $I`
    echo "$dirname | $O"
    if [ $PRINT -eq 0 ] ; then
	mkdir $dirname
	for G in $GENERATORS; do
	    gname=`basename $G .dat`
	    for ((J=1; J <= N ; J++)) # Double parenthèses, et "N" sans "$".
	    do
		name=`printf '%s/%s-%02d.sh\n'  $dirname $gname $J`
	    ##echo $name
		echo "#!/bin/sh" > $name
		echo "$EXEC -v1 -s$J  -i $G  $O" >> $name
		chmod +x $name
	    done
	done
    fi
    I=`expr $I + 1` 
done    




