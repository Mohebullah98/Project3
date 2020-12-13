#include <iostream>
#include <fstream>
using namespace std;

class imageProcessing{
  public:
  int numRows;
  int numCols;
  int minVal;
  int maxVal;
  int newMin;
  int newMax;
  int maskRows;
  int maskCols;
  int maskMin;
  int maskMax;
  int ** mirrorFramedAry;
  int ** avgAry;
  int ** medianAry;
  int ** GaussAry;
  int ** maskAry;
  
  
  void loadImage(int ** mirrorFramedAry, ifstream& inFile){
    for(int i=1;i<numRows+1;i++){
      for(int j=1;j<numCols+1;j++){
        inFile>>mirrorFramedAry[i][j];
      }
    }
  }
  void loadMask(int ** maskAry, ifstream& maskFile){
    for(int i=0;i<maskRows;i++){
      for(int j=0;j<maskCols;j++){
        maskFile>>maskAry[i][j];
      }
    }
  }
  void loadNeighbors(int i, int j, int neighborAry[]){
    int counter = 0;
    for(int a=i-1;a<i+2;a++){
      for(int b=j-1;b<j+2;b++){
        neighborAry[counter]= mirrorFramedAry[a][b]; 
        counter++;
      }
    }
  }
  void computeAvgImg (int ** avgAry, int& newMin, int& newMax){
    newMin=9999;
    newMax = 0;
    for(int i=1;i<numRows+1;i++){
      for(int j=1;j<numCols+1;j++){
        avgAry[i][j] = avg3x3(i,j,avgAry);
        if(newMin>avgAry[i][j]) newMin= avgAry[i][j];
        if(newMax<avgAry[i][j]) newMax = avgAry[i][j];
      }
    }
  }
  void computemedianImg(int ** medianAry, int& newMin, int& newMax){
    int neighborAry[9]={};
    newMin=9999;
    newMax=0;
    for(int i=1;i<numRows+1;i++){
      for(int j=1;j<numCols+1;j++){
        loadNeighbors(i,j,neighborAry);
        sort(neighborAry);
        medianAry[i][j]=neighborAry[4];
        if(newMin>medianAry[i][j]) newMin = medianAry[i][j];
        if(newMax<medianAry[i][j]) newMax = medianAry[i][j];
      }
    }
  }
  void computeGaussianImg(int ** GaussAry,int& newMin, int& newMax ){
    newMin=9999;
    newMax=0;
    for(int i=1;i<numRows+1;i++){
      for(int j=1;j<numCols+1;j++){
        GaussAry[i][j] = convolution(i,j,GaussAry,maskAry);
        if(newMin>GaussAry[i][j]) newMin = GaussAry[i][j];
        if(newMax<GaussAry[i][j]) newMax = GaussAry[i][j];
      }
    }
  }
 int avg3x3(int i, int j, int ** avgAry){
    int total=0;
    int neighbors[9]={};
    loadNeighbors(i,j,neighbors);
    for(int a=0;a<9;a++){
      total+=neighbors[a];
    } 
    return (total/9);
  }
  int median3x3(int i,int j, int ** medianAry){
    int neighborAry[9]={};
    loadNeighbors(i,j,neighborAry);
    sort(neighborAry);
    return neighborAry[5];  
  }

  void swap(int *x, int*y){
    int temp = *x;
    *x = *y;
    *y=temp;
  }
  void sort(int neighborAry[]){
    for(int i=0;i<8;i++){
      for(int j=0;j<9-i-1;j++){
        if(neighborAry[j]>neighborAry[j+1]) swap(&neighborAry[j], &neighborAry[j+1]);
      }
    }
  }
  int convolution(int i, int j, int **GaussAry,int ** maskAry){
    
    int result=0;
    int count = 0;
    int count2=0;
    int weighted_total=0;
    for(int a=i-1;a<i+2;a++){
      for(int b=j-1;b<j+2;b++){
        result+=(mirrorFramedAry[a][b] * maskAry[count][count2]);
        weighted_total+=maskAry[count][count2];
        count2++;
      }count2=0;
      count++;
    }
    return (result/weighted_total);
  }
  void outputAryToFile(int ** ary,ofstream& outFile,int newMin, int newMax){
    outFile<<numRows<<" "<<numCols<<" "<<newMin<<" "<<newMax;
    outFile<<endl;
    for(int i=1;i<numRows+1;i++){
      for(int j=1;j<numCols+1;j++){
        outFile<<ary[i][j]<<" ";
      }
      outFile<<endl;
    }
  }
  void mirrorFraming(int ** mirrorFramedAry){
    for(int i=0;i<numRows+2;i++){
      mirrorFramedAry[i][0]=mirrorFramedAry[i][1];
      mirrorFramedAry[i][numCols+1]=mirrorFramedAry[i][numCols];
    }
    for(int j=0;j<numCols+2;j++){
      mirrorFramedAry[0][j]= mirrorFramedAry[1][j];
      mirrorFramedAry[numRows+1][j]=mirrorFramedAry[numRows][j];
    }
  }
};

int main(int argc, char* argv[]) {
  ifstream inFile;
  inFile.open(argv[1]);
  ifstream maskFile;
  maskFile.open(argv[2]);
  
  ofstream AvgOutImg;
  ofstream MedianOutImg;
  ofstream GaussOutImg;
  AvgOutImg.open(argv[3]);
  MedianOutImg.open(argv[4]);
  GaussOutImg.open(argv[5]);
  imageProcessing image;

  inFile>>image.numRows>>image.numCols>>image.newMin>>image.newMax;
  maskFile>>image.maskRows>>image.maskCols>>image.maskMin>>image.maskMax;
  
   image.mirrorFramedAry = new int*[image.numRows+2]();
   image.avgAry = new int*[image.numRows+2]();
   image.medianAry = new int*[image.numRows+2]();
   image.GaussAry = new int*[image.numRows+2]();
   image.maskAry = new int*[image.maskRows]();
   for(int i=0;i<image.numRows+2;i++){
     image.mirrorFramedAry[i]= new int[image.numCols+2] ();
     image.avgAry[i] = new int[image.numCols+2]();
     image.medianAry[i]= new int[image.numCols+2]();
     image.GaussAry[i]= new int[image.numCols+2]();
   }
   for(int i=0;i<image.maskRows;i++){
     image.maskAry[i]= new int[image.maskCols]();
   }
    
   image.loadMask (image.maskAry, maskFile);
   image.loadImage(image.mirrorFramedAry, inFile);
   image.mirrorFraming(image.mirrorFramedAry);
   image.computeAvgImg(image.avgAry, image.newMin, image.newMax);
   image.outputAryToFile(image.avgAry, AvgOutImg,  image.newMin,image.newMax);
   image.computemedianImg(image.medianAry,image.newMin,image.newMax);
   image.outputAryToFile(image.medianAry,MedianOutImg,image.newMin,image.newMax);
   image.computeGaussianImg(image.GaussAry,image.newMin, image.newMax);
   image.outputAryToFile(image.GaussAry,GaussOutImg,image.newMin,image.newMax);
   inFile.close();
   maskFile.close();
   AvgOutImg.close();
   MedianOutImg.close();
   GaussOutImg.close();
}