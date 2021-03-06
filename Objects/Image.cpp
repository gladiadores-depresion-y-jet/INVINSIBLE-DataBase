//
// Created by Daniel on 21/05/2019.
//

#include "Image.h"


Image::Image() {
    this->name = "Default";
    this->author = "NoneAuthor";
    this->year = 0;
    this->size = 0;
    this->description = "nothing to describe";
    this->ID = 0;
}

Image::Image(string nam, string aut, int yea, int siz, string des ,int id, string exten) {

    this->name = nam;
    this->author = aut;
    this->year = yea;
    this->size = siz;
    this->description = des;
    this->ID = id;
    this->extension = exten;

}

 std::string Image::getName(){
    return this->name;
}

void Image::setName(std::string n){
    this->name = n;
}

std::string Image::getAuthor(){
    return this->author;
}

void Image::setAuthor(std::string a) {
    this->author = a;
}

std::string Image::getDescription(){
    return this->description;
}

void Image::setDescription(string des) {
    this->description= des;
}

int Image::getSize() {
    return this->size;
}

void Image::setSize(int siz) {
    this->size = siz;
}

int Image::getYear(){
    return this->year;
}

void Image::setYear(int y) {
    this->year = y;
}

int Image::getID() {
    return this->ID;
}

string Image::getExten() {
    return this->extension;
}

void Image::setExtension(string exte) {
    this->extension = exte;
}




