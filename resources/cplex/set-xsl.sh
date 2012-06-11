#!/bin/sh

sed   -i~ '2i<?xml-stylesheet href=\"resources/cplex/solution.xsl\" type=\"text/xsl\"?>' $*
