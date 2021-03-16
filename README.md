# emergent

This is the C++ version of emergent.  Main documentation is here: https://grey.colorado.edu/emergent -- basic install and build information is on the [Github Wiki](https://github.com/emer/cemer/wiki)

We are no longer developing this software.  We are now developing a new framework based on the `Go` language, with a `Python` interface: https://github.com/emer/emergent

The current release built from github sources (source code only) is: https://github.com/emer/cemer/releases/tag/v8.6.1

The latest release with built packages is: https://github.com/emer/cemer/releases/tag/v8.5.2, released Feb, 2018.

The main dependency files for current releases are found in: https://github.com/emer/cemer/releases/tag/v8.5.1

This github repository was converted from the svn original, and has captured the full history of git tags (including historical dates!) in: https://github.com/emer/cemer/tags

# About

*emergent* is a comprehensive neural network simulator that enables the creation and analysis of complex, sophisticated models of the brain in the world; features:

* Full browser and 3D GUI for constructing, visualizing, & interacting.
    + Accessible to non-programmers
    + But also highly productive for experts, used daily in scientific research.
    
* Powerful C++ scripting language, `css` (not ''that'' css), GUI `Program`ming environment (IDE) -- `TypeAccess` access to C++.

* Rich, dynamic, embodied environments for training networks:
    + `DataTable` for network inputs and `DataProc`, `DataAnal`, `DataGen` (filtering, grouping, sorting, dimensionality reduction, graphing, etc).
    
    + Newtonian physics simulator for Virtual Environment, e.g., a biophysically realistic human arm, and realistic embodied, dynamic vision.
    
    + Sensory filtering for vision, audition, and vocal-tract speech.
    
* Many classic neural network algorithms and variants: Backpropagation (e.g., deep convolutional neural networks), Constraint Satisfaction, Self Organizing, and the Leabra algorithm which incorporates many of the most important features from each of these algorithms, in a biologically consistent manner.  Also, symbolic / subsymbolic ACT-R.

    + Highly optimized vector-based back-end code with thread-specific memory allocation, and GPU (CUDA); Convenient compute cluster for GUI-based job control and data management.
    
* In use for decades, for hundreds of scientific publications from a variety of different labs.  Detailed models of the hippocampus, prefrontal cortex, basal ganglia, visual cortex, cerebellum, etc.

    + Direct descendant of earlier simulators: PDP (1986) and PDP++ (1995).
