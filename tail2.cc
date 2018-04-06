/**
 * @file tail2.cc
 * @author Natalia Holkova, FIT
 * @date 5.4.2018
 * @brief Implementation of function tail in C++
 *
 * IJC-DU2, a)
 */

#include <fstream>
#include <string>
#include <iostream>
#include <queue>
#include <string.h>

using namespace std;

bool check_if_int(char str[]) {
    unsigned int i = 0;
    while (i < strlen(str) ) {
        if (!isdigit(str[i]))
            return false;
        i++;
    }
    return true;
}

int load_from_file(char *filename, queue<string>& q) {
    string text;
    ifstream infile;
    infile.open(filename);

    if (!infile.is_open()) {
        cerr << "Error! Could not open file " << filename << "." << endl;
        return -1;
    }

    while (!infile.eof()) {
        getline(infile, text);
        //if (text.length() > 0) {
        //    q.push(text);
        //}
        q.push(text);
    }
    infile.close();

    return 0;
}

int load_from_stdin(queue<string>& q) {
    string text;
    cout << "Som tu\n";
    while(!cin.eof()) {
        getline(cin, text);
        //if (text.length() > 0) {
        //    q.push(text);
        //}
        q.push(text);
    }
    return 0;
}

void print_all(queue<string> q) {
    unsigned int i = q.size()-1;
    while (!q.empty() && i > 0) {
        cout << q.front() << endl;
        q.pop();
        i--;
    }
}

void print_last_X_lines(queue<string> q, int x) {
    int start = q.size() - x - 1;
    if (start <= 0 )
        print_all(q);
    else {
        unsigned int i = q.size()-1;
        while (!q.empty() && i > 0) {
            if (start == 0) {
                cout << q.front() << endl;
                q.pop();
            } else {
                start--;
                q.pop();
            }
            i--;
        }
    }
}

int main(int argc, char **argv) {
    int lines_to_print = 10;
    queue<string>q1;

    if (argc == 1) {
        load_from_stdin(q1);
        print_all(q1);
    }
    else if (argc == 2) {
        load_from_file(argv[1], q1);
    }
    else if (argc == 3) {
        if (strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n' ) {
            cerr << "Error! " << argv[1] << " is not -n." << endl;
            return -1;
        }

        if (!check_if_int(argv[2])) {
            cerr << "Error! " << argv[2] << " is not an integer." << endl;
            return -1;
        }
        lines_to_print = stoi(argv[2]);
        load_from_stdin(q1);
    }
    else if (argc == 4) {
        if (strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'n' ) {
            cerr << "Error! " << argv[1] << " is not -n." << endl;
            return -1;
        }
        if (!check_if_int(argv[2])) {
            cerr << "Error! " << argv[2] << " is not an integer." << endl;
            return -1;
        }
        lines_to_print = stoi(argv[2]);
        load_from_file(argv[3], q1);
    }
    else {
        cerr << "Error! Wrong arguments." << endl;
        return -1;
    }
    print_last_X_lines(q1, lines_to_print);

    return 0;
}
