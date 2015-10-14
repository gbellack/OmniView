#include "openbr/openbr_plugin.h"

#include "openbr/plugins/openbr_internal.h"
#include <QImage>
#include <QString>
#include <QColor>
#include <QIODevice>

#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>

using namespace std;
using namespace br;


/******************* TO DO******************

1)Change for jpeg? depending on camera's ability to format
2)finish blobbing

*////////////////////////////////////////

string file_glob;

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
    for(int i = top_x ; i < bottom_x; i ++){
        //loop through y
        for(int j = top_y; j < bottom_y; j++){
            //cout << "loop";

            //cout << "value = " << value.rgb() << endl;
            image.setPixel(i,j,value);
        }
    }

    image.save(fileName, 0,100); // writes image into ba in PNG format


}

void define (){
    // Globals->abbreviations.insert("FaceRecognition2", "FaceRecognition_1_12");


    // Globals->abbreviations.insert("FaceRecognition_1_12", "FR_Detect2+(FR_Eyes+FR_Represent)/(FR_Eyebrows+FR_Represent)/(FR_Mouth+FR_Represent)/(FR_Nose+FR_Represent)/(FR_Face+FR_Represent+ScaleMat(2.0))+Cat+LDA(768)+Normalize(L2):Unit(Dist(L2))");
    // Globals->abbreviations.insert("FR_Detect2", "(FaceDetection2+ASEFEyes+Affine(136,136,0.35,0.35,warpPoints=true))");
    // Globals->abbreviations.insert("FaceDetection2", "Open+Cvt(Gray)");
    

}
//dunno if necessary
void train(){


    const QString trainedModelFile = "FaceRecognitionLab";
    //Globals->algorithm = "FR_Detect+(FR_Eyes+FR_Represent)/(FR_Eyebrows+FR_Represent)/(FR_Mouth+FR_Represent)/(FR_Nose+FR_Represent)/(FR_Face+FR_Represent+ScaleMat(2.0))+Cat+LDA(768)+Normalize(L2):Unit(Dist(L2))";
    Globals->algorithm = "FaceRecognition";
    if (!QFile(trainedModelFile).exists()){
         const QString trainingData = "data/training";
        printf("Note: Training will take at least a few minutes to complete.\n");
        br::Train(trainingData, trainedModelFile);
    }
    cout << "done training" << endl;
}



int main(int argc, char *argv[]) {
    br::Context::initialize(argc, argv);

    train();

    // Retrieve classes for enrolling and comparing templates using the FaceRecognition algorithm
    //QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
    
    QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
    QSharedPointer<br::Distance> distance = br::Distance::fromAlgorithm("FaceRecognition");


    // Initialize templates
    br::TemplateList target = br::TemplateList::fromGallery("testPics");
    cout << "Input pic name:" << endl;
    cin >> file_glob;
    br::Template query(file_glob.c_str());
    br::TemplateList queryList;
    queryList.push_back(query);

    cout << "before" << queryList.size() << endl;
    

    // Enroll templates
    br::Globals->enrollAll = false; // Enroll 0 or more faces per image
    target >> *transform;
    br::Globals->enrollAll = true; // Enroll exactly one face per image
    queryList >> *transform;

    cout << "after " << queryList.size() << endl;

    // Compare first face
    QList<float> scores = distance->compare(target, queryList[0]);
    for (int j = 0; j < scores.size(); j++) {
    // Print an example score
        printf("Images %s and %s have a match score of %.3f\n",
               qPrintable(target[j].file.name),
               qPrintable(queryList[0].file.name),
               scores[j]);

    }
    cout << endl;
    int size = queryList.size();

    //enter loop if there are multiple faces to loop through.
    for(int z = 1; z< size; z++){

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
        QList<float> scores = distance->compare(target, queryList[0]);
        for (int j = 0; j < scores.size(); j++) {
        // Print an example score
            printf("Images %s and %s have a match score of %.3f\n",
                   qPrintable(target[j].file.name),
                   qPrintable(queryList[0].file.name),
                   scores[j]);

        }
        cout << endl;
        

    }
    br::Context::finalize();
    return 0;
}
