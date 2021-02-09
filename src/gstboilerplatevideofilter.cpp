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
#include <gst/base/gstcollectpads.h>
#include "gstboilerplatevideofilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_boilerplatevideofilter_debug_category);
#define GST_CAT_DEFAULT gst_boilerplatevideofilter_debug_category

static GstStaticPadTemplate eo_sink_factory = GST_STATIC_PAD_TEMPLATE ("eo_sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("{ BGR }"))
    );

static GstStaticPadTemplate ir_sink_factory = GST_STATIC_PAD_TEMPLATE ("ir_sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("{ BGR }"))
    );

/* prototypes boilerplatevideofilter */
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

static GstFlowReturn gst_boilerplatevideofilter_collect_pads (GstCollectPads * cpads,
    GstBoilerPlateVideoFilter * boilerplatevideofilter);

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
  // gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
  //     gst_pad_template_new ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
  //         gst_caps_from_string (VIDEO_SINK_CAPS)));

  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass), 
      &eo_sink_factory);
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass), 
      &ir_sink_factory);
  
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
  boilerplatevideofilter->cpads = gst_collect_pads_new ();
  gst_collect_pads_set_function (boilerplatevideofilter->cpads,
      (GstCollectPadsFunction) GST_DEBUG_FUNCPTR (gst_boilerplatevideofilter_collect_pads),
      boilerplatevideofilter);

  boilerplatevideofilter->eo_sinkpad = 
      gst_pad_new_from_static_template(&eo_sink_factory, eo_sink_factory.name_template);
  GST_PAD_SET_PROXY_CAPS(boilerplatevideofilter->eo_sinkpad);
  gst_element_add_pad (GST_ELEMENT (boilerplatevideofilter),
      boilerplatevideofilter->eo_sinkpad);
  
  boilerplatevideofilter->ir_sinkpad = 
      gst_pad_new_from_static_template(&ir_sink_factory, ir_sink_factory.name_template);
  gst_element_add_pad (GST_ELEMENT (boilerplatevideofilter),
      boilerplatevideofilter->ir_sinkpad);

  gst_collect_pads_add_pad (boilerplatevideofilter->cpads, boilerplatevideofilter->eo_sinkpad,
      sizeof (GstCollectData), NULL, TRUE);
  gst_collect_pads_add_pad (boilerplatevideofilter->cpads, boilerplatevideofilter->ir_sinkpad,
      sizeof (GstCollectData), NULL, TRUE);

  // set default prorerty values
  boilerplatevideofilter->int_property = DEFAULT_INT_PROPERTY;
  boilerplatevideofilter->bool_property = DEFAULT_BOOL_PROPERTY;
}

static GstFlowReturn
gst_boilerplatevideofilter_collect_pads (GstCollectPads * cpads, GstBoilerPlateVideoFilter * boilerplatevideofilter)
{
  GstBuffer *buf1, *buf2;
  GstCaps *caps1, *caps2;

  // buf1 = gst_collect_pads_pop (comp->cpads,
  //     gst_pad_get_element_private (comp->sinkpad));
  // caps1 = gst_pad_get_current_caps (comp->sinkpad);

  // buf2 = gst_collect_pads_pop (comp->cpads,
  //     gst_pad_get_element_private (comp->checkpad));
  // caps2 = gst_pad_get_current_caps (comp->checkpad);

  // if (!buf1 && !buf2) {
  //   gst_pad_push_event (comp->srcpad, gst_event_new_eos ());
  //   return GST_FLOW_EOS;
  // } else if (buf1 && buf2) {
  //   gst_compare_buffers (comp, buf1, caps1, buf2, caps2);
  // } else {
  //   GST_WARNING_OBJECT (comp, "buffer %p != NULL", buf1 ? buf1 : buf2);

  //   comp->count++;
  //   gst_element_post_message (GST_ELEMENT (comp),
  //       gst_message_new_element (GST_OBJECT (comp),
  //           gst_structure_new ("delta", "count", G_TYPE_INT, comp->count,
  //               NULL)));
  // }

  // if (buf1)
  //   gst_pad_push (comp->srcpad, buf1);

  // if (buf2)
  //   gst_buffer_unref (buf2);

  // if (caps1)
  //   gst_caps_unref (caps1);

  // if (caps2)
  //   gst_caps_unref (caps2);

  return GST_FLOW_OK;
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
