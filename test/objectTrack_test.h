//
//  objecttrack_test.h
//
//  Created by Isaac Reed on 3/9/20
//

#include <iostream>
#include <signal.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/controller/gstinterpolationcontrolsource.h>
#include <gst/controller/gstdirectcontrolbinding.h> 
#include <string>
#include <sstream>
#include <pthread.h>        
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <execinfo.h>

typedef struct _Elements{
  GstElement *pipeline,
             *source1,
             *nvvidconv,
             *videoconvert,
             *source1_queue,
             *objecttrack,
             *sink_queue,
             *sink_videoconvert,
             *sink;
  GstCaps *source1_caps, *videoconvert_caps, *bgr_caps;
  GMainLoop *loop;

}Elements;

Elements elements;