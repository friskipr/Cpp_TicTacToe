/*
 * tictac.h
 *
 *  Created on: May 17, 2016
 *      Author: ibrahim
 */

#ifndef TICTAC_H_
#define TICTAC_H_

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "Poco/MongoDB/Database.h"

using namespace std;

char mat[3][3], p1, p2;
int init();
void display();
int input(char, int, int);
char check();

int init()
{
    int i, j;
    for (i=0; i<3; i++)
		for (j=0; j<3; j++)
            mat[i][j] = '-';

    return 0;
}

void display()
{

	int i, j;
	system("clear");
	cout<<"\nWelcome to the Tic-Tac-Toe Game!\n\nGrid Co-ordinates : "<<endl<<endl;

    for (i=0; i<3; i++)
	{
		cout<<"\t";
	    for (j=0; j<3; j++)
			cout<<i<<" "<<j<<"    ";
		cout<<endl;
	}

	cout<<"\nThe Tic-Tac-Toe board : "<<endl<<endl;
	for (i=0; i<3; i++)
	{
		cout<<"\t";
	    for (j=0; j<3; j++)
		    cout<<" "<<mat[i][j]<<"     ";
		cout<<endl;
	}
}

int input(char choice, int x, int y)
{
	if (x >= 0 && x <= 2 && y >= 0 && y<= 2)
		if (mat[x][y] == '-')
		{
			mat[x][y] = choice;
			return 0;
		}
		else
		{
			cout<<endl<<"Co-ordinates "<<x<<" "<<y<<" are already used!"<<endl<<"Press ENTER to continue..."<<endl;
			do{}
			while(cin.get() != '\n');
			return 1;
		}
	else
	{
		cout<<"\nInvalid Co-ordinates!"<<endl<<"Press ENTER to continue..."<<endl;
		do{}
		while(cin.get() != '\n');
		return 1;
	}

}

char check()
{
	int i, j;
	char temp;

	for (j=0; j<3; j++)
	{
		if (mat[0][j] != '-')
		{
			temp = mat[0][j];
			if (j == 0)
			{
				if (mat[0][1] == temp)
					if (mat[0][2] == temp)
						return temp;
				if (mat[1][0] == temp)
					if (mat[2][0] == temp)
						return temp;
				if (mat[1][1] == temp)
					if (mat[2][2] == temp)
						return temp;
			}
			else if (j == 1)
			{
				if (mat[1][1] == temp)
					if (mat[2][1] == temp)
						return temp;
			}
			else
			{
				if (mat[1][2] == temp)
					if (mat[2][2] == temp)
						return temp;
			}
		}
	}
	for (i=1; i<3; i++)
	{
		if (mat[i][0] != '-')
		{
			temp = mat[i][0];
			if (i == 1)
			{
				if (mat[1][1] == temp)
					if (mat[1][2] == temp)
						return temp;
			}
			else
			{
				if (mat[2][1] == temp)
					if (mat[2][2] == temp)
						return temp;
				if (mat[1][1] == temp)
					if (mat[0][2] == temp)
						return temp;
			}
		}
	}

	return 'f';

}

class Player
{
	public:
		string name;
		int win;
		int lose;
		int draw;
};

class Repo
{
    public:
        void Create(Poco::MongoDB::Document::Ptr document);
        void Read();
        void Update(Player player);
        void Delete(char *name);
        Repo(string host, int port);
        virtual ~Repo();

    private:
        string _host;
        int _port;
};
#endif /* TICTAC_H_ */
