Demo: GrabImage.cpp
	g++ -std=c++11 -g -o GrabImage GrabImage.cpp -I../../../include -I /usr/local/include/opencv2 -I /usr/local/include/eigen3 -I /usr/local/include/aruco -Wl,-rpath=$(MVCAM_COMMON_RUNENV)/64 -L$(MVCAM_COMMON_RUNENV)/64 -lMvCameraControl -lpthread -L /usr/local/lib -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs -laruco

clean:
	rm GrabImage -rf
