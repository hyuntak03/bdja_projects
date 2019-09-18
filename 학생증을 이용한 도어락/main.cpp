#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <regex>

using namespace std;

int main() {
	cout<<"This is a program to read serial data"<<endl;

	list<long> barcode_list;

	ifstream filein("barcodes.txt");
	string str_line;
	while(getline(filein, str_line)) {
		barcode_list.push_back(stol(str_line));		
	}

	int fd = open("/dev/ttyACM1", O_RDWR | O_NOCTTY);
	if(fd < 0) {
		cout << "error " << endl;
	}

	struct termios tty;
	struct termios tty_old;
	memset(&tty, 0, sizeof(tty));

	if(tcgetattr(fd, &tty) != 0) {
		cout<< "error from tcgetattr" <<endl;
	}

	tty_old = tty;

	cfsetospeed(&tty, B9600);
	cfsetispeed(&tty, B9600);

	tty.c_cflag &= -PARENB;
	tty.c_cflag &= -CSTOPB;
	tty.c_cflag &= -CSIZE;
	tty.c_cflag |= CS8;


	tty.c_cflag &= -CRTSCTS; 			// no flow control
	tty.c_cc[VMIN] = 1;				// read doen't block
	tty.c_cc[VTIME] = 5;
	tty.c_cflag |= CREAD | CLOCAL;

	/* Make raw */
	cfmakeraw(&tty);

	tcflush(fd, TCIFLUSH);

	if(tcsetattr(fd, TCSANOW, &tty) != 0) {
		cout << "error from tcsetattr" << endl;
	}

	int loop_count = 0;
	
	while(true) {
		int read_size = 0;
		int pos = 0;
		char buff = '\0';
		char request[1024];
		memset(request, '0', sizeof(request));

		do{
			read_size = read(fd, &buff, sizeof(char));
			sprintf(&request[pos], "%c", buff);
			pos += read_size;
		} while(buff != '\r' && read_size > 0);

		if(read_size < 0) {
			cout << "Error in reading" << endl;
		} else if (read_size == 0) {
			cout << "Nothing to read" << endl;
		} else {
			string str_request = request;
				string str_number;
				for(int i=0; i<str_request.length(); i++) {
					if(isdigit(str_request[i])) { 
						str_number += str_request[i];
					}
				}

				cout << str_number << ", " ;
				list<long>::iterator itr = 
					find(barcode_list.begin(),
						barcode_list.end(),
						stol(str_number));
				string str_response;
				cout << " received number : " 
					<< str_number << ", ";
				if(itr!=barcode_list.end()) {
					cout << "success" << endl;
					str_response = string("1");
				} else {
					cout << "fail" << endl;
					str_response = string("0");
				}
				
				write(fd, str_response.data(), str_response.size());
			
				loop_count++;
		}
	}
	
	close(fd);
	return 0;
}
