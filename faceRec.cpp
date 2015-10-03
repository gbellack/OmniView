#include "openbr/openbr_plugin.h"
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
int main(int argc, char *argv[])
{
    br::Context::initialize(argc, argv);

    // Retrieve classes for enrolling and comparing templates using the FaceRecognition algorithm
    QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
    QSharedPointer<br::Distance> distance = br::Distance::fromAlgorithm("FaceRecognition");

    // Initialize templates
    br::TemplateList target = br::TemplateList::fromGallery("testPics");
    cout << "Input pic name:" << endl;
    string pic_name;
    cin >> pic_name;
    br::Template query(pic_name.c_str());
    // Enroll templates
    br::Globals->enrollAll = true; // Enroll 0 or more faces per image
    target >> *transform;
    br::Globals->enrollAll = false; // Enroll exactly one face per image
    query >> *transform;

    // Compare templates
    QList<float> scores = distance->compare(target, query);
    for(int i = 0; i < scores.size(); i++ ){
    // Print an example score
        printf("Images %s and %s have a match score of %.3f\n",
               qPrintable(target[i].file.name),
               qPrintable(query.file.name),
               scores[i]);
    }
    br::Context::finalize();
    return 0;
}
