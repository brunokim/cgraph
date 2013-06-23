#!/bin/bash

folder=$1
framerate=$2
output=$3

ls $folder/*.svg | parallel --gnu convert {} -depth 8 {.}.png
avconv -f image2 -r $framerate -i $folder/frame%05d.png -r $framerate -qscale 10 -pix_fmt yuv420p $output
