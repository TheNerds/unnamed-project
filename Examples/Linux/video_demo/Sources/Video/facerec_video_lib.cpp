/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include "/usr/local/include/opencv2/core.hpp"
#include "/usr/local/include/opencv2/face.hpp"
#include "/usr/local/include/opencv2/highgui.hpp"
#include "/usr/local/include/opencv2/imgproc.hpp"
#include "/usr/local/include/opencv2/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace cv::face;
using namespace std;

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

extern "C" void print_msg(){
printf("teste");
}

// edschneider: neste ponto foi transformado a função main por uma função para a ser chamada pelo programa do AR Drone,
// recebendo uma imagem IplImage, convertendo para Mat, processando a imagem e devolvendo uma IplImage novamente.

// TODO: remover a instanciação dos objetos haar e csv daqui para otimização do código!

//int main(int argc, const char *argv[]) {
extern "C" IplImage* facerecog(IplImage* cap){
    //printf("facerec inicio\n");
    
    // edschneider: Neste ponto foi desativado a passagem dos parâmetros usada pelo código original para a localização dos arquivos XML, CSV e DeviceId da WebCam.
    // Check for valid command line arguments, print usage
    // if no arguments were given.
    /*    if (argc != 4) {
        cout << "usage: " << argv[0] << " </path/to/haar_cascade> </path/to/csv.ext> </path/to/device id>" << endl;
        cout << "\t </path/to/haar_cascade> -- Path to the Haar Cascade for face detection." << endl;
        cout << "\t </path/to/csv.ext> -- Path to the CSV file with the face database." << endl;
        cout << "\t <device id> -- The webcam device id to grab frames from." << endl;
        exit(1);
    }
    */

    // Get the path to your CSV:
    // edschneider: Localização do arquivo HAAR
    //string fn_haar = string(argv[1]); 
    string fn_haar = "/home/developer/Workspaces/TheNerds/unnamed-project/Examples/Linux/video_demo/Sources/Video/haarcascade_frontalface_default.xml";
    // edschneider: Localização do arquivo CSV do banco de imagens.
    //string fn_csv = string(argv[2]);
    string fn_csv = "/home/developer/Workspaces/TheNerds/unnamed-project/Examples/Linux/video_demo/Sources/Video/images.csv";
    // edschneider: Desativado pois não será usado imagens da WebCam e sim imagens fornecidas pelo AR Drone
    //int deviceId = atoi(argv[3]); 
    // These vectors hold the images and corresponding labels:
    vector<Mat> images;
    vector<int> labels;
    // Read in the data (fails if no valid input filename is given, but you'll get an error message):
    try {
        read_csv(fn_csv, images, labels);
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }
    //printf("facerec 2\n");
    // Get the height from the first image. We'll need this
    // later in code to reshape the images to their original
    // size AND we need to reshape incoming faces to this size:
    int im_width = images[0].cols;
    int im_height = images[0].rows;
    // Create a FaceRecognizer and train it on the given images:
    //printf("facerec 3\n");
    Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
    //printf("facerec 4\n");
    model->train(images, labels);
    // That's it for learning the Face Recognition model. You now
    // need to create the classifier for the task of Face Detection.
    // We are going to use the haar cascade you have specified in the
    // command line arguments:
    //
    //printf("facerec 5\n");
    CascadeClassifier haar_cascade;
    //printf("facerec 6\n");
    haar_cascade.load(fn_haar);
    // edschneider: Desativado pois foi substituido a origem do video da webcam pela passagem de imagens do programa do AR Drone
    // Get a handle to the Video device:
    // VideoCapture cap(deviceId);
    // Check if we can use this device at all:
    /*
    if(!cap.isOpened()) {
        cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
        return -1;
    }
    */

    // Holds the current frame from the Video device:
    Mat frame;

    // edschneider: desabilitando o loop infinito do programa para ser controlado pelo programa do AR Drone
    //for(;;) {
	//printf("facerec 7\n");
	// edschneider: Desativado a operação de bits com os frames vindos da WebCam e incluído a conversão do formato 
        // IplImage para o Mat que é necessário para as rotinas de reconhecimento facial no seu lugar.
	//cap >> frame;
	frame = cvarrToMat(cap); 
	//printf("facerec 8\n");
        // Clone the current frame:
        Mat original = frame.clone();
        // Convert the current frame to grayscale:
        Mat gray;
        cvtColor(original, gray, CV_BGR2GRAY);
        // Find the faces in the frame:
        vector< Rect_<int> > faces;
        haar_cascade.detectMultiScale(gray, faces);
        // At this point you have the position of the faces in
        // faces. Now we'll get the faces, make a prediction and
        // annotate it in the video. Cool or what?
        for(int i = 0; i < faces.size(); i++) {
            // Process face by face:
            Rect face_i = faces[i];
            // Crop the face from the image. So simple with OpenCV C++:
            Mat face = gray(face_i);
            // Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
            // verify this, by reading through the face recognition tutorial coming with OpenCV.
            // Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
            // input data really depends on the algorithm used.
            //
            // I strongly encourage you to play around with the algorithms. See which work best
            // in your scenario, LBPH should always be a contender for robust face recognition.
            //
            // Since I am showing the Fisherfaces algorithm here, I also show how to resize the
            // face you have just found:
            Mat face_resized;
            cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
            // Now perform the prediction, see how easy that is:
            int prediction = model->predict(face_resized);
            // And finally write all we've found out to the original image!
            // First of all draw a green rectangle around the detected face:
            rectangle(original, face_i, CV_RGB(0, 255,0), 1);
            // Create the text we will annotate the box with:
            string box_text = format("Prediction = %d", prediction);
            // Calculate the position for annotated text (make sure we don't
            // put illegal values in there):
            int pos_x = std::max(face_i.tl().x - 10, 0);
            int pos_y = std::max(face_i.tl().y - 10, 0);
            // And now put it into the image:
            putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);
        }
 	// edschneider: Desabilitado a visualização da imagem final para ser passada ao programa do AR Drone via return desta função
        // Show the result:
        //imshow("face_recognizer", original);
        // And display it:
        //char key = (char) waitKey(20);
        // Exit this loop on escape:
        //if(key == 27)
        //    break;
    //}
    // edschneider: retornando a imagem processada para o programa do AR Drone que chamou a função
    //return 0;
    // printf("facerec 10\n");
    IplImage* retipl;
    retipl = cvCreateImage(cvSize(original.cols,original.rows),8,3);
    IplImage ipltemp=original;
    cvCopy(&ipltemp,retipl);
    //printf("facerec final\n");
    return retipl;
}
