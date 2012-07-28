#!/bin/sh
#################################################################
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
#################################################################

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

link() {
    DIR=$1
    shift
    mkdir $DIR
    FILES=""
    for F in $*; do
	FILES=$FILES" "`readlink -f $F`
    done
    ln -s  -t $DIR $FILES 
}
##############
## Post-process generated instances


##############
## 1 Criteria
# obj-01 | -lex[-pserv]
# obj-18 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]]]

##############
## 2 Criteria
# obj-02 | -lex[-pserv,+agregate[-local[stage,1-][5000],-bandw[stage,1-]]]
# obj-19 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],+agregate[-local[stage,1-][5000],-bandw[stage,1-]]]
link sum_bandw obj-02 obj-19 
# obj-06 | -lex[-pserv,-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]]]
# obj-23 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]]]
link dist_pserv obj-06 obj-23 

##############
## 4 Criteria
# obj-10 | -lex[-pserv,-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]
# obj-27 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]
link lexicographic obj-10 obj-27 
# obj-14 | -lex[-pserv,-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]]
# obj-31 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]]
link leximax obj-14 obj-31 


##############
## 5 Criteria
# obj-04 | -lex[-pserv,+agregate[-local[stage,1-][5000],-bandw[stage,1-]],-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]
# obj-05 | -lex[-pserv,+agregate[-local[stage,1-][5000],-bandw[stage,1-]],-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]]
# obj-08 | -lex[-pserv,-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]],-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]
# obj-09 | -lex[-pserv,-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]],-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]]
#######
# obj-21 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],+agregate[-local[stage,1-][5000],-bandw[stage,1-]],-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]
# obj-22 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],+agregate[-local[stage,1-][5000],-bandw[stage,1-]],-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]]
# obj-25 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]],-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]
# obj-26 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]],-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]]]
link perf_rel obj-04 obj-05 obj-08 obj-09 obj-21 obj-22 obj-25 obj-26 

# obj-11 | -lex[-pserv,-pserv[reliable,0],-pserv[type,1],-pserv[type,4],+agregate[-local[stage,1-][5000],-bandw[stage,1-]]]
# obj-12 | -lex[-pserv,-pserv[reliable,0],-pserv[type,1],-pserv[type,4],-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]]]
# obj-15 | -lex[-pserv,-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]],+agregate[-local[stage,1-][5000],-bandw[stage,1-]]]
# obj-16 | -lex[-pserv,-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]],-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]]]
#######
# obj-28 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-pserv[reliable,0],-pserv[type,1],-pserv[type,4],+agregate[-local[stage,1-][5000],-bandw[stage,1-]]]
# obj-29 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-pserv[reliable,0],-pserv[type,1],-pserv[type,4],-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]]]
# obj-32 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]],+agregate[-local[stage,1-][5000],-bandw[stage,1-]]]
# obj-33 | -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],-leximax[-pserv[reliable,0],-pserv[type,1],-pserv[type,4]],-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]]]
link rel_perf obj-11 obj-12 obj-15 obj-16 obj-28 obj-29 obj-32 obj-33 

##############
## Primary Criteria
link pserv obj-02 obj-06 obj-10 obj-14 obj-04 obj-05 obj-08 obj-09 obj-11 obj-12 obj-15 obj-16 
link cost_pserv obj-19 obj-23 obj-27 obj-31 obj-21 obj-22 obj-25 obj-26 obj-28 obj-29 obj-32 obj-33 



 
