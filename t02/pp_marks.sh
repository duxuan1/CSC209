#!/bin/bash

declare -i assignment=0;
declare -i assignment_total=0;
declare -i tutorial=0;
declare -i tutorial_total=0;

for d in a1 a2 a3 a4; do
    if [ -e "$d/feedback.txt" ]; then
    grade=`tail -n 1 $d/feedback.txt`
    IFS='/' read -ra grArray <<< "$grade"
    score=${grArray[0]}
    max=${grArray[1]}
    assignment=assignment + $score
    assignment_total=assignment_total + $max
    echo $d: $grade
    else
    grade='- / -'
    echo $d: $grade
    fi
done

echo ""

for d in t01 t02 t03 t04 t05 t06 t07 t08 t09 t10 t11; do
    if [ -e "$d/feedback.txt" ]; then
    grade=`tail -n 1 $d/feedback.txt`
    IFS='/' read -ra grArray <<< "$grade"
    score=${grArray[0]}
    max=${grArray[1]}
    tutorial=$score+tutorial
    tutorial_total=$max+tutorial_total
    echo $d: $grade
    else
    grade='- / -'
    echo $d: $grade
    fi
done
echo ""
echo Assignment Total: $assignment / $assignment_total
echo Tutorial Total: $tutorial / $tutorial_total