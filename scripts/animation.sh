#!/bin/bash

folder=$1
framerate=$2
size=$3
output=$4

ls $folder/*.svg | parallel --gnu convert {} -depth 8 -resize ${size}x${size} {.}.png
avconv -f image2 -r $framerate -i $folder/frame%03d.png -r $framerate -qscale 10 -pix_fmt yuv420p $output
