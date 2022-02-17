#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

#define SHELLSCRIPT "\
#/bin/bash \n\
echo -e \"This is a test shell script inside C code!!\" \n\
"
//-----------------Prototypy----------------------
void Remove_All_Lines_Until_IFJCODE();
void ErrorStatus(const string &s);
//------------------------------------------------


int main(int argc, char *argv[]) {

    system("./DoMake.sh");
    ErrorStatus("Makefile");

//    system("cp input.txt cmake-build-debug/input.txt");

    system("./Run_IFJ18.sh");
    ErrorStatus("Run_IFJ18.sh");

    Remove_All_Lines_Until_IFJCODE();

    system("./Run_ic18int.sh");
//    system("./ic18int Clean_IFJ_Code.txt > IFJ18_result2.txt");
    ErrorStatus("Run_ic18int.sh");

    system("./RunRuby.sh");
    printf("\n");
    //printf("----------------------TEST-START-----------------------");

    system("./RunDiff.sh");



    return 0;
}


//---------------------Funkcie-------------------------

void ErrorStatus(const string &s){
    if ( (system("echo $? > /dev/null 2>&1") ) ){
        fprintf(stderr, "Error %s\n", s.c_str());
    } else {
        printf("%s done.\n", s.c_str());
    }
}

//source: https://stackoverflow.com/questions/30627168/c-delete-all-lines-in-a-txt-file-up-until-a-certain-keyword-is-reached
void Remove_All_Lines_Until_IFJCODE(){
    int counter = 0;

    ifstream FileSearch("IFJ18_output.txt"); // search OutputVector input file.

    while(!FileSearch.eof())
    {
        counter++;
        string temp;
        getline(FileSearch, temp);
//        FileSearch >> temp;

        if(temp == ".IFJcode18")
        {
            break; //While loop adding +1 to counter each time <event> is not found.
        }
    }


    std::ofstream outFile("./Clean_IFJ_Code.txt"); //Create output file "final.txt."
    std::string line;

    std::ifstream inFile("IFJ18_output.txt"); //open input file OutputVector again.

    int count = 0;

    while(getline(inFile, line)){

        if(count > counter-2){
            outFile << line << std::endl;
        }
        count++; //while loop counts from counter-2 until the end and writes them to the new file.
    }
    outFile.close();
    inFile.close(); //close the files.
}

//  setenv("ShellVar", "make cd ../", 1) ;
//  system("echo Vysledok $ShellVar") ;
//  system("chmod 777 DoMake.sh");


//    system((std::string("echo --help ") + std::string("Ano ")).c_str());

//    system((std::string("\"C=This is a test shell script inside C code!!\"")).c_str());
//    system((string("echo vysledok $C")+string("$C")).c_str());
//      system("echo $B");




//    // 1. Via the system() call.....
//    std::stringstream ss;
//    ss << "C=\"position\"";
//    system(ss.str().c_str());
//    ss << "\"echo $C\"";
//    system(ss.str().c_str());



//    setenv("ShellVar", "hello world", 1) ;
//    system("echo Vysledok $ShellVar") ;
//
//
//    system(SHELLSCRIPT);
