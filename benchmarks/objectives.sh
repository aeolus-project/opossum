#!/bin/sh

cost_pserv="-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]]"
capa_pserv="+agregate[-pserv[type,0-1][500],-pserv[type,2][300],-pserv[type,3][100],-pserv[type,4][20]]"
bad_pserv="-pserv[reliable,0],-pserv[type,1],-pserv[type,4]"
dist_pserv="-agregate[-conn[length,1][1],-conn[length,2][2],-conn[length,3][3]]"
sum_bandw="+agregate[-local[stage,1-][5000],-bandw[stage,1-]]"

