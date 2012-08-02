#!/usr/bin/python
import os,sys,math,re
import numpy as np
import colorsys

def file_len(fname):
        '''Returns the number of lines in a file'''
	with open(fname) as f:
                for i, l in enumerate(f):
                        pass
        return i + 1
        
def htmlColors(num_colors):
        '''Generates list of html colors (e.g. #ff00c8) equally spaced throughout HSV range'''
        colors=[]
        for i in np.arange(0., 360., 360. / num_colors):
                hue = i/360.
                lightness = (50 + np.random.rand() * 10)/100.
                saturation = (90 + np.random.rand() * 10)/100.
                rgb = colorsys.hls_to_rgb(hue, lightness, saturation)
                rgb = (int(rgb[0]*255),int(rgb[1]*255),int(rgb[2]*255))
                s = format((rgb[0]<<16)|(rgb[1]<<8)|rgb[2], '06x')
                colors.append('#'+s)
        return colors

def regexpReplace(string,pattern,new):
        '''Replaces pattern matched in string with new string'''
        regexp = re.compile(pattern)
        match = re.search(regexp,string)
        if match == None:
                s = string
        else:
                s = string.replace(match.group(),new)
        return s
        
def addColor(line,color):
        '''Adds a color property (e.g. color="#ff00c8")to the label tags in the atlas file'''
        pattern = r'">'
        new = '" color="' + color + '">'
        if re.search(r'">$',line) != None:
                return line
        else:
                # if this is index 0, make color = white (e.g. #ffffff)
                if re.search('index="0"', line) != None:
                        new = '" color="#ffffff">'
                return regexpReplace(line,pattern,new)


if __name__ == "__main__":
        ''' This script attempts to convert FSL's Talairach.xml atlas file to the kind of
            atlas file we use in Emergent.

            It will:
             1. add a color property to each label

             You first should add a proper <description> tag.
             For exmaple:
                 <name>Talairach Daemon Labels</name>
                 <description>Contains Talairach Daemon Labels</description>

             After the above manual modificaitons, you shoudld be able to run this script to perform
             the tedious changes of #1 above.

             The output should have lines that look like:

                  <name>Talairach Daemon Labels</name>
                  <description>Contains Talairach Daemon Labels</description>
                  <type>Label</type>
                  <images>
                       <imagefile>/Talairach/Talairach-labels-2mm</imagefile>
                       <summaryimagefile>/Talairach/Talairach-labels-2mm</summaryimagefile>
                  </images>
                  <images>
                       <imagefile>/Talairach/Talairach-labels-1mm</imagefile>
                       <summaryimagefile>/Talairach/Talairach-labels-1mm</summaryimagefile>
                  </images>
             </header>
             <data>
             <label index="0" x="0" y="0" z="0" color="#ffffff">*.*.*.*.*</label>
             <label index="1" x="65" y="21" z="16" color="#f51e0a">Left Cerebellum.Posterior Lobe.Inferior Semi-Lunar Lobule.Gray Matter.*</label>
             <label index="2" x="23" y="20" z="15" color="#f73926">Right Cerebellum.Posterior Lobe.Inferior Semi-Lunar Lobule.Gray Matter.*</label>
              ...
        '''

        # FSl atlas we will be converting
        atlas = 'Talairach.xml'
        out_file = open('Talairach_new.xml', 'w')
        
        # need to know approx how many labels, so we can generate that many unique colors
        num_labels = file_len(atlas)
        colors = htmlColors(num_labels)        
        count = 0
        for line in open(atlas, 'r'):
                l = addColor(line,colors[count])
                count += 1
                out_file.write(l)

        out_file.close()
