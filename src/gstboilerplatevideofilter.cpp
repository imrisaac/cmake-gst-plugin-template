/* GStreamer
 * Copyright (C) 2020 Isaac Reed <ireed@altavian.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-boilerplatevideofilter
 *
 * Boilerplate template video filter element using cmake build system
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 videotestsrc ! boilerplatevideofilter ! fakevideosink
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>
#include "gstboilerplatevideofilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_boilerplatevideofilter_debug_category);
#define GST_CAT_DEFAULT gst_boilerplatevideofilter_debug_category

#define ARC_SECS_PER_RADIAN 206265

/* prototypes */
static void gst_boilerplatevideofilter_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_boilerplatevideofilter_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static gboolean gst_boilerplatevideofilter_set_info (GstVideoFilter * filter,
    GstCaps * incaps, GstVideoInfo * in_info, GstCaps * outcaps,
    GstVideoInfo * out_info);

static GstFlowReturn gst_boilerplatevideofilter_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * inframe, GstVideoFrame * outframe);
static GstFlowReturn gst_boilerplatevideofilter_transform_frame_ip (GstVideoFilter *
    filter, GstVideoFrame * frame);

#define DEFAULT_INT_PROPERTY 5
#define DEFAULT_BOOL_PROPERTY 0

enum{
  PROP_0,
  PROP_INT_PROPERTY,
  PROP_BOOL_PROPERTY
};

/* pad templates */
#define VIDEO_SRC_CAPS \
    GST_VIDEO_CAPS_MAKE("{ BGR }")

#define VIDEO_SINK_CAPS \
    GST_VIDEO_CAPS_MAKE("{ BGR }")

/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstBoilerPlateVideoFilter, gst_boilerplatevideofilter, GST_TYPE_VIDEO_FILTER,
    GST_DEBUG_CATEGORY_INIT (gst_boilerplatevideofilter_debug_category, "boilerplatevideofilter", 0,
        "debug category for boilerplatevideofilter element"));

static void
gst_boilerplatevideofilter_class_init (GstBoilerPlateVideoFilterClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *base_transform_class =
      GST_BASE_TRANSFORM_CLASS (klass);
  GstVideoFilterClass *video_filter_class = GST_VIDEO_FILTER_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
      gst_pad_template_new ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
          gst_caps_from_string (VIDEO_SRC_CAPS)));
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
      gst_pad_template_new ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
          gst_caps_from_string (VIDEO_SINK_CAPS)));

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
      "Gstreamer Object Tracker", "Generic", "Object and scene tracking gsreamer plugin",
      "Isaac Reed <ireed@altavian.com>");

  gobject_class->set_property = gst_boilerplatevideofilter_set_property;
  gobject_class->get_property = gst_boilerplatevideofilter_get_property;
  video_filter_class->set_info = GST_DEBUG_FUNCPTR (gst_boilerplatevideofilter_set_info);
  video_filter_class->transform_frame_ip =
      GST_DEBUG_FUNCPTR (gst_boilerplatevideofilter_transform_frame_ip);


  g_object_class_install_property(gobject_class, PROP_INT_PROPERTY,
    g_param_spec_int("int-property", "An example of an integer property",
      "An example of an integer property",
      0, 200, DEFAULT_INT_PROPERTY, 
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property(gobject_class, PROP_BOOL_PROPERTY,
    g_param_spec_boolean("bool-property", "An example of an boolean property",
      "An example of an boolean property",
       DEFAULT_BOOL_PROPERTY, 
      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));
}

static void
gst_boilerplatevideofilter_init (GstBoilerPlateVideoFilter * boilerplatevideofilter)
{
  // set default prorerty values
  boilerplatevideofilter->int_property = DEFAULT_INT_PROPERTY;
  boilerplatevideofilter->bool_property = DEFAULT_BOOL_PROPERTY;
}

void
gst_boilerplatevideofilter_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstBoilerPlateVideoFilter *boilerplatevideofilter = GST_BOILERPLATE_VIDEOFILTER (object);

  GST_DEBUG_OBJECT(boilerplatevideofilter, "set_property");

  switch (property_id) {
    case PROP_INT_PROPERTY:
      boilerplatevideofilter->int_property = g_value_get_int(value);
      break;

    case PROP_BOOL_PROPERTY:
      boilerplatevideofilter->bool_property = g_value_get_boolean(value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_boilerplatevideofilter_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstBoilerPlateVideoFilter *boilerplatevideofilter = GST_BOILERPLATE_VIDEOFILTER (object);

  GST_DEBUG_OBJECT (boilerplatevideofilter, "get_property");

  switch (property_id) {
    case PROP_INT_PROPERTY:
      g_value_set_int(value, boilerplatevideofilter->int_property);
      break;
      
    case PROP_BOOL_PROPERTY:
      g_value_set_boolean(value, boilerplatevideofilter->bool_property);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static gboolean
gst_boilerplatevideofilter_set_info (GstVideoFilter * filter, GstCaps * incaps,
    GstVideoInfo * in_info, GstCaps * outcaps, GstVideoInfo * out_info)
{
  GstBoilerPlateVideoFilter *boilerplatevideofilter = GST_BOILERPLATE_VIDEOFILTER (filter);

  GST_DEBUG_OBJECT (boilerplatevideofilter, "set_info");

  return TRUE;
}

/* transform */
static GstFlowReturn
gst_boilerplatevideofilter_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * inframe, GstVideoFrame * outframe)
{
  GstBoilerPlateVideoFilter *boilerplatevideofilter = GST_BOILERPLATE_VIDEOFILTER (filter);

  GST_DEBUG_OBJECT (boilerplatevideofilter, "transform_frame");

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_boilerplatevideofilter_transform_frame_ip (GstVideoFilter * filter,
    GstVideoFrame * frame)
{
  GstBoilerPlateVideoFilter *boilerplatevideofilter = GST_BOILERPLATE_VIDEOFILTER (filter);

  return GST_FLOW_OK;
}

static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "boilerplatevideofilter", GST_RANK_NONE,
      GST_TYPE_BOILERPLATE_VIDEOFILTER);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION THISBUILD
#endif
#ifndef PACKAGE
#define PACKAGE "gst_boilerplate_videofilter_package"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "gst_boilerplate_videofilter"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "https://github.com/imrisaac/cmake-gst-plugin-template"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    boilerplatevideofilter,
    "Boilerplate template for gstreamer videofilter plugin using cmake build system",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)
