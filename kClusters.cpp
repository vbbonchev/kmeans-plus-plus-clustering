#include<iostream>
#include<fstream>
#include<string>
#include<math.h>
#include<windows.h>
#include<stdio.h>
#include "cimg/CImg.h"
#include<cstdlib>
#include<time.h>
#define NUM_CLUSTERS 4
#define width 1200
#define heigth 2400
using namespace cimg_library;
using namespace std;

//CImg изисква windows библиотека gdi32 да е добавена в линкер-а

struct point{
double X;
double Y;
int assignedCluster;
double propability;//изисква се за kmeans++
};

//глобални променливи за точките и клъстърите
point points[10000],clusters[10];
int clustersSize=NUM_CLUSTERS;
int pointsCounter=0;


void visualise()
{
    CImg<unsigned char> visu(heigth,width,1,3,127);
    visu.fill(55);
    for(int i=0;i<pointsCounter;++i){
    int cluster=points[i].assignedCluster+1;

    //оцветяване в 4 цвята за по-разбираема картинка
//   int R,G,B;
//   if(cluster==1){R=255;G=0;B=0;}
//   if(cluster==2){R=0;G=255;B=0;}
//   if(cluster==3){R=0;G=0;B=0;}
//   if(cluster==4){R=0;G=0;B=255;}

    int color[]={cluster*(250/clustersSize),255-cluster*(250/clustersSize),cluster*(250/clustersSize)};
    visu.draw_circle(points[i].X,points[i].Y,1,color);
    }
    int clusterColor[]={255,255,255};
    for(int j=0;j<clustersSize;++j)visu.draw_rectangle(clusters[j].X-2,clusters[j].Y-2,clusters[j].X+2,clusters[j].Y+2,clusterColor);
    // visu.save_magick("clustersResult.bmp"); //изисква библиотека magick++ да е инсталирана на компютъра
    visu.display();
}

// избира място на клъстера clusterNum според kmeans++ алгоритъма
void kmeans_plus_plus(int clusterNum)
{
    //ако е първия клъстър избираме произволна точка
    if(clusterNum==0){
        int chosenPoint=rand()%pointsCounter;
        clusters[clusterNum].X=points[chosenPoint].X;
        clusters[clusterNum].Y=points[chosenPoint].Y;
        return;
    }
    //ако не е първия клъстър на всяка точка задаваме вероятност за избиране
    //равна на квадрата на дистанцията до най-близкия клъстър от вече създадените
    else{
        long double sumOfAllSqDistances=0;

        for(int i=0;i<pointsCounter;i++){
            double closestClusterDist=99999999.0;
            for(int j=0;j<clusterNum;j++){
                double newClustDist=sqrt(pow(points[i].X
                                             - clusters[j].X,2) +
                                         pow(points[i].Y
                                             - clusters[j].Y,2));
                if(newClustDist<closestClusterDist){
                    closestClusterDist=newClustDist;
                }
            }
            points[i].propability = pow(closestClusterDist,2);
            sumOfAllSqDistances+=pow(closestClusterDist,2);
            //cout<<"for point "<<i<<": propability of picking - "<<pow(closestClusterDist,2)<<endl;
        }
    // cout<<"sumOfAllSqDistances so far:"<<sumOfAllSqDistances<<endl;


    //тъй като всяка точка се избира вероятност дистанция/(общата дистанция)
    //за да е правилно избирането намираме число между 1 и общата дистанция
    //и пускаме алгоритъм, който на всяка стъпка увеличава итератора с дистанцията на текущата точка
    //по този начин осигуряваме, че всяка точка се избира с равна вероятност
    srand(time(NULL));
    double coefficient=(double)rand()/RAND_MAX; //число между 0 и 1
    double chosenNumber=sumOfAllSqDistances*coefficient;
    double currentNumber=0;
    int currentPoint=0;
    while(currentNumber<=chosenNumber){
        currentNumber+=points[currentPoint].propability;
        currentPoint++;
        //cout<<"chosen number was: "<<chosenNumber<<" ,currently at "<<currentNumber<<" which is point "<<currentPoint<<endl;
        //Sleep(50);
    }
    //cout<<"picked point: "<<currentPoint<<" with propability of: "<<points[currentPoint].propability<<endl;
   // cout<<"sumofallsqdistances was:" <<sumOfAllSqDistances<<endl;

    clusters[clusterNum].X=points[currentPoint].X;
    clusters[clusterNum].Y=points[currentPoint].Y;
    }
    //visualise();

    return;
}


int main() {
    string line;
    ifstream myfile("unbalance.txt");


    //вкарваме данните от файл
    bool flag=true;
    if (myfile.is_open()){
        while ( getline (myfile,line))
        {
          double x=stod(line)*20;
          points[pointsCounter].X= x;
          size_t pos=line.find(' ');//трябва да се промени разделителя на ' ' за файл unbalance.txt
          if(pos==string::npos ){cout<<"Greshka! Koordinatite ne sa razdeleni s '/t' - razdelitelq trqbva da se smeni"<<endl;return 0;}
          string yCoords = line.substr(pos);
          double y=stod(yCoords)*20;
          //y=width-y;
          points[pointsCounter].Y= y;
          pointsCounter++;
        }
    }

    //намираме начални позиции за клъстърите според алгоритъма kmeans
    //int iterationsCounter=1;
    for(int i=0;i<clustersSize;++i){
        kmeans_plus_plus(i);
        //cout<<"cluster made at: "<<clusters[i].X<<" "<<clusters[i].Y<<endl;
    }
    while(flag){

        //намираме най-близкия клъстър за всяка точка
        for(int i=0;i<pointsCounter;++i){
            double closestClusterDist=999999.0;
            for(int j=0;j<clustersSize;j++){
                double newClustDist=sqrt(pow(points[i].X
                                             - clusters[j].X,2) +
                                         pow(points[i].Y
                                             - clusters[j].Y,2));
                if(newClustDist<closestClusterDist){
                    points[i].assignedCluster=j;
                    closestClusterDist=newClustDist;
                   //cout<<"point "<<i<<" was assigned cluster "<<j<<" with distance "<<newClustDist<<endl;
                    //Sleep(100);
                }
            }
        }
        flag=false;

        //намираме нови позиции за клъстърите (и вдигаме флага за продължаване на цикъла, ако се премести някой от тях)
        for(int j=0;j<clustersSize;j++){
            double xSum=0,ySum=0;
            int pointsCount=0;
            for(int i=0;i<pointsCounter;i++)if(points[i].assignedCluster==j){
                                                xSum+=points[i].X;
                                                ySum+=points[i].Y;
                                                pointsCount++;
                                                }
            double avgX=xSum/pointsCount;
            double avgY=ySum/pointsCount;

            if(avgX!=clusters[j].X || avgY!=clusters[j].Y){
                flag=true;
            }

            if(pointsCount>0){
                clusters[j].X=avgX;
                clusters[j].Y=avgY;
            }
        }
        //cout<<"Iteration num "<<iterationsCounter<<" complete"<<endl;
        //cout<<"cluster coords are: ";
        //for(int i=0;i<clustersSize;i++)cout<<"("<<clusters[i].X<<" "<<clusters[i].Y<<")  ";
        //sleep(10);
        //cout<<endl;
        // iterationsCounter++;
        visualise();

    }

      //запис във файл
//    ofstream outfile("normalClusters.txt",ios::trunc);
//    for(int j=0;j<clustersSize;j++){
//        outfile<<"Cluster number "<<j+1<<endl;
//        outfile<<"---------------------------------------------------------------------"<<endl;
//        for(int i=0;i<pointsCounter;i++)if(points[i].assignedCluster==j)outfile<<(width-points[i].X)/20<<" "<<points[i].Y/20<<endl;
//        }
}

