Demo: GrabImage.cpp
	g++ -g -o GrabImage GrabImage.cpp -I../../../include -I/usr/local/include/opencv2 -Wl,-rpath=$(MVCAM_COMMON_RUNENV)/64 -L$(MVCAM_COMMON_RUNENV)/64 -lMvCameraControl -lpthread -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_videoio
	

clean:
	rm GrabImage -rf
