#!/usr/bin/awk --posix -f 
#~|#Facilities~|Demand~|#Pserv~|#Rel. pserv
{
    if (/^c [0-9]+ +FACILITIES +[0-9]+ +PSERVERS +[0-9]+ +CLIENTS/) {
	rtab[0]=$2;
	rtab[1]=$6;
    }  else if (/^d PSERVERS /) {
	rtab[2] = $3;
    } else if (/^d REL_PSERVERS /) {
	rtab[3] = $3;
    }
}
END {
    for (x = 0; x < 4; x++) {
	printf " ~|%s", rtab[x]
    }
}
