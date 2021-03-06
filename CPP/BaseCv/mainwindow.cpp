#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<iostream>
#include "colordetecor.h"
#include "histogram1d.h"
#include "morphofeatures.h"
#include "laplacianzc.h"
#include "linesfinder.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_pushButton_clicked(){

    QString fName = QFileDialog::getOpenFileName(this,
                                                 tr("Open Image"),".",
                                                 tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));


    image = cv::imread(fName.toStdString());
    cv::namedWindow("Original Image");
    cv::imshow("Original Image", image);
}

void MainWindow::on_pushButton_2_clicked(){
    cv::flip(image,image,1);
    // change color channel ordering
    cv::cvtColor(image,image,CV_BGR2RGB);

    displayImage(image);
}


void MainWindow::on_pushButton_3_clicked(){

    salt(image,3000);

    displayImage(image);
}

void MainWindow::on_pushButton_4_clicked(){
    colorReduce(image);

    displayImage(image);
}

void MainWindow::on_pushButton_5_clicked(){
    cv::Mat result;
    result.create(image.rows, image.cols, image.type());
    colorReduce(image,result);
    displayImage(result);
}


void MainWindow::on_pushButton_6_clicked(){
    cv::Mat result;
    result.create(image.size(), image.type());
    sharpen(image,result);
    displayImage(result);
}

void MainWindow::on_pushButton_7_clicked(){
    cv::Mat result;
    result.create(image.size(), image.type());
    sharpen2D(image,result);
    displayImage(result);
}


void MainWindow::on_pushButton_8_clicked(){
    ColorDetecor cd;

    cd.setTargetColor(130,150,230);

    cv::Mat result = cd.process(image);
    displayImage(result);
}

void MainWindow::on_pushButton_9_clicked(){
    Histogram1D hist;
    cv::Mat histImg = hist.getHistogramImage(image);

    cv::namedWindow("Histogram");
    cv::imshow("Histogram",histImg);
}


void MainWindow::on_pushButton_10_clicked(){
    Histogram1D hist;

    cv::Mat lut = hist.produceLookUpTable();

    cv::Mat lutRes = hist.applyLookUp(image,lut);

    displayImage(lutRes);
}

void MainWindow::on_pushButton_11_clicked(){
    Histogram1D hist;

    cv::Mat stretch = hist.stretch(image,100);
    displayImage(stretch);

    cv::Mat histImg = hist.getHistogramImage(stretch);
    cv::namedWindow("Histogram");
    cv::imshow("Histogram",hist.getHistogramImage(histImg));

}

void MainWindow::on_pushButton_12_clicked(){

    cv::Mat res;
    cv::erode(image, res,cv::Mat());
    displayImage(res);
}

void MainWindow::on_pushButton_13_clicked(){
    cv::Mat res;
    cv::dilate(image, res,cv::Mat());
    displayImage(res);
}


void MainWindow::on_pushButton_14_clicked(){
    MorphoFeatures morpho;
    morpho.setThreshold(100);
    cv::Mat edges;

    edges = morpho.getEdges(image);

    displayImage(edges);

}


void MainWindow::on_pushButton_15_clicked(){
    MorphoFeatures morpho;
    cv::Mat corners;
    corners = morpho.getCorners(image);

    morpho.drawOnImage(corners,image);
    displayImage(image);
}


void MainWindow::on_pushButton_16_clicked(){
    cv::Mat result;

    cv::blur(image,result,cv::Size(5,5));
    displayImage(result);
}

void MainWindow::on_pushButton_17_clicked(){
    cv::Mat reducedImage;

    cv::pyrDown(image,reducedImage);
    displayImage(reducedImage);

}


void MainWindow::on_pushButton_18_clicked(){
    cv::Mat resizedImage;

    cv::resize(image,resizedImage,
               cv::Size(image.cols/3,image.rows/3));
    displayImage(resizedImage);
}



void MainWindow::on_pushButton_19_clicked(){
    cv::Mat result;

    cv::medianBlur(image,result,5);
    displayImage(result);
}



void MainWindow::on_pushButton_20_clicked(){
    cv::Mat sobelX;
    cv::Sobel(image,sobelX,CV_8U,1,0,3,0.4,128);
    displayImage(sobelX);

}


void MainWindow::on_pushButton_21_clicked(){
    cv::Mat sobelY;
    cv::Sobel(image,sobelY,CV_8U,0,1,3,0.4,128);
    displayImage(sobelY);
}


void MainWindow::on_pushButton_22_clicked(){
    LaplacianZC laplacianZC;
    laplacianZC.setAperture(7);

    cv::Mat flap = laplacianZC.computeLaplacian(image);

    cv::Mat laplace;

    laplace = laplacianZC.getLaplacianImage();
    displayImage(laplace);

}


void MainWindow::on_pushButton_23_clicked(){
     LaplacianZC laplacianZC;
     laplacianZC.setAperture(7);
     cv::Mat flap= laplacianZC.computeLaplacian(image);

     cv::Mat zeros = laplacianZC.getZeroCrossings();

     displayImage(zeros);
}

void MainWindow::on_pushButton_24_clicked(){
    cv::Mat counturs;
    cv::Canny(image,counturs,125,350);

    displayImage(counturs);
}

void MainWindow::on_pushButton_25_clicked(){
    LinesFinder finder;

      cv::Mat counturs;
    // set probabilistic hough params
    finder.setLineLengthAndGap(100,20);
    finder.setMinVote(80);

    // detect and draw lines
    std::vector<cv::Vec4i> lines = finder.findLines(counturs);
    finder.drawDetectedLines(image);


    displayImage(image);
}

// n - number of pixels to overwrite
void MainWindow::salt(cv::Mat &image, int n){
    srand(cv::getTickCount());
    for (int k=0;k<n;k++){
        int i = rand()%image.cols;
        int j = rand()%image.rows;

        if (image.channels() == 1){
            //gray level img
            image.at<uchar>(j,i) = 255;
        } else if (image.channels() == 3){
            //color img
            image.at<cv::Vec3b>(j,i)[0] = 255;
            image.at<cv::Vec3b>(j,i)[1] = 255;
            image.at<cv::Vec3b>(j,i)[2] = 255;

        }
    }
}



void MainWindow::colorReduce(cv::Mat &image, int div){

    if (image.isContinuous()){
        //no padded pixels
        image.reshape(1,image.cols*image.rows);
    }

    // number of lines
    int nl = image.rows;

    // number of elements per line
    int nc = image.cols * image.channels();
    for (int j =0; j<nl; j++){

        // address of the row
        uchar* data = image.ptr<uchar>(j);

        //process each pixel
        for (int i=0; i<nc; i++){

            // there are many more reduction formulas
            data[i] = data[i]/div*div +div/2;
        }

    }

}

void MainWindow::colorReduce(cv::Mat &image, cv::Mat &result, int div){
    // number of lines
    int nl = image.rows;

    // number of elements per line
    int nc = image.cols * image.channels();

    for (int j =0; j<nl; j++){

        // address of the row
        const uchar* data_in = image.ptr<uchar>(j);
        uchar* data_out = result.ptr<uchar>(j);

        //process each pixel
        for (int i=0; i<nc; i++){

            // there are many more reduction formulas
            data_out[i] = data_in[i]/div*div +div/2;
        }

    }

}


void MainWindow::colorReduceIter(cv::Mat &image, int div){

    // obtain an iterator at init pos
    cv::Mat_<cv::Vec3b>::iterator it = image.begin<cv::Vec3b>();

    // obtain end position
    cv::Mat_<cv::Vec3b>::iterator itend = image.end<cv::Vec3b>();

    //loop over all pixels
    for(; it!=itend; ++it){
        //process each pixel
        (*it)[0] = (*it)[0]/div*div +div/2;
        (*it)[1] = (*it)[1]/div*div +div/2;
        (*it)[2] = (*it)[2]/div*div +div/2;
    }


}


void MainWindow::sharpen(cv::Mat &image,cv::Mat &result){

    // all rows except first and last
    for (int j=1; j<image.rows-1; j++){
        const uchar* previous = image.ptr<const uchar>(j-1);
        const uchar* current = image.ptr<const uchar>(j);
        const uchar* next = image.ptr<const uchar>(j+1);

        uchar* output = result.ptr<uchar>(j);

        for (int i=1; i<image.cols-1;i++){
            *output ++= cv::saturate_cast<uchar>(5*current[i]-current[i-1]-current[i+1]-previous[i]-next[i]);
        }
    }

    // border pixels
    result.row(0).setTo(cv::Scalar(0));
    result.row(result.rows-1).setTo(cv::Scalar(0));
    result.col(0).setTo(cv::Scalar(0));
    result.col(result.cols-1).setTo(cv::Scalar(0));

}


void MainWindow::sharpen2D(cv::Mat &image,cv::Mat &result){

    cv::Mat kernel(3,3,CV_32F,cv::Scalar(0));
    kernel.at<float>(1,1) = 5.0;
    kernel.at<float>(0,1) = -1.0;
    kernel.at<float>(2,1) = -1.0;
    kernel.at<float>(1,1) = -1.0;
    kernel.at<float>(1,2) = -1.0;

}


void MainWindow::displayImage(cv::Mat &image){
    QImage img ((const unsigned char*)(image.data),
                image.cols, image.rows, QImage::Format_RGB888);

    //display on label
    ui->label->setPixmap(QPixmap::fromImage(img));

    //resize label
    ui->label->resize(ui->label->pixmap()->size());
}






