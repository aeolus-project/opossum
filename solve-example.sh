#!/bin/sh


bin/Release/opossum -v2 -s5 -i benchmarks/instances/genM.dat  -lex[-agregate[-pserv[type,0][10],-pserv[type,1][18],-pserv[type,2][6],-pserv[type,3][4],-pserv[type,4][1]],+bandw] -t60
