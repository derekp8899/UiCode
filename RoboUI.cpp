/*
Author: Derek Popowski & Zach Hause
derek.a.popowski@und.edu
zachary.hause@ndus.edu
UND Robotic Mining Team

Main Robot control client with CLUI
for use on control station running windows OS

Values read in from server are in the order of:
	[0] = Left Front speed sensor
	[1] = Left Rear speed sensor
	[2] = Right Front speed sensor
	[3] = Right Rear speed sensor
	[4] = Distance measurment

Values sent out to the server are in the order of:
	[0] = Left control stick X direction
	[1] = Left control stick Y direction
	[2] = Right control stick X direction
	[3] = Right control stick Y direction
	[4] = Left Trigger
	[5] = Right Trigger
	[6] = Left Shoulder button
	[7] = Right Shoulder button
	[8] = A button
	[9] = B button
	[10] = X button
	[11] = Y button
	[12] = Start button
	[13] = Select button

requires PDCurses libraries
 this can be found at https://www.projectpluto.com/win32a.htm


*/

/*Necessary Windows header file*/
#include "client.h" 

//these are the linux version includes
/* Important to note that socket code will
	not work in a linux environment
#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
*/

using namespace std;

//define the normal colors
#define white 15
#define red 12
#define green 10
#define yellow 14
#define black 16
#define grey 8

//define the highlight colors
#define hgreen 100
#define hred 101
#define hyellow 102
#define hgrey 103

//define max speed of motors
#define MAX .3

//these values come directly from client.cpp for the sole purpose
//	of populating the UI. They are global to anything that
//	includes client.h
std::string socketStatus;
std::string lShoulder, rShoulder;
std::string a, b, x, y;
std::string startButton, selectButton;
int isConnected;
float lXstick;
float lYstick;
float rXstick;
float rYstick;
float rTrigger;
float lTrigger;
float LFspd;
float LRspd;
float RRspd;
float RFspd;
float dist;

int main(int argc, char** argv) {


	//SOCKET ConnectSocket = INVALID_SOCKET; // instantiate the socket 
	a = b = x = y = "0";
	int i = 0;//loop counter
	int t = 0;
	SOCKET ConnectSocket = INVALID_SOCKET;
	int iResult = 0;
	int loop = 1, loop2 = 1;
	//printf("attempting to connect\n");
	ConnectSocket = setup(iResult, ConnectSocket);
	while (ConnectSocket == INVALID_SOCKET) {
		ConnectSocket = setup(iResult, ConnectSocket);
	}
	//printf("connections established trying send and recv loop\n");
	//printf("client connect attempting data send and recv\n");
	
	/* this is the tester of remote control */
	/* -TODO- DEBUG THIS in client.cpp or how it is being called*/
	/**************************************************
	while (true) {	//loop to keep the client active
		
		loop = sendData(iResult, ConnectSocket);
		loop2 = recieveDistance(iResult, ConnectSocket);
		if (loop == 0 || loop2 == 0) {						 //If sendData returns 1 to loop variable, that means the server shutdown, so if statement will be triggered to reestablish connection
			ConnectSocket = INVALID_SOCKET;					//reinitialize ConnectSocket to default
			ConnectSocket = setup(iResult, ConnectSocket); //Connect to the new server
			loop = sendData(iResult, ConnectSocket);	  //reset counters so we maintain connection
			loop2 = recieveDistance(iResult, ConnectSocket);
		}
	}
	******************************************************/
	initscr(); //init it
	start_color();//start color support

	clear();  //clear the current screen
	refresh();//to write changes to the screen

	//init all the color pairs FG,BG
	for (i = 0; i < 256; i++) {
		init_pair(i, i, COLOR_BLACK);
	}
	//init the highlight pairs to known values
	init_pair(100, COLOR_BLACK, green);
	init_pair(101, COLOR_BLACK, red);
	init_pair(102, COLOR_BLACK, yellow);
	init_pair(103, COLOR_BLACK, grey);

	/*//this will show the color test screen
	while(1){
	  clear();//clear the screen so no hanging characters
	  for(int j = 0; j <256;j++){
		for(i=0;i<15;i++){
	  attron(COLOR_PAIR(j));
	  printw(" [%d] ",j);
	  attroff(COLOR_PAIR(j));
	  j++;
		}
		j--;
		printw("\n");
		}

	  refresh();
	*/
	//Sleep(10000); uncomment to show colortest card for 10 sec
	clear();//clear before we start out ui

	while (1) {
		/*   remote control testing*/
		loop = sendData(iResult, ConnectSocket);
		loop2 = recieveDistance(iResult, ConnectSocket);
		if (loop == 0 || loop2 == 0) {						 //If sendData returns 1 to loop variable, that means the server shutdown, so if statement will be triggered to reestablish connection
			ConnectSocket = INVALID_SOCKET;					//reinitialize ConnectSocket to default
			ConnectSocket = setup(iResult, ConnectSocket); //Connect to the new server
			loop = sendData(iResult, ConnectSocket);	  //reset counters so we maintain connection
			loop2 = recieveDistance(iResult, ConnectSocket);
		}
		move(0, 0);
		printw("testing the recieves: %f %f %f %f %f \n\n",LFspd,RFspd, LRspd,RRspd);
		printw("Controller IN     |     Motor speed            |      Wheel Status \n");
		printw("--------------------------------------------------------------------\n");
		printw("  X : ");
		if (lXstick < 0) {
			attron(COLOR_PAIR(red));
			printw("[%+1.6f]", lXstick);
			attroff(COLOR_PAIR(red));
		}
		else if (lXstick > 0) {
			attron(COLOR_PAIR(green));
			printw("[%+1.6f]", lXstick);
			attroff(COLOR_PAIR(green));
		}
		else {
			attron(COLOR_PAIR(grey));
			printw("[%+1.6f]", lXstick);
			attroff(COLOR_PAIR(grey));
		}
		printw(" | ");
		attron(COLOR_PAIR(white));
		printw("LF: [%+.3f]  RF: [%+.3f] | \n", LFspd, RFspd);
		attroff(COLOR_PAIR(white));
		printw("  Y : ");
		if (lYstick < 0) {
			attron(COLOR_PAIR(red));
			printw("[%+1.6f]", lYstick);
			attroff(COLOR_PAIR(red));
		}
		else if (lYstick > 0) {
			attron(COLOR_PAIR(green));
			printw("[%+1.6f]", lYstick);
			attroff(COLOR_PAIR(green));
		}
		else {
			attron(COLOR_PAIR(grey));
			printw("[%+1.6f]", lYstick);
			attroff(COLOR_PAIR(grey));
		}
		printw(" | ");
		attron(COLOR_PAIR(white));
		printw("LR: [%+.3f]  RR: [%+.3f] |   ", LRspd, RRspd);
		attroff(COLOR_PAIR(white));
		if (LFspd > 0) {
			attron(COLOR_PAIR(hgreen));
			printw("[ LF }");
			attroff(COLOR_PAIR(hgreen));
		}
		else if (LFspd < 0) {
			attron(COLOR_PAIR(hred));
			printw("[ LF ]");
			attroff(COLOR_PAIR(hred));
		}
		else {
			attron(COLOR_PAIR(hgrey));
			printw("[ LF ]");
			attroff(COLOR_PAIR(hgrey));
		}
		printw("   ");
		if (RFspd < 0) {
			attron(COLOR_PAIR(hgreen));
			printw("[ RF ]\n");
			attroff(COLOR_PAIR(hgreen));
		}
		else if (RFspd > 0) {
			attron(COLOR_PAIR(hred));
			printw("[ RF ]\n");
			attroff(COLOR_PAIR(hred));
		}
		else {
			attron(COLOR_PAIR(hgrey));
			printw("[ RF ]\n");
			attroff(COLOR_PAIR(hgrey));
		}
		printw("                  |                            |                \n");
		printw("------------------|----------------------------|                \n");
		printw("  Mining Status   |    Distance Measure        |                \n");
		printw("------------------|----------------------------|   ");
		if (LRspd > 0) {
			attron(COLOR_PAIR(hgreen));
			printw("[ LR ]");
			attroff(COLOR_PAIR(hgreen));
		}
		else if (LRspd < 0) {
			attron(COLOR_PAIR(hred));
			printw("[ LR ]");
			attroff(COLOR_PAIR(hred));
		}
		else {
			attron(COLOR_PAIR(hgrey));
			printw("[ LR ]");
			attroff(COLOR_PAIR(hgrey));
		}
		printw("   ");
		if (RRspd < 0) {
			attron(COLOR_PAIR(hgreen));
			printw("[ RR ]\n");
			attroff(COLOR_PAIR(hgreen));
		}
		else if (RRspd > 0) {
			attron(COLOR_PAIR(hred));
			printw("[ RR ]\n");
			attroff(COLOR_PAIR(hred));
		}
		else {
			attron(COLOR_PAIR(hgrey));
			printw("[ RR ]\n");
			attroff(COLOR_PAIR(hgrey));
		}
		if (!a.compare("0")) {
			attron(COLOR_PAIR(hgrey));
			printw("Mining arm: UP");
			attroff(COLOR_PAIR(hgrey));
		}
		else {
			attron(COLOR_PAIR(hgreen));
			printw("Mining arm: DN");
			attroff(COLOR_PAIR(hgreen));
		}
		printw("  ");
		printw("  |   ");
		attron(COLOR_PAIR(white));
		printw("Ds: ");
		attroff(COLOR_PAIR(white));
		attron(COLOR_PAIR(hred));
		printw("%06.2f(m)", dist);
		attroff(COLOR_PAIR(hred));
		printw("   ");
		printw("         |                \n");
		if (!b.compare("0")) {
			attron(COLOR_PAIR(hgrey));
			printw("Miner: SHUTDOWN");
			attroff(COLOR_PAIR(hgrey));
		}
		else {
			attron(COLOR_PAIR(hgreen));
			printw("Miner:  ACTIVE ");
			attroff(COLOR_PAIR(hgreen));
		}
		printw(" ");
		printw("  |                            |                \n");
		printw("                  |                            |   ");
		if (LFspd == 0 && LRspd == 0 && RFspd == 0 && RRspd == 0) {
			printw("    ");
			attron(COLOR_PAIR(hgrey));
			printw("[ IDLE ]\n");
			attroff(COLOR_PAIR(hgrey));
		}
		else if ((LFspd < MAX || LRspd < MAX || RFspd > -MAX || RRspd > -MAX) ||
			     (LFspd > -MAX || LRspd > -MAX || RFspd < MAX || RRspd < MAX)) {
			printw("    ");
			attron(COLOR_PAIR(hyellow));
			printw("[ RAMP ]\n");
			attroff(COLOR_PAIR(hyellow));
		}
		else {
			printw("    ");
			attron(COLOR_PAIR(hgreen));
			printw("[ FULL ]\n");
			attroff(COLOR_PAIR(hgreen));
		}
		printw("                  |                            |                    \n");
		printw("--------------------------------------------------------------------\n");
		printw("\n");
		printw("Connections Status: ");
		if (1) {
			attron(COLOR_PAIR(hgreen));
			printw("[ %s ]\n", socketStatus.c_str());
			attroff(COLOR_PAIR(hgreen));
		}
		else {
			attron(COLOR_PAIR(hred));
			printw("[ %s ]\n", socketStatus.c_str());
			attroff(COLOR_PAIR(hred));
		}
		printw("\n");
		printw("Controller Port: ");
		if (isConnected) {
			attron(COLOR_PAIR(hgreen));
			printw("[ %d ]\n", isConnected);
			attroff(COLOR_PAIR(hgreen));
		}
		else {
			attron(COLOR_PAIR(hred));
			printw("[ %d ]\n", isConnected);
			attroff(COLOR_PAIR(hred));
		}
		refresh();
	}

	refresh();

	getch();//wait for a key to be pressed

	endwin();//end the curses session

}
