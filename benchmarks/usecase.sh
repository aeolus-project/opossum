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
$PROG executes a usecase for the criteria combination features of the opossum solver for PSL problems.
     
Usage: $PROG [OPTION]...
     
Options:
  --help        display this help and exit
  --version     output version information and exit
     
Example:
  $PROG               execute the usecase.
     
Report bugs to <arnaud (dot) malapert (at) unice (dot) fr>."
EOF
}

#--------------------------------------------------------------------
# Setup Global Variables
#--------------------------------------------------------------------

EXEC="../bin/Release/opossum-0.1"
OBJ_DEF="./objectives.sh"
GEN="instances/genM.dat"
SEED="201"


#--------------------------------------------------------------------
# Test for prerequisites
#--------------------------------------------------------------------

if [ $# -ne 0 ]; then
    if [ $1 = "--help" ]; then
	help
	exit 0
    elif [ $1 = "--version" ]; then
	version
	exit 0
    fi
fi

#--------------------------------------------------------------------
# Do something
#--------------------------------------------------------------------


 if [ -x $EXEC ]; then
     if source $OBJ_DEF ;  then
	 I=1
	 for OBJ in \
	     "-lex[-pserv]" \
	     "-lex[-pserv,$capa_pserv]" \
	     "-lex[-pserv,$bad_pserv]" \
	     "-lex[-pserv,-leximax[$bad_pserv]]" \
	     "-lex[$cost_pserv]" \
	     "-lex[$cost_pserv,$bad_pserv]" \
	     "-lex[$cost_pserv,-leximax[$bad_pserv]]" \
	     "-lex[$cost_pserv,$sum_bandw]" \
	     "-lex[$cost_pserv,$dist_pserv]" 
	 do
	     usecase=usecase-$I
	     echo "Processing use case $I ..."
	     cmd="$EXEC -v2 -s$SEED -i $GEN  $OBJ"
	     echo $cmd
	     $cmd >  $usecase".log"
	     mkdir $usecase
	     mv sol-* cplexpb* $usecase
             ##echo rename cplexpb cplexpb$I cplexpb*;
	     ##echo rename sol- sol$I"-" sol-*;
	     I=`expr $I + 1` 
	 done
	 exit 0
     fi
 fi
 echo "An error has occured"
exit -1
 



