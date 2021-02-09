//
//  objecttrack_test.h
//
//  Created by Isaac Reed on 3/9/20
//

// gst-launch-1.0 nvarguscamerasrc sensor-mode=0 ! 
// 'video/x-raw(memory:NVMM), width=2592, height=1944' ! 
// nvvidconv ! 'video/x-raw, format=RGBA, width=480, height=360' ! 
// videoconvert ! 'video/x-raw, format=BGR' ! objecttrack ! 
// valve drop=false ! videoconvert ! nveglglessink qos=false window-x=1440
//

#include "objectTrack_test.h"

using namespace std;

bool signal_recieved = false;
int sigint_count = 0;

void sig_handler(int signo){
	if( signo == SIGINT ){
		//printf("received SIGINT\n");
    if(sigint_count == 3){
      cout << "\ni said please enter 'q' to exit" << endl;
    }else if(sigint_count == 6){
      cout << "\nok seriously just enter 'q' it is not that hard, the key next to the 'w'" << endl;
    }else if(sigint_count == 9){
      cout << "\nsorry wrong again enter 'q' to exit" << endl;
    }else if(sigint_count == 12){
      cout << "\nnope, the letter 'q' thats what you want" << endl;
    }else if(sigint_count == 15){
      cout << "\n no no no no, still wrong" << endl;
    }else{
      cout << "\nplease enter 'q' to exit" << endl;
    }
		//signal_recieved = true;
    sigint_count ++;

	}
}

void *TermioControl(void *ptr){
  cout << "termio control running" << endl;
  struct termios t;
  tcgetattr(STDIN_FILENO, &t); //get the current terminal I/O structure
  t.c_lflag &= ~ICANON; //Manipulate the flag bits to do what you want it to do
  tcsetattr(STDIN_FILENO, TCSANOW, &t); //Apply the new settings

  while(!signal_recieved){
    char input;
    cin >> input;
    cout << " | ";
    switch(input){
      case 27:
      case 'q':
        cout << "quit" << endl;
        signal_recieved = true;
        break;

      case 's':
        gboolean enabled;
        cout << "scene track toggle" << endl;
        g_object_get(G_OBJECT(elements.objecttrack), "enable", &enabled, NULL);
        g_object_set(G_OBJECT(elements.objecttrack), "enable", !enabled, NULL);
        break;
      
      case 'p':
        break;
      
      case '=':
      case 't':
        break;

      case '-':
      case 'w':
        break;

      case 92:  /* \ */
        break;

      case '[':
        break;

      case ']':
        break;
      
      case '1':
        cout << "switch to cam 1" << endl;
        break;

      case '2':
        cout << "switch to cam 2" << endl;
        break;

      case '3':
        cout << "switch to cam 3" << endl;
        break;
        
      case '4':
        cout << "switch to cam 4" << endl;
        break;

      case '5':
        cout << "switch to cam 5" << endl;
        break;

      case 'a':
        break;

      default:
        break;
    }
  }
  cout << "termio control exit" << endl;
}

static gboolean
bus_callback (GstBus * bus, GstMessage * message)
{
  const GstStructure *msg_struct;
  //g_print ("Got %s message\n", GST_MESSAGE_TYPE_NAME (message));
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:{
      GError *err;
      gchar *debug;

      gst_message_parse_error (message, &err, &debug);
      g_print ("Error: %s\n", err->message);
      g_error_free (err);
      g_free (debug);

      signal_recieved = true;
      break;
    }
    case GST_MESSAGE_EOS:
      /* end-of-stream */
      signal_recieved = true;
      break;

    case GST_MESSAGE_ELEMENT:
      msg_struct = gst_message_get_structure (message);
        if (gst_structure_has_name (msg_struct, "objecttrack-rate-out")){
          const GValue *pitch_rate_g = gst_structure_get_value(msg_struct, "pitch-rate");
          const GValue *yaw_rate_g = gst_structure_get_value(msg_struct, "yaw-rate");
          float pitch_rate = g_value_get_float(pitch_rate_g);
          float yaw_rate = g_value_get_float(yaw_rate_g);
          // cout << "recieved objecttrack rate out message "
          //      << " pitch " << pitch_rate 
          //      << " yaw " << yaw_rate << endl;
          
        }
      break;


    default:
      /* unhandled message */
      break;
  }

  /* we want to be notified again the next time there is a message
   * on the bus, so returning TRUE (FALSE means we want to stop watching
   * for messages on the bus and our callback should not be called again)
   */
  return TRUE;
}

bool SourceSetup(){
  g_object_set(G_OBJECT(elements.source1), "sensor-id", 0, NULL);
  g_object_set(G_OBJECT(elements.source1), "sensor-mode", 2, NULL);
  g_object_set(G_OBJECT(elements.source1), "maxperf", true, NULL);
  
  // source 1 caps filters
  if(!gst_element_link_filtered(elements.source1, elements.nvvidconv, elements.source1_caps)){
    cout << "source 1 nvvidconv caps link failed" << endl;
    return false;
  }

  if(!gst_element_link_filtered(elements.nvvidconv, elements.source1_queue, elements.videoconvert_caps)){
    cout << "source 1 videoconvert caps link failed" << endl;
    return false;
  }
  return true;
}

bool SinkSetup(){
  g_object_set(G_OBJECT(elements.sink), "qos", false, NULL);
  g_object_set(G_OBJECT(elements.sink), "window-x", 1440, NULL);
  return true;
}

int main(int argc, char *argv[]){

  if(signal(SIGINT, sig_handler) == SIG_ERR){
    cout << "cannot register signal handler" << endl;
  }

  GstBus *bus;
  guint bus_watch_id;
  GstStateChangeReturn ret;
  GMainLoop *main_loop;

  // GStreamer library init 
  gst_init(NULL, NULL);

  elements.pipeline = gst_pipeline_new("pipeline");
  elements.source1 = gst_element_factory_make("nvarguscamerasrc", "source1");
  elements.nvvidconv = gst_element_factory_make("nvvidconv", "capture-nvvidconv");
  elements.videoconvert = gst_element_factory_make("videoconvert", "capture-videoconvert");
  elements.source1_queue = gst_element_factory_make("queue", "source-queue");
  elements.objecttrack = gst_element_factory_make("objecttrack", "objectrack1");
  elements.sink_videoconvert = gst_element_factory_make("videoconvert", "sink-videoconvert");
  elements.sink_queue = gst_element_factory_make("queue", "sink-queue");
  elements.sink = gst_element_factory_make("nveglglessink", "sink");

  // caps filters
  elements.source1_caps = gst_caps_from_string("video/x-raw(memory:NVMM), width=1920, height=1080");
  elements.videoconvert_caps = gst_caps_from_string("video/x-raw, format=RGBA, width=480, height=360");
  elements.bgr_caps = gst_caps_from_string("video/x-raw, format=BGR");
  bool elements_ok = true;

  if(!elements.pipeline){ cerr << "failed to create pipeine" << endl; elements_ok = false;}
  if(!elements.source1){ cerr << "failed to create source1 element" << endl; elements_ok = false;}
  if(!elements.nvvidconv){ cerr << "failed to create nvvidconv element" << endl; elements_ok = false;}
  if(!elements.videoconvert){ cerr << "failed to create videoconvert element" << endl; elements_ok = false;}
  if(!elements.source1_queue){ cerr << "failed to create source1_queue element" << endl; elements_ok = false;}
  if(!elements.objecttrack){ cerr << "failed to create objectrack element" << endl; elements_ok = false;}
  if(!elements.videoconvert){ cerr << "failed to create videoconvert element" << endl; elements_ok = false;}
  if(!elements.sink_queue){ cerr << "failed to create sink queue element" << endl; elements_ok = false;}
  if(!elements.sink){ cerr << "failed to create sink element" << endl; elements_ok = false;}

  if(elements_ok == false){
    cout << "not all elements could be created, exiting" << endl;
    return -1;
  }

  gst_bin_add_many(GST_BIN(elements.pipeline), elements.source1, elements.nvvidconv,
                           elements.videoconvert, elements.source1_queue,
                           elements.objecttrack, elements.sink_queue,
                           elements.sink_videoconvert, elements.sink, NULL);

  if(!SourceSetup()){
    gst_object_unref(elements.pipeline);
    return -1;
  }

  if(!SinkSetup()){
    gst_object_unref(elements.pipeline);
    return -1;
  }

  gst_element_link(elements.source1_queue, elements.videoconvert);

  if(!gst_element_link_filtered(elements.videoconvert, elements.objecttrack, elements.bgr_caps)){
    cout << "bgr caps link failed" << endl;
    gst_object_unref(elements.pipeline);
    return -1;
  }

  gst_element_link_many(elements.objecttrack, elements.sink_videoconvert, elements.sink, NULL);

  bus = gst_pipeline_get_bus(GST_PIPELINE(elements.pipeline));

  ret = gst_element_set_state(elements.pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    cout << "Unable to set the pipeline to the playing state." << endl;
    gst_object_unref(elements.pipeline);
    return -1;
  }
  
  pthread_t termio_thread;
    int err = pthread_create(&termio_thread, NULL, &TermioControl, NULL);
  if(err){
    cout << "termio thread creation failed, "
            "user control will not be available" << endl;
  }

  gst_bus_enable_sync_message_emission(bus);
  usleep(500);
  GstMessage *msg;
  while(!signal_recieved){
    usleep(50);
    msg = gst_bus_pop(bus);
    if(msg != NULL){
      bus_callback(bus, msg);
    }
  }
  gst_message_unref(msg);

  err = pthread_join(termio_thread, NULL);
  if(err){
    cout << "termio thread join fail err: " << err << endl;
  }else{
    cout << "termio thread join success" << endl;
  }

  gst_element_set_state(elements.pipeline, GST_STATE_NULL);

  return 0;
}