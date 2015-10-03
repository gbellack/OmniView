#include "openbr/openbr_plugin.h"
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
int main(int argc, char *argv[]) {
    br::Context::initialize(argc, argv);

    // Retrieve classes for enrolling and comparing templates using the FaceRecognition algorithm
    //QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
    QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
    QSharedPointer<br::Distance> distance = br::Distance::fromAlgorithm("FaceRecognition");

    // Initialize templates
    br::TemplateList target = br::TemplateList::fromGallery("testPics");
    cout << "Input pic name:" << endl;
    string pic_name;
    cin >> pic_name;
    br::Template query(pic_name.c_str());
    br::TemplateList queryList;
    queryList.push_back(query);

    cout << "before" << queryList.size() << endl;
    
    // Enroll templates
    br::Globals->enrollAll = true; // Enroll 0 or more faces per image
    target >> *transform;
    br::Globals->enrollAll = true; // Enroll exactly one face per image
    queryList >> *transform;

    cout << "after " << queryList.size() << endl;

    // if(queryList[0] == queryList[1]){
    //     cout << "true" << endl;
    // }
    // else{
    //     cout << "false" << endl;
    // }
    // Compare templates

    for(int i = 0; i < queryList.size(); i++){
        QList<float> scores = distance->compare(target, queryList[i]);
        for (int j = 0; j < scores.size(); j++) {
        // Print an example score
            printf("Images %s and %s have a match score of %.3f\n",
                   qPrintable(target[j].file.name),
                   qPrintable(queryList[i].file.name),
                   scores[j]);

        }
        cout << endl;
    }
    br::Context::finalize();
    return 0;
}
