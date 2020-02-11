#include "line2polygon.h"
#include "ui_line2polygon.h"
#include <QFileDialog>
#include <QFile>
#include <string.h>
#include <stdio.h>
#include <QtDebug>
#include <QGeoCoordinate>
#include <QtMath>
#include <QtGlobal>

line2polygon::line2polygon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::line2polygon)
{
    ui->setupUi(this);
}

line2polygon::~line2polygon()
{
    delete ui;
}

double getDist(double times, double depth){

    double dist = times / 2;

    dist = abs(depth) * dist;

    return dist;

}

QList<QGeoCoordinate> getPoints(QVector<QByteArray> &coor) {// Function to get points of line

/////////////////////////////////// *Variables
    QList<QByteArray> xy;
    QList<QGeoCoordinate> line;
    QByteArray readline;
    QGeoCoordinate points;

////////////////////////////////////////////// For loop to split lines
    for (int i = 0; i < coor.length(); i++) {

        xy.append(coor[i].split(','));

    }


////////////////////////////////////////////////// For loop to gather line points
    for (int j = 0; j < xy.size() - 2; j = j+3) {

        points.setLatitude(xy[j].toDouble());
        points.setLongitude(xy[j + 1].toDouble());
        points.setAltitude(xy[j + 2].toDouble());

        line.append(points);

    }

    return line;

}

void closeFile(QFile *file){// Closes file

    file->close();
}


void createPolyPoints(double multi, QVector<QByteArray> *readlines, QFile *outFile){

///////////////////////////////////// *Variables
    double times = multi;
    double length = 0;
    QGeoCoordinate point1;
    QGeoCoordinate point2;
    double bearing = 0;
    QList<QGeoCoordinate> points;
    QList<QGeoCoordinate> newline;
    QTextStream out(outFile);
////////////////////////////////////////

    if(outFile->isOpen()){

    }
    else{
        outFile->open(QIODevice::WriteOnly | QIODevice::Text);
    }
    points = getPoints(*readlines);

////////////////////////////////////////////// For loop to get points 90 and -90 degrees from beging point of line
    for (int i = 0; i < points.size() ; i++) {


        if(i == points.size() - 1){

            point1.setLongitude(points[i].longitude());
            point1.setLatitude(points[i].latitude());
            point2.setLongitude(points[i - 1].longitude());
            point2.setLatitude(points[i - 1].latitude());
            length = getDist(times, points[i].altitude());
        }
        else{

            point1.setLongitude(points[i].longitude());
            point1.setLatitude(points[i].latitude());
            point2.setLongitude(points[i + 1].longitude());
            point2.setLatitude(points[i + 1].latitude());  // If statement to make line out of points in file/
            length = getDist(times, points[i].altitude()); // If last look at the point behind else look one ahead
        }

        bearing = point1.azimuthTo(point2);

        point2 = point1.atDistanceAndAzimuth(length, bearing + 90);

        point1 = point1.atDistanceAndAzimuth(length, bearing - 90);

        newline.push_back(point1);
        newline.push_back(point2);

    }

    for(int j = 0; j < newline.size(); j++) {
        out << newline[j].toString(QGeoCoordinate::Degrees) << "\n";
    }
    out << "\n";
    points.clear();

}

void readFile (double multi, QString filePath, int counter, QFile *outFile){

////////////////////////////// *Variables
    QVector<QByteArray> line;
    QFile file(filePath);
    int count = counter;
    QFileInfo fileInfo(filePath);    
/////////////////////////////

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){// open file as read only

        return;

    }

    if(file.size() == 0){

        return;

    }

///////////////////////////////////////////////////////////
        while(!file.atEnd()) {

            line.push_back(file.readLine());

            if(line.endsWith("\n")){

                line.pop_back();

                createPolyPoints(multi, &line, outFile);

                line.clear();

                count++;

            }
            else{

                line.push_back(file.readLine());

            }

        }


        createPolyPoints(multi, &line, outFile);
        line.clear();
        closeFile(&file);


}



void line2polygon::on_BrowseButton_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"C://");

    ui->fileLocation->setText(dir);

}

void line2polygon::on_converButton_clicked(){

/////////////////////////////////////////////////// *Variables
    int counter = 1;
    QDir dir(ui->fileLocation->text());
    QStringList list = dir.entryList(QDir::Files);
    double angle = ui->multi->text().toDouble();
    QString filename(ui->fileLocation->text() + "/Boundry.txt");
    QFile outFile(filename);
///////////////////////////////////////////////////

    dir.setNameFilters(QStringList() << "*.txt");

    ui->progressBar->setValue(0);

    ui->progressBar->setMinimum(0);

    ui->progressBar->setMaximum(list.size());



    for(int i = 0; i < list.size(); i++){

        if(angle <= 0){

            ui->cLabel->setText("Must be Greater than 0!");

            continue;
        }


        readFile(angle, dir.path().append("/" + list.at(i)), counter, &outFile);

        ui->progressBar->setValue( i + 1);

    }

}
