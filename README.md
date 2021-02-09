# GStremer Boilerplate Element Plugin with CMake Build System

# Building

Navigate to the top project directory, same directory this readme is in.

Make a build director and enter that directory

    mkdir build
    cd build

Setup the cmake project

    # the '..' tells cmake to go back one dir to find the CMakeLists txt file
    cmake ..

Finally build the plugin

    make

# Testing

Tell gstreamer where to look for the newly build plugin

    # run from the build dir
    export GST_PLUGIN_PATH_1_0=$PWD

Use `gst-inspect-1.0` to check the new plugin

    gst-inspect-1.0 boilerplatevideofilter

Test the element in a pipeline

    gst-launch-1.0 videotestsrc ! boilerplatevideofilter ! videoconvert ! fpsdisplaysink

# Generating a Debian Package

Current a binary debian package is used for releasing the gst_boilerplatevideofilter plugin

To build a package

    # Follow the build instructions above

Run make debgen to generate the debian packages

    make debgen

The result will be a debian package in the build directory called 'libgstboilerplatevideofilter-0.1.0.0-aarch64-main-release.deb'

# Installing debian Package

To install this package:

    sudo dpkg -i relative_path_to_deb_file

Use gst-inspect-1.0 to check if the package was installed correctly

    gst-inspect-1.0 boilerplatevideofilter

Note:

The standard gstreamer plugin install path is `/usr/lib/x86_64-linux-gnu//gstreamer-1.0` you may want to install in /opt and tell gstreamer to look there for more plugins

# Installing the element on x86 Ubuntu

Follow just the bild instructions above then tell gstreamer where to fin the element binary.

For example:

    export GST_PLUGIN_PATH_1_0=$PWD


## Removing installed .deb packages

    sudo apt-get remove gst-boilerplatevideofilter

name_of_package is the name of the package in the control file, not the name of the .deb

It may also be helpful to check the installed version is as you expect

    apt-cache policy name_of_package

# Helpful GStreamer stuff

    # check for blacklisted pugins
    gst-inspect-1.0 -b

    # clear blacklister plugins
    rm ~/.cache/gstreamer-1.0/registry*

    find . -name "foo*"

    # enable gstreamer debugging
    export GST_DEBUG=4

# Helpful resources

## Good Tutorial for GTK signals

https://www.cc.gatech.edu/data_files/public/doc/gtk/tutorial/gtk_tut-2.html

## See post here for explenation of gstreamer element communication methods

http://gstreamer-devel.966125.n4.nabble.com/Pass-extra-information-between-elements-in-a-pipeline-td4669768.html
