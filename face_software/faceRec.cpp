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
#include <deque>
#include <unordered_map>

#include <tcpClient.h>

#define LOWER_BOUND 0.5
#define max_size 100
#define buffer 1
using namespace std;
using namespace br;


/*************************************

                  _,.--''''''  -    ''`-...__
             /''___...---    -   ---..    `.._
           .,--'              `-  __  `-.     .
         .'/     ___..--------  -   ''---'-.   `.
         /J  ,-''         __,.   -   .._  `-'-. `\
       ,//  |     _..--'''    _         '-._|  `  \
      / |  ,'  ,-'      _..--'    -  _____ `\|  '  `.
    ,'  /    ,'  ,  ,.-'   _.-            '-'|    | \
    '  / .','   / ,-    _,'  :  :      -.    '.|  |  \
      |  /'    //'    /'   . /  | \  \   \    \+   |  |
      | //   .'/     |     |.'  | |   |  '.    +\  |  \
      ||/    J|      /    / |   | '.  '.  |    '.\ \   \
      |||   | |     |   ,'  |   |  \   \   |   || ||   |
      ||    J J    ,'   |  /_   |  |   |  ,|    |   |  |
      ||   | |    .'|. .||/'|   | / |  + |\'     |  |  |
      ||   | |  . | \ ./ |,'|/'|'/\  _'|,'| /  / |  |  L
      ||   | | |'.| |'--..._`  | /| ,' |'_,.. -  || |   |
      /L   | / | \|,.....  -'\ |/ |/  . .....    |\ |  .'
      | |  | | |   `._(  )\           /(  )_.'   |  |  /
     | .\  J | \      '''''           '````     /'   .|
     |  | |  |  |                              .|  ,.'|
     |  |.'. | _\.                             /  / | |
     || || |  \'\:                            /   | / |
      +L||-\...__ -._                        / /  | | |
    | '||.+ \ _  '`-.:._     _     _        /'|   J|  J
    |  \\ +` \ -.     `       `...'        ,' |  | | |
    |  || |.  .         _ _               /'  |  | | |
    |  '.  |   \          \''------;'   ,'.'  |  | | |
     |  || |\   `._        '-.._..'   ,/  |   |  / | L
     \  ||  |\     '-.               ,|'' |   |  | | '|
      | '.| \ `\      `             J ||  |   |  | |  |
      \  ||  |  \       '..      ,-'  /|  |   |  | |  L.
      '. ||  `    -.       '''`-' |   '|  |   /  | |   |
       | | |  '-..  `.            '''-'\ ._....._| |   |
       | | '.         `                ||_ `.     ' . |


 ||||||||||||||||||||\
|||||||||||||||||||\\\\
|||/////////        \||
||/////   _____   ___\
||||||   / ___   /___/
|/   ||   /_(_) |/(_)
 | |  |          \  |
 |                \ /
 \__|   /      /___\
   |   /|\________\
   |    |\________|
   |    |      ||  
   |    |  ____||___
   |    | /   __    \
   |    \/___/__\    \
   |   \________/\___/
   |            |

*////////////////////////////////////////

string file_glob;
int sock; 
int mode;

deque <string> string_buf;

string majority(string new_val){
    if(string_buf.size() < 4)
        string_buf.push_front(new_val);

    else{
        string_buf.pop_back();
        string_buf.push_front(new_val);    
    }

    //records nums
    unordered_map<string,double> my_map;

    for(int i = 0; i < string_buf.size(); i++){
        string tmp = string_buf[i];

        //no occurrence
        auto it = my_map.find(tmp); 
        if(it == my_map.end()){
            my_map.insert({tmp,0});
        }
        else{
            it->second++; 
        }
    }
    //finds max num if No face detected is not the most

    if(my_map.find("No Face Detected") != my_map.end()){
        if(my_map.find("No Face Detected")->second == 3){
            return "No Face Detected";
        }
        return new_val;
    }
    else{
        int max_num = 0;
        string max_name = "No Face Detected";
        string sec_name = "No Face Detected";

        for(auto it = my_map.begin(); it != my_map.end(); it++){
            if(it->second > max_num){
                max_name = it->first;
                max_num = it->second;
                sec_name = max_name;
            }
        }

        if(max_name == "No Face Detected"){
            return sec_name;
        }
        else
            return max_name;
    }
}   




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


        if(buffer == true){
            string ret = majority(file.substr(second+1, first-second-1));
            cout << "ret string: " << ret << endl;
            sendString(sock,ret);
        }
        else{
            sendString(sock,file.substr(second+1, first-second-1));
        }
    }
    else{
        cout << "No Face Detected" << endl;

        if(buffer == true){
            string ret = majority("No Face Detected");
            cout << "ret string: " << ret << endl;
            sendString(sock,ret);
        }
        else{
            sendString(sock,"No Face Detected");
        }
        
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
            image.setPixel(i,j,value);
        }
    }

    image.save(fileName, 0,100); // writes image into ba in PNG format
}

/*
///////////////////////******************MAIN*****************\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
                ___           ___         __                      ___     ___
                |||\         /|||        //\\        ====##====   |||\    |||
                |||\\       //|||       //  \\           ||       |||\\   |||
                ||| \\     // |||      //    \\          ||       ||| \\  |||
                |||  \\   //  |||     //______\\         ||       |||  \\ |||
                |||   \\ //   |||    //========\\        ||       |||   \\|||
                |||    \v/    |||   //          \\   ====##====   |||    \|||
                    
*///////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]) {
    br::Context::initialize(argc, argv);

    // Retrieve classes for enrolling and comparing templates using the FaceRecognition algorithm
    QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
    QSharedPointer<br::Distance> distance = br::Distance::fromAlgorithm("FaceRecognition");

    //enroll gallery
    br::TemplateList target = br::TemplateList::fromGallery("training");
    br::Globals->enrollAll = false; //Enroll exactly one face per image 
    target >> *transform;
    mode = 0;

    //Init socket
    initSocket(sock);

    while(1){
        mode = recvMain(sock);
        cout << "finished receiving image" << endl;
        string stringToSend;


        //image detection mode
        if(mode == 1){

            cout << "in image mode" << endl;
            //Initialize test templates
            file_glob = "outquery.jpg";
            string command = "convert query.jpg -background black -distort ScaleRotateTranslate -90 outquery.jpg ";
            system(command.c_str());


            br::Template query(file_glob.c_str());
            br::TemplateList queryList;
            queryList.push_back(query);

            cout << "before transform: " << queryList.size() << endl;
            // Enroll target templates
            br::Globals->enrollAll = true; // Enroll 0 or more faces per image
            queryList >> *transform;
            cout << "after transform: " << queryList.size() << endl;

            double best_score = -100;
            QString filepath;
            if( queryList.size() >= 1 ){
                // Compare first face
                QList<float> scores = distance->compare(target, queryList[0]);
                for (int j = 0; j < scores.size(); j++) {
                    if(scores[j] > best_score){
                        best_score = scores[j];
                        filepath = target[j].file.name;
                    }
                }
                extractName(best_score, filepath);
            }
            else{
                string ret = majority("No Face Detected");
                cout << "ret string: " << ret << endl;
                if(buffer == true){
                    sendString(sock,ret);
                }
                else{
                    sendString(sock,"No Face Detected");
                }
            }

        }
        //mic/speech mode
        else if (mode == 2){
            cout << "mic mode" << endl;
            FILE *fp;
            char name[max_size];
            int status;

            //convert raw file to wav
            string com1 = "sox -r 10000 -e signed-integer -b 16 sound.raw sound.wav";
            system(com1.c_str());

            //calls python script to find name -> name stored into name
            
            fp = popen("python ./wav_transcribe.py", "r");

            while (fgets(name, max_size, fp) != NULL)
                //cout << "error: no return" << endl;

            status = pclose(fp);
            cout << "name is: " << name << endl;
            if(name == NULL){
                sendString(sock, "No name found");
            }
            else{
                //check if person exists
                //creates new folder for that person
                string folder = "/home/loren/Documents/Mich/eecs473/OmniView/face_software/training/";
                folder.append(name);
                
                
                string command = "mkdir ";
                command.append(folder);
                system(command.c_str());

                //renames pic and inserts into folder
                string rotate = "convert add.jpg -background black -distort ScaleRotateTranslate -90 outadd.jpg ";
                system(rotate.c_str());

                command = "mv outadd.jpg ";
                command.append(folder);
                command.append("/pic");
                command.append("0.jpg");
                system(command.c_str());

                //reenroll gallery
                target = br::TemplateList::fromGallery("training");
                br::Globals->enrollAll = false; //Enroll exactly one face per image 
                target >> *transform;
            }
        }
        else{
            cout << "in error mode" << endl; 
            sendString(sock,"In Error Mode");
        }
    }
    br::Context::finalize();
    return 0;
}