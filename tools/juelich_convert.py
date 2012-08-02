#!/usr/bin/python
import os,sys,math,re
import numpy as np
import colorsys
'''
The Juelich.xml atlas file that ships with FSL, uses index values which correspond to the 3D volume which contains the voxels of that brain area in the probabilistic atlas. There is one 3D volume per label and the volumes are referenced starting with 0 (i.e. zeroth volume is the first label).

As a shortcut, and so we don't have to explicitly handle 4D atlases, we use the Juelich-maxprob-thr25-1mm image file, which is a 3D file where voxel values correspond to the 4D volume in which they occur. These voxel values thus range from 1:N where N is the number of volumes. This represents a problem since the indices listed in the atlas file are 0:M, where M is the number of volumes minus one. To remedy this, and thus allow us to map from label to index to voxel values, we increment all indices in the Juelich.xml file and insert a dummy label with index 0.
'''

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
        
def modifyIndex(line, new_index):
	'''Increments indices so they start with 1 for first real label'''
        pattern = r'index="[0-9]+"'

        # return an incremented new_index if this line has an index
        if re.search(r'index=',line) != None:
                idx = new_index + 1
        else:
                idx = new_index

        return (re.sub(pattern,lambda x: 'index="' + str(new_index) + '"', line),idx)


def addColor(line,color):
        '''Adds a color property (e.g. color="#ff00c8")to the label tags in the atlas file'''
        pattern = r'">[GW]M '
        if re.search(r'GM ',line) != None:
                tissue_type = 'GM '
        elif re.search(r'WM ',line) != None:
                tissue_type = 'WM '
        else:
                tissue_type = ''

        if re.search('index="0"',line) != None:
                # handle the first label, and force its color to white (#ffffff)'
                pattern = r'">\*'
                new = '" color="#ffffff">*'                
        else:
                new = '" color="' + color + '">' + tissue_type

        return regexpReplace(line,pattern,new)

def addColumnDesignators(line):
        '''Adds special Talairach-style column designators to each label line so we can replace
           with Talairach-style text (e.g. replace R with Right and GM with Gray Matter) or wildcards'''
        pattern = r'">[GW]M '
        if re.search(r'GM ',line) != None:
                tissue_type = 'GM '
        elif re.search(r'WM ',line) != None:
                tissue_type = 'WM '
        else:
                tissue_type = ''
                
        new = '">HEMI_COL.LOBE_COL.GYRUS_COL.TISSUE_COL.' + tissue_type
        return regexpReplace(line,pattern,new)

def addLeftRight(line):
        '''Replace R or L with Right or Left'''
        pattern = r' [RL]</label>'
        new = '</label>'
        if re.search('L</label>',line) != None:
                l = regexpReplace(line,pattern,new)
                pattern = r'HEMI_COL\.'
                new = 'Left.'
                return regexpReplace(l,pattern,new)
        elif re.search('R</label',line) != None:
                l = regexpReplace(line,pattern,new)
                pattern = r'HEMI_COL\.'
                new = 'Right.'
                return regexpReplace(l,pattern,new)
        else:
                return line
        
def addGrayWhite(line):
        '''Replaces GM or WM with Gray Matter or White Matter'''
        pattern = r'[GW]M '
        new = ''
        if re.search('GM ',line) != None:
                l = regexpReplace(line,pattern,new)
                pattern = r'TISSUE_COL\.'
                new = 'Gray Matter.'
                return regexpReplace(l,pattern,new)
        elif re.search('WM ',line) != None:
                l = regexpReplace(line,pattern,new)
                pattern = r'TISSUE_COL\.'
                new = 'White Matter.'
                return regexpReplace(l,pattern,new)
        else:
                return line

def addDotStars(line):
        '''Inserts Talairach-style wildcards (e.g. *) where column designators remain'''
        pattern = r'(HEMI_COL|LOBE_COL|GYRUS_COL|TISSUE_COL)'
        return re.sub(pattern,lambda m: '*',line)

if __name__ == "__main__":
        '''
	    The Juelich.xml atlas file that ships with FSL, uses index values which correspond to the 3D 
	    volume which contains the voxels of that brain area in the probabilistic atlas. There is one
	    3D volume per label and the volumes are referenced starting with 0 (i.e. zeroth volume is the
	    first label).

	    As a shortcut, and so we don't have to explicitly handle 4D atlases, we use the 
	    Juelich-maxprob-thr25-1mm image file, which is a 3D file where voxel values correspond to the
	    4D volume in which they occur. These voxel values thus range from 1:N where N is the number of
	    volumes. This represents a problem since the indices listed in the atlas file are 0:M, where M
	    is the number of volumes minus one. To remedy this, and thus allow us to map from label to 
	    index to voxel values, we increment all indices in the Juelich.xml file and insert a dummy 
	    label with index 0.
	   
	    This script attempts to convert FSL's Juelich.xml atlas file to the kind of
            atlas file we use in Emergent.

            It will:
             1. add a color property to each label
             2. try to modify the labels to be Talairach-style (*.*.*.*.*), with the following 5 fields:
                  a. Hemisphere
                  b. Lobe
                  c. Gyrus
                  d. Tissue type
                  e. Cell type
             3. Increments the label indices (see note above)

             You first should add a proper <description> tag.
             For exmaple:
                 <name>Juelich Histological Atlas</name>
                 <description>Contains Juelich Labels</description>

             You must also insert an initial label with index="0" and x,y,z coordinates of "0".
             For example:
                 <label index="0" x="0" y="0" z="0">*.*.*.*.*</label>

             You also should modify the paths in the <imagefile> and <summaryimagefile> tags to point
             to the appropriate image files you wish to use. Curently July2012, only 1mm files with
             short (uint16) data in a single volume will work and Emergent will use the last 1mm file
             listed in the atlas xml file.
             For example:
                 <imagefile>/Juelich/Juelich-prob-2mm</imagefile>
                 <summaryimagefile>/Juelich/Juelich-maxprob-thr25-2mm</summaryimagefile>

             After the above manual modificaitons, you shoudld be able to run this script to perform
             the tedious changes of #1-3 above.

             The output should have lines that look like:

             <name>Juelich Histological Atlas</name>
              <description>Contains Juelich Labels</description>
              <type>Probabalistic</type>
              <images>
                  <imagefile>/Juelich/Juelich-maxprob-thr0-1mm</imagefile>
                  <summaryimagefile>/Juelich/Juelich-maxprob-thr0-1mm</summaryimagefile>
              </images>
              <images>
                  <imagefile>/Juelich/Juelich-maxprob-thr0-2mm</imagefile>
                  <summaryimagefile>/Juelich/Juelich-maxprob-thr0-2mm</summaryimagefile>
              </images>
            </header>
            <data>
                <label index="0" x="0" y="0" z="0">*.*.*.*.*</label>
                <label index="1" x="125" y="75" z="107" color="#f5ba1f">Left.*.*.Gray Matter.Anterior intra-parietal sulcus hIP1</label>
                <label index="2" x="53" y="78" z="108" color="#f5c93c">Right.*.*.Gray Matter.Anterior intra-parietal sulcus hIP1</label>
                ...
        '''

        # FSl atlas we will be converting
        atlas = 'Juelich.xml'
        out_file = open('Juelich_new.xml', 'w')
        
        # need to know approx how many labels, so we can generate that many unique colors
        num_labels = file_len(atlas)
        colors = htmlColors(num_labels)        
        count = 0
	idx = 0
        for line in open(atlas, 'r'):
                l,idx = modifyIndex(line,idx)
		l = addColor(l,colors[count])
                l = addColumnDesignators(l)
                l = addLeftRight(l)
                l = addGrayWhite(l)
                l = addDotStars(l)
                count += 1
                #print l
                out_file.write(l)

        out_file.close()
