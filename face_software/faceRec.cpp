#include "openbr/openbr_plugin.h"

#include "openbr/plugins/openbr_internal.h"
#include <QImage>
#include <QString>
#include <QColor>
#include <QIODevice>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <math.h>
#include <boost/timer.hpp>
#include <time.h>

#include <tcpClient.h>

#define LOWER_BOUND 0.5
#define max_size 100
using namespace std;
using namespace br;


/******************* TO DO******************

1) Adds name of people it does not recognize

*////////////////////////////////////////

string file_glob;
int sock; 
int mode;



void extractName(double score, QString file_input){
    if(score >= LOWER_BOUND){
        string file = file_input.toStdString();
        int first = 0;
        int second = 0;

        for(int i = file.size() - 1; i >= 0 ; i--){
            if(file[i] == '/'){
                if (first == 0){
                    first = i;
                }
                else{
                    second = i;
                    break;
                }
            }
        }

        cout << "Face recognized as: " << file.substr(second+1, first-second-1) << endl;
        cout << endl;

        sendString(sock,file.substr(second+1, first-second-1));
    }
    else{
        cout << "Unable to recognize face" << endl;
        sendString(sock,"Unable to recognize face");
    }

}


int calcDistance(const br::Template &t)
{
    const QPoint firstEye = t.file.get<QPoint>("Affine_0");

    QRect face_glob = t.file.get<QRect>("FrontalFace");
    QPoint topLeft = face_glob.topLeft();
    QPoint bottomRight = face_glob.bottomRight();

    int top_x = topLeft.x();
    int top_y = topLeft.y();

    int bottom_x = bottomRight.x();
    int bottom_y = bottomRight.y();

    int mid_x = (top_x + bottom_x) / 2;
    int mid_y = (top_y + bottom_y) / 2;

    return abs(firstEye.x() - mid_x) + abs(firstEye.y()-mid_y);

}

void blackblob(const br::Template &t){
    QRect face_glob = t.file.get<QRect>("FrontalFace");
    QPoint topLeft = face_glob.topLeft();
    QPoint bottomRight = face_glob.bottomRight();

    int top_x = topLeft.x();
    int top_y = topLeft.y();

    int bottom_x = bottomRight.x();
    int bottom_y = bottomRight.y();

    cout << "top x =" << top_x << ", top y = " << top_y << endl;
    cout << "bottom x =" << bottom_x << ", bottom y = " << bottom_y << endl;
    const QString fileName = QString::fromStdString(file_glob);
    cout << "fileName = " << fileName.toStdString() <<endl;
    QImage image(fileName);
    QRgb value;
    value = qRgb(0,0,0);
    //loop through x
    for(double i = top_x ; i < bottom_x; i++){
        //loop through y
        for(int j = top_y; j < bottom_y; j++){
            //cout << "loop";

            //cout << "value = " << value.rgb() << endl;
            image.setPixel(i,j,value);
        }
    }

    image.save(fileName, 0,100); // writes image into ba in PNG format
}


int main(int argc, char *argv[]) {
    br::Context::initialize(argc, argv);

    // Retrieve classes for enrolling and comparing templates using the FaceRecognition algorithm
    QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
    QSharedPointer<br::Distance> distance = br::Distance::fromAlgorithm("FaceRecognition");

    //enroll gallery
    br::TemplateList target = br::TemplateList::fromGallery("training");
    br::Globals->enrollAll = false; //Enroll exactly one face per image 
    target >> *transform;
    //cout << "Input pic name:" << endl;
    //cin >> file_glob;
    mode = 0;

    //Init socket
    initSocket(sock);

    while(1){
        recvImage(sock);
        cout << "finished receiving image" << endl;

        //image detection mode
        if(mode == 0){
            cout << "in image mode" << endl;
            // double elapsed_time1 = t1.elapsed();
            //cout << " diff time in seconds: " << elapsed_time1 << endl;

            //boost::timer t;
            //Initialize test templates
            file_glob = "query.jpg";
            br::Template query(file_glob.c_str());
            br::TemplateList queryList;
            queryList.push_back(query);

            cout << "before transform: " << queryList.size() << endl;
            

            // Enroll target templates
            br::Globals->enrollAll = true; // Enroll 0 or more faces per image
            queryList >> *transform;


            cout << "after transform: " << queryList.size() << endl;

            /*
            //if multiple faces
            if(queryList.size() > 1){
                string comms = "convert query.jpg query.png"
                system(comms.c_str());
                file_glob = "query.png";
                br::Template query(file_glob.c_str());
                queryList.pop_front();
                queryList.push_back(query);
            }
            */

            double best_score = -100;
            QString filepath;
            if( queryList.size() >= 1 ){

                // Compare first face
                QList<float> scores = distance->compare(target, queryList[0]);
                for (int j = 0; j < scores.size(); j++) {
                // Print an example score
                    // printf("Images %s and %s have a match score of %.3f\n",
                    //        qPrintable(target[j].file.name),
                    //        qPrintable(queryList[0].file.name),
                    //        scores[j]);
                    if(scores[j] > best_score){
                        best_score = scores[j];
                        filepath = target[j].file.name;
                    }

                }
                extractName(best_score, filepath);
            }
            else{
                cout << "No face detected" << endl;
                sendString(sock,"No face detected");
            }

            //double elapsed_time = t.elapsed();
            //cout << " diff time in seconds: " << elapsed_time << endl;

            int size = queryList.size();
            //enter loop if there are multiple faces to loop through.
            for(int z = 1; z< size; z++){
                /*
                //remove face that was just compared
                if(queryList.size() > 1){

                    int template_number = 0;
                    int smallest = calcDistance(queryList[0]);
                    //since templates contain same eyes but different face,
                    // need to find face associated with the same pair of eyes to remove
                    for(int i =1; i < queryList.size(); i++){
                        int dis = calcDistance(queryList[i]);
                        if(dis < smallest){
                            smallest = dis;
                            template_number = i;
                        }
                    }
                    cout << "template_number: " << template_number << endl;
                    blackblob(queryList[template_number]);
                }

                //enroll new image with removed face
                br::Template query(file_glob.c_str());
                queryList.clear();
                queryList.push_back(query);

                cout << "before" << queryList.size() << endl;


                // Enroll templates
                br::Globals->enrollAll = false; // Enroll 0 or more faces per image
                target >> *transform;
                br::Globals->enrollAll = true; // Enroll exactly one face per image
                queryList >> *transform;

                cout << "after " << queryList.size() << endl;


                // Compare templates
                best_score = -100;

                QList<float> scores = distance->compare(target, queryList[0]);
                for (int j = 0; j < scores.size(); j++) {
                // Print an example score
                    printf("Images %s and %s have a match score of %.3f\n",
                           qPrintable(target[j].file.name),
                           qPrintable(queryList[0].file.name),
                           scores[j]);
                    if(scores[j] > best_score){
                        best_score = scores[j];
                        filepath = target[j].file.name;
                    }
                }
                extractName(best_score,filepath);
                */
            }

        }
        //mic/speech mode
        else{
            FILE *fp;
            char name[max_size];
            int status;

            //convert raw file to wav
            string com1 = "sox sound.raw sound.wav";
            system(com1.c_str());

            //calls python script to find name -> name stored into name
            fp = popen("python ./wav_transcribe.py", "r");

            while (fgets(name, max_size, fp) != NULL)
                //printf("%s", name);

            status = pclose(fp);
            

            //check if person exists
            //creates new folder for that person
            string folder = "/home/loren/Docuements/Mich/eecs473/OmniView/face_software/training/";
            folder.append(name);
            
            
            string command = "mkdir ";
            command.append(folder);
            system(command.c_str());

            //receives picture
            recvImage(sock);

            //renames pic and inserts into folder
            command = "mv query.jpg ";
            command.append(folder);
            command.append("0.jpg");
            system(command.c_str());

            //reenroll gallery
            target = br::TemplateList::fromGallery("training");
            br::Globals->enrollAll = false; //Enroll exactly one face per image 
            target >> *transform;

        }

    }
    br::Context::finalize();
    return 0;
}