//
// Created by Daniel on 26/05/2019.
//


#include "DataBase.h"

DataBase::DataBase() {

    this->MainList = new LinkedList<LinkedList<Image*>*>();
    this->jManager = new JManager();
    this->root = "../Master/";

    ifstream inFile;
    inFile.open("../Master/imagesID.txt");

    if (inFile.is_open()) {

        string line;
        getline(inFile,line);
        this->imageID = stoi(line);
        cout<<"imagesID: "<<this->imageID<<endl;
        inFile.close();
    }else{
        cout << "Cant pen the file";
        this->imageID = 0;
    }


}

int DataBase::getImageId() {
    return this->imageID;
}
string DataBase::getRoot() {
    return this->root;
}

void DataBase::setRoot(string newRoot) {
    this->root = "../"+newRoot+"/";
}

LinkedList<LinkedList<Image*>*> *DataBase::getMainList() {
    return this->MainList;
}

bool DataBase::addGalery(string nameGalery) {
    LinkedList<Image*> * newGalery =  new LinkedList<Image*>(nameGalery);
    return MainList->add(newGalery);

}

bool DataBase::deleteGalery(string galery) {

    if(!MainList->isEmpty()){
        Node<LinkedList<Image*> *> * delPtr = nullptr;

        MainList->setTemp(MainList->getHead());
        MainList->setCurr(MainList->getHead());
        if(MainList->getCurr()->getValue()->getID() == galery){
            delPtr = MainList->getHead();
            MainList->setHEAD(MainList->getHead()->getNext());
            delPtr->setNext(nullptr);
            delFolder(this->root+delPtr->getValue()->getID()); // eliminando folder
            delete delPtr;
            MainList->substractSize();
            return true;
        }
        while(MainList->getCurr() != nullptr && MainList->getCurr()->getValue()->getID() != galery){
            MainList->setTemp(MainList->getCurr());
            MainList->setCurr(MainList->getCurr()->getNext());
        }
        if(MainList->getCurr() == nullptr){
            cout<<"Galery not found to delete"<<endl;
            delete delPtr;
            return false;
        }else{
            MainList->getTemp()->setNext(MainList->getCurr()->getNext());
            MainList->getCurr()->setNext(nullptr);
            delPtr = MainList->getCurr();
            delFolder(this->root+delPtr->getValue()->getID()); // eliminando folder
            delete delPtr;
            MainList->substractSize();
            return true;
        }
    }

}

void DataBase::delFolder(string name) {


/*
    const int result = rmdir(name.c_str());
    if( result == 0 ){
        cout<<"success"<<endl;
    } else {
        cout<<"cant delete the galery >> "<<name<<" <<"<<endl;
    }*/

}


bool DataBase::insertImage(string json) {
    if(!MainList->isEmpty()) {

        ptree ptImage = jManager->stringToPtree(json);
        string galery = ptImage.get<string>("table");
        MainList->setTemp(MainList->getHead());

        while (MainList->getTemp() != nullptr && MainList->getTemp()->getValue()->getID() != galery) {

            MainList->setTemp(MainList->getTemp()->getNext());

        }

        if(MainList->getTemp() == nullptr){
            cout<<"Gallery not found , image not added ";
            return false;
        }else{

            string name = "NULL", author = "NULL", description = "NULL";
            int year = 0, size = 0;

            vector<string> atributVector = split(ptImage.get<string>("cols"), ',');
            vector<string> valuesVector = split(ptImage.get<string>("values"),',');
            for (int i = 0; i < atributVector.size(); i++) {
                string at = atributVector[i];
                if (at.compare("name") == 0) {
                    name = valuesVector[i];
                } else if (at.compare("author") == 0) {
                    author = valuesVector[i];
                } else if (at.compare("year") == 0) {
                    year = stoi(valuesVector[i]);
                } else if (at.compare("size") == 0) {
                    size = stoi(valuesVector[i]);
                } else if (at.compare("description") == 0) {
                    description = valuesVector[i];
                }

            }

            Image * newImage = new Image(name, author, year, size, description , this->imageID);
            this->imageID++;

            MainList->getTemp()->getValue()->add(newImage);
            cout<<"The image has been added!"<<endl;
            return true;
        }

    }else{
        cout<<"The list is empty! "<<endl;
        return false;
    }

}

ptree DataBase::selectImage(string json) {
    ptree ptImage = jManager->stringToPtree(json);
    string from = ptImage.get<string>("table");

    MainList->setTemp(MainList->getHead());

    while(MainList->getTemp() != nullptr && MainList->getTemp()->getValue()->getID() != from){
        MainList->setTemp(MainList->getTemp()->getNext());
    }
    if(MainList->getTemp() == nullptr){
        cout<<"No se encontró una galeria con el nombre <<"<<from<<">>";

    }else{
        ptree imagesJson;

        vector<string> atributesVEC = this->split(ptImage.get<string>("cols"),',');
        vector<string> whereVEC = split(replace_ALL(ptImage.get<string>("WHERE"),"OR",","),',');
        string name = "n-u*l-l";
        string author = "n-u*l-l";
        int year = -2;
        int size =-2;
        int id = -2;
        string description = "n-u*l-l";

        for(int i = 0 ; i<whereVEC.size() ; i++ ){
            whereVEC[i].erase(remove(whereVEC[i].begin(), whereVEC[i].end(), ' '), whereVEC[i].end()); //delete the spaces ' '
            vector<string> set = split(whereVEC[i],'=');
            if(set[0] == "name"){
                name = set[1];
            }else if(set[0] == "author"){
                author = set[1];
            }else if(set[0] == "year"){
                year = stoi(set[1]);
            }else if(set[0] == "size"){
                size = stoi(set[1]);
            }else if(set[0] == "description"){
                description = set[1];
            }else if(set[0] == "id"){
                id = stoi(set[1]);
            }
        }

        MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getHead());
        int cont = 0;
        while(MainList->getTemp()->getValue()->getTemp() != nullptr){

            if(name == MainList->getTemp()->getValue()->getTemp()->getValue()->getName()
            || author == MainList->getTemp()->getValue()->getTemp()->getValue()->getAuthor()
            || year == MainList->getTemp()->getValue()->getTemp()->getValue()->getYear()
            || size == MainList->getTemp()->getValue()->getTemp()->getValue()->getSize()
            || description == MainList->getTemp()->getValue()->getTemp()->getValue()->getDescription()
            || id == MainList->getTemp()->getValue()->getTemp()->getValue()->getID()){

                imagesJson.put("Image"+to_string(cont),jManager->ptreeToString(this->fillPtreeImage(MainList->getTemp()->getValue()->getTemp(),atributesVEC)));
                cont++;
            }
            MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getTemp()->getNext());

        }
        return imagesJson;
    }
}

bool DataBase::updateImage(string json) {
    ptree ptImage = jManager->stringToPtree(json);
    string from = ptImage.get<string>("UPDATE");
    MainList->setTemp(MainList->getHead());
    while(MainList->getTemp() != nullptr && MainList->getTemp()->getValue()->getID() != from){
        MainList->setTemp(MainList->getTemp()->getNext());
    }
    if(MainList->getTemp() == nullptr){
        cout<<"No se encontró la galeria >>"<<MainList->getTemp()->getValue()->getID()<<">>"<<endl;
        return false;

    }else{



        MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getHead());
        vector<string> setVEC = split(ptImage.get<string>("SET"),',');
        string name = "n-u*l-l";
        string author = "n-u*l-l";
        int year = -2;
        int size =-2;
        string description = "n-u*l-l";

        for(int i = 0 ; i<setVEC.size() ; i++ ){
            setVEC[i].erase(remove(setVEC[i].begin(), setVEC[i].end(), ' '), setVEC[i].end()); //delete the spaces ' '
            vector<string> set = split(setVEC[i],'=');
            if(set[0] == "name"){
                name = set[1];
            }else if(set[0] == "author"){
                author = set[1];
            }else if(set[0] == "year"){
                year = stoi(set[1]);
            }else if(set[0] == "size"){
                size = stoi(set[1]);
            }else if(set[0] == "description"){
                description = set[1];
            }
        }


        vector<string> whereVEC = split(replace_ALL(ptImage.get<string>("WHERE"),"OR",","),',');
        string nameID = "n-u*l-l";
        string authorID = "n-u*l-l";
        int yearID = -2;
        int sizeID =-2;
        int idID = -2;
        string descriptionID = "n-u*l-l";

        for(int i = 0 ; i<whereVEC.size() ; i++ ){
            whereVEC[i].erase(remove(whereVEC[i].begin(), whereVEC[i].end(), ' '), whereVEC[i].end()); //delete the spaces ' '
            vector<string> set = split(whereVEC[i],'=');
            if(set[0] == "name"){
                nameID = set[1];
            }else if(set[0] == "author"){
                authorID = set[1];
            }else if(set[0] == "year"){
                yearID = stoi(set[1]);
            }else if(set[0] == "size"){
                sizeID = stoi(set[1]);
            }else if(set[0] == "description"){
                descriptionID = set[1];
            }else if(set[0] == "id"){
                idID = stoi(set[1]);
            }
        }



        //TODO cambiar el where a vector para poder leer bien la sintaxis
        while(MainList->getTemp()->getValue()->getTemp() != nullptr){
            if(MainList->getTemp()->getValue()->getTemp()->getValue()->getName() == nameID
            ||MainList->getTemp()->getValue()->getTemp()->getValue()->getAuthor() == authorID
            ||MainList->getTemp()->getValue()->getTemp()->getValue()->getYear() == yearID
            ||MainList->getTemp()->getValue()->getTemp()->getValue()->getSize() == sizeID
            ||MainList->getTemp()->getValue()->getTemp()->getValue()->getDescription() == descriptionID
            ||MainList->getTemp()->getValue()->getTemp()->getValue()->getID() == idID){

                if(name != "n-u*l-l"){
                    MainList->getTemp()->getValue()->getTemp()->getValue()->setName(name);
                }
                if(author != "n-u*l-l"){
                    MainList->getTemp()->getValue()->getTemp()->getValue()->setAuthor(author);
                }
                if(year != -2){
                    MainList->getTemp()->getValue()->getTemp()->getValue()->setYear(year);
                }
                if(size != -2){
                    MainList->getTemp()->getValue()->getTemp()->getValue()->setSize(size);
                }
                if(description != "n-u*l-l"){
                    MainList->getTemp()->getValue()->getTemp()->getValue()->setDescription(description);
                }
                MainList->getTemp()->getValue()->getTemp();
            }

            MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getTemp()->getNext());
        }
        return true;


    }

}

bool DataBase::deleteImage(string json) {

    ptree imageJson = jManager->stringToPtree(json);
    MainList->setTemp(MainList->getHead());
    while(MainList->getTemp() != nullptr && MainList->getTemp()->getValue()->getID() != imageJson.get<string>("FROM")){
        MainList->setTemp(MainList->getTemp()->getNext());
    }
    if(MainList->getTemp() == nullptr){
        cout<<"Galery not found";
        return false;
    }else{

        string name = "n-u*l-l";
        string author = "n-u*l-l";
        int year = -2;
        int size = -2;
        string description = "n-u*l-l";
        vector<string> whereVEC = split(replace_ALL(imageJson.get<string>("WHERE"),"OR",","),',');

        for(int i = 0 ; i<whereVEC.size() ; i++ ){
            whereVEC[i].erase(remove(whereVEC[i].begin(), whereVEC[i].end(), ' '), whereVEC[i].end()); //delete the spaces ' '
            vector<string> set = split(whereVEC[i],'=');
            if(set[0] == "name"){
                name = set[1];
            }else if(set[0] == "author"){
                author = set[1];
            }else if(set[0] == "year"){
                year = stoi(set[1]);
            }else if(set[0] == "size"){
                size = stoi(set[1]);
            }else if(set[0] == "description"){
                description = set[1];
            }
        }

        Node<Image*> * delPtr = nullptr;
        MainList->getTemp()->getValue()->setTemp(MainList->getHead()->getValue()->getHead());
        MainList->getTemp()->getValue()->setCurr(MainList->getTemp()->getValue()->getHead());

        if(MainList->getTemp()->getValue()->getCurr()->getValue()->getName() == name
           || MainList->getTemp()->getValue()->getCurr()->getValue()->getAuthor() == author
           || MainList->getTemp()->getValue()->getCurr()->getValue()->getYear() == year
           || MainList->getTemp()->getValue()->getCurr()->getValue()->getSize() == size
           || MainList->getTemp()->getValue()->getCurr()->getValue()->getDescription() == description){
            delPtr = MainList->getTemp()->getValue()->getHead();
            MainList->setHEAD(MainList->getHead()->getNext());
            delPtr->setNext(nullptr);
            delete delPtr;
            MainList->getTemp()->getValue()->substractSize();
            return this->deleteImage(json);
        }

        while(MainList->getTemp()->getValue()->getTemp() != nullptr && MainList->getTemp()->getValue()->getCurr()->getValue()->getName() != name
            && MainList->getTemp()->getValue()->getCurr()->getValue()->getAuthor() != author
            && MainList->getTemp()->getValue()->getCurr()->getValue()->getYear() != year
            && MainList->getTemp()->getValue()->getCurr()->getValue()->getSize() != size
            && MainList->getTemp()->getValue()->getCurr()->getValue()->getDescription() != description ){

            MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getCurr());
            MainList->getTemp()->getValue()->setCurr(MainList->getTemp()->getValue()->getCurr()->getNext());
        }
        if(MainList->getTemp()->getValue()->getCurr() == nullptr){
            delete delPtr;
            return true;
        }else{
            MainList->getTemp()->getValue()->getTemp()->setNext(MainList->getTemp()->getValue()->getCurr()->getNext());
            MainList->getTemp()->getValue()->getCurr()->setNext(nullptr);
            delPtr = MainList->getTemp()->getValue()->getCurr();
            delete delPtr;
            MainList->getTemp()->getValue()->substractSize();
            return this->deleteImage(json);
        }

    }


}

ptree DataBase::fillPtreeImage(Node<Image*> *image, vector<string> vec) {

    ptree ptim;
    for (int i = 0; i < vec.size(); i++) {
        string at = vec[i];
        if (at.compare("name") == 0) {
            ptim.put("name",image->getValue()->getName());
        } else if (at.compare("author") == 0) {
            ptim.put("author",image->getValue()->getAuthor());
        } else if (at.compare("year") == 0) {
            ptim.put("year",image->getValue()->getYear());
        } else if (at.compare("size") == 0) {
            ptim.put("size",image->getValue()->getSize());
        } else if (at.compare("description") == 0) {
            ptim.put("description",image->getValue()->getDescription());
        }

    }

    return ptim;
}

vector<string> DataBase::split(string word, char delim) {

    vector<string> vec;
    std::stringstream ss(word);
    std::string token;
    while (std::getline(ss, token, delim)) {
        vec.push_back(token);}
    return vec;

}

void DataBase::saveToDisk() {
    if(MainList->isEmpty()){
        cout<<"NO hay ninguna galeria para ser creada";
    }else {
        vector<string> folders;
        vector<string> foldersNot;
        MainList->setTemp(MainList->getHead());

        while (MainList->getTemp() != nullptr) {

            string newFolder = this->root + MainList->getTemp()->getValue()->getID();
            if (mkdir(newFolder.c_str(), 0777) == 0) {
                MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getHead());
                ptree allImages;
                int cont = 0;
                while(MainList->getTemp()->getValue()->getTemp() != nullptr){
                    ptree image;
                    image.put("name",MainList->getTemp()->getValue()->getTemp()->getValue()->getName());
                    image.put("author",MainList->getTemp()->getValue()->getTemp()->getValue()->getAuthor());
                    image.put("year",MainList->getTemp()->getValue()->getTemp()->getValue()->getYear());
                    image.put("size",MainList->getTemp()->getValue()->getTemp()->getValue()->getSize());
                    image.put("description",MainList->getTemp()->getValue()->getTemp()->getValue()->getDescription());
                    allImages.put("Image"+to_string(cont),jManager->ptreeToString(image));
                    cont++;
                    MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getTemp()->getNext());
                }
                //Making a file
                //TODO AGREGAR EL COMPRESOR
                ofstream file;
                file.open(root+MainList->getTemp()->getValue()->getID()+"/MetaData.txt");
                file << jManager->ptreeToString(allImages);
                file.close();
                folders.push_back(MainList->getTemp()->getValue()->getID());
            } else {
                MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getHead());
                ptree allImages;
                int cont = 0;
                while(MainList->getTemp()->getValue()->getTemp() != nullptr){
                    ptree image;
                    image.put("name",MainList->getTemp()->getValue()->getTemp()->getValue()->getName());
                    image.put("author",MainList->getTemp()->getValue()->getTemp()->getValue()->getAuthor());
                    image.put("year",MainList->getTemp()->getValue()->getTemp()->getValue()->getYear());
                    image.put("size",MainList->getTemp()->getValue()->getTemp()->getValue()->getSize());
                    image.put("description",MainList->getTemp()->getValue()->getTemp()->getValue()->getDescription());
                    allImages.put("Image"+to_string(cont),jManager->ptreeToString(image));
                    cont++;
                    MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getTemp()->getNext());
                }
                //Making a file
                //TODO: AGREGAR EL COMPRESOR
                ofstream file;
                file.open(root+MainList->getTemp()->getValue()->getID()+"/MetaData.txt");
                file << jManager->ptreeToString(allImages);
                file.close();
                foldersNot.push_back(MainList->getTemp()->getValue()->getID());
            }
            MainList->setTemp(MainList->getTemp()->getNext());
        }
        cout<<"-- Carpetas creadas:"<<"\n"<<endl;
        for(int i = 0 ; i<folders.size() ; i++){
            cout<<"-> "<<folders[i]<<endl;
        }cout<<endl;
        cout<<"-- Carpetas NO creadas:"<<"\n"<<endl;
        for(int j = 0 ; j<foldersNot.size() ; j++){
            cout<<"-> "<<foldersNot[j]<<endl;
        }
    }

    ofstream file;
    file.open("../Master/imagesID.txt");
    file << this->imageID;
    file.close();

}

void DataBase::loadInMemory() {

}

void DataBase::showDirs() {
    if(!MainList->isEmpty()){

        MainList->setTemp(MainList->getHead());
        cout<<"[";
        while(MainList->getTemp()->getNext() != nullptr){

            cout<<MainList->getTemp()->getValue()->getID()<<",";
            MainList->setTemp(MainList->getTemp()->getNext());
        }MainList->setTemp(MainList->getTemp());
        cout<<MainList->getTemp()->getValue()->getID()<<"]"<<endl;
    }else{
        cout<<"[]"<<endl;
    }

}

void DataBase::showALLImages(string galery) {

    MainList->setTemp(MainList->getHead());

    while(MainList->getTemp() != nullptr && MainList->getTemp()->getValue()->getID() != galery){
        MainList->setTemp(MainList->getTemp()->getNext());
    }
    if(MainList->getTemp() == nullptr){
        cout<<"No se encontró una galeria con el nombre <<"<<galery<<">>";

    }else{
        ptree imagesJson;

        MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getHead());
        int cont = 0;
        while(MainList->getTemp()->getValue()->getTemp() != nullptr){

            ptree ptim;
            ptim.put("name",MainList->getTemp()->getValue()->getTemp()->getValue()->getName());
            ptim.put("author",MainList->getTemp()->getValue()->getTemp()->getValue()->getAuthor());
            ptim.put("year",MainList->getTemp()->getValue()->getTemp()->getValue()->getYear());
            ptim.put("size",MainList->getTemp()->getValue()->getTemp()->getValue()->getSize());
            ptim.put("description",MainList->getTemp()->getValue()->getTemp()->getValue()->getDescription());

            imagesJson.put("Image"+to_string(cont),jManager->ptreeToString(ptim));
            cont++;

            MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getTemp()->getNext());

        }
        this->jManager->printJson(imagesJson);
    }

}

void DataBase::compressData(string json , string galery) {

    vector<char> ch;
    for(int i=0;i<json.size();i++)
    {
        ch.push_back(json.at(i));
    }
    Compressor::Codified_File * code = this->compressor->compress(ch,"txt",this->root+galery+"/compress");
    this->compressor->writeToDiskComp(code);

}
//TODO: agregar a los archivos un identificador code + i tree + i para poder identificarlos en esta parte
// agregar tambien el numero de imagenes que hay para poder hacer unn for y descomprimir todas las imagenes

string DataBase::descompressData(string galery, string tree , string code) {
    Compressor::Codified_File * c = this->compressor->treeReconstructor(this->root+galery+"/"+tree+".txt",this->root+galery+"/"+code+".txt");
    Compressor::Decodified_File * d = this->compressor->decompress(c);
    this->compressor->writeToDiskDecomp(d);

    ifstream inFile;
    //TODO: completar esta parte con el iterador de las imagenes
    inFile.open(this->root+galery);
    string json = "";
    string x;

    if(inFile.is_open()){
        while(inFile >> x){
            json = json +x;
        }
        inFile.close();

        string fileroot = this->getRoot()+galery;

        if(remove(fileroot.c_str()) != 0){
            cout<<"Error al eliminar"<<endl;
        }
    }else{
        cout<<"Cant open the file!"<<endl;
    }
    return json;

}

string DataBase::initIdleTree() {

    ptree allGalery;
    int contGaleries = 0;

    MainList->setTemp(MainList->getHead());

    while(MainList->getTemp() != nullptr){
        ptree oneGalery;
        oneGalery.put("Name",MainList->getTemp()->getValue()->getID());
        int contImages = 0;
        MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getHead());
        while(MainList->getTemp()->getValue()->getTemp() != nullptr){
            ptree oneImage;
            oneImage.put("name",MainList->getTemp()->getValue()->getTemp()->getValue()->getName());
            oneImage.put("code",MainList->getTemp()->getValue()->getTemp()->getValue()->getID());
            oneGalery.put("Image"+to_string(contImages),this->jManager->ptreeToString(oneImage));
            contImages++;
            MainList->getTemp()->getValue()->setTemp(MainList->getTemp()->getValue()->getTemp()->getNext());
        }
        oneGalery.put("NumImages",to_string(contImages));
        allGalery.put("Galery"+to_string(contGaleries),this->jManager->ptreeToString(oneGalery));
        contGaleries++;
        MainList->setTemp(MainList->getTemp()->getNext());
    }
    allGalery.put("NUM",to_string(contGaleries));

    return this->jManager->ptreeToString(allGalery);
}

string DataBase::replace_ALL(string str , const string &from , const string &to) {
    int pos = 0;
    int flen = from.length();
    int tlen = to.length();
    while((pos = str.find(from,pos)) != -1){
        str.replace(pos,flen,to);
        pos += tlen;
    }
    return str;
}


