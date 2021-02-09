/* GStreamer
 * Copyright (C) 2020 FIXME <ireed@altavian.com>
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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_BOILERPLATE_VIDEOFILTER_H_
#define _GST_BOILERPLATE_VIDEOFILTER_H_

#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>
#include "version.h"

G_BEGIN_DECLS

#define GST_TYPE_BOILERPLATE_VIDEOFILTER   (gst_boilerplatevideofilter_get_type())
#define GST_BOILERPLATE_VIDEOFILTER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_BOILERPLATE_VIDEOFILTER,GstBoilerPlateVideoFilter))
#define GST_BOILERPLATE_VIDEOFILTER_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_BOILERPLATE_VIDEOFILTER,GstBoilerPlateVideoFilterClass))
#define GST_IS_BOILERPLATE_VIDEOFILTER(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_BOILERPLATE_VIDEOFILTER))
#define GST_IS_BOILERPLATE_VIDEOFILTER_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_BOILERPLATE_VIDEOFILTER))

typedef struct _GstBoilerPlateVideoFilter GstBoilerPlateVideoFilter;
typedef struct _GstBoilerPlateVideoFilterClass GstBoilerPlateVideoFilterClass;

struct _GstBoilerPlateVideoFilter
{
  GstVideoFilter base_boilerplatevideofilter;

  GstPad *srcpad;
  GstPad *ir_sinkpad;
  GstPad *eo_sinkpad;

  GstCollectPads *cpads;

  gint count;

  // properties
  gint int_property;
  bool bool_property;

};

struct _GstBoilerPlateVideoFilterClass
{
  GstVideoFilterClass base_boilerplatevideofilter_class;
};

GType gst_boilerplatevideofilter_get_type (void);

G_END_DECLS

#endif
