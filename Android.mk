LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := libpcap 

LOCAL_SRC_FILES := libpcap.a

include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := mmdump
	
LOCAL_SRC_FILES:=\
	main.cpp\
	json.cpp\
	lodepng.cpp\
	mmdump.cpp\
	mmoperate.cpp
LOCAL_STATIC_LIBRARIES := libpcap

include $(BUILD_EXECUTABLE)
