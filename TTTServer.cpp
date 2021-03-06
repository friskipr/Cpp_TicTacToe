//============================================================================
// Name        : TTTServer.cpp
// Author      : Friski
// Version     :
// Copyright   : My Copyright
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdio> /* declarations used in most input and output */
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <sys/socket.h> /* needed for socket */
#include <sys/types.h> /* definitions of a number of data types used in system calls */
#include <netinet/in.h> /* needed for internet domain address */
#include <arpa/inet.h>
#include <unistd.h>
#include "tictac.h"
//#include "Poco/MongoDB/Connection.h"
#include "Poco/MongoDB/Database.h"
#include "Poco/MongoDB/InsertRequest.h"
#include "Poco/MongoDB/UpdateRequest.h"
#include "Poco/MongoDB/DeleteRequest.h"
#include "Poco/MongoDB/QueryRequest.h"
//#include "Poco/MongoDB/MongoDB.h"
#include "Poco/Net/NetException.h"
#include "Poco/SharedPtr.h"
#define   PORT 			"9987"
#define   MONGODB_PORT 	27017
#define   MONGODB_HOST 	"localhost"

const string DATABASE_NAME = "TicTacToe";
using namespace std;
using namespace Poco;
using namespace Poco::MongoDB;
using namespace Poco::Net;

Player *Exist(char *name);

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, port_no, bindfd, listenfd, bytes_sent, bytes_recvd;
	char sbuffer[512], cli_ip[16], sname[64], cname[64];
	char *ptr_buff, *ptr_port;
	const char *ptr_cli_ip;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t serv_size, cli_size;

	int inp_true = 0, count = 0, inp, ni, x, y, toss;
	char serv_choice, cli_choice, nc;
	char choice_buffer[2], co_ordinates_buffer[2], toss_buffer;

	system("clear");
	ptr_buff = &sbuffer[0];
	ptr_port = (char *)&PORT;

	//creating sever side socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("Server side listening Socket could not be created!");
		return 1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	port_no = atoi(ptr_port);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	//binding socket
	bindfd = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (bindfd == -1)
	{
		perror("Failed to bind!");
		return 1;
	}

    Repo repo(MONGODB_HOST, MONGODB_PORT);
    cout<<"--Player History--"<<endl;
    repo.Read();

    cout<<endl<<"Enter your Name : ";
	cin>>sname;
	Player *playerExist = Exist(sname);

	if (playerExist->name != "")
	{
		cout<<endl<<"Welcome back "<<sname;
		cout<<endl<<"Your history : ";
		cout<<endl<<"win  = "<<playerExist->win;
		cout<<endl<<"lose = "<<playerExist->lose;
		cout<<endl<<"draw = "<<playerExist->draw;
	}
	else
	{
		Document::Ptr person = new Document();
		person->add("name", string(sname));
		person->add("win", 0);
		person->add("lose", 0);
		person->add("draw", 0);
		repo.Create(person);
	}

	char ans;
	cout<<endl<<"Reset Game History ? (y/N)";
	cin>>ans;

	if (ans == 'y')
	{
		repo.Delete("all");
		cout<<"--Player History--"<<endl;
		repo.Read();
	}

    /*-----*/

    cout<<"Server created!"<<endl<<"Waiting for a Player..."<<endl;

    //listening for incoming connections
	listenfd = listen(sockfd, 5);
	if (listenfd == -1)
	{
		perror("Failed to listen!");
		return 1;
	}

	serv_size = sizeof(serv_addr);
	cli_size = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_size);

	if (newsockfd == -1)
	{
		perror("Failed to accept from client!");
		return 1;
	}

	ptr_cli_ip = inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, cli_size);
	cout<<"Server received connections from "<<cli_ip<<endl;

	memset(&cname, 0, sizeof(cname));
	do
	{
		static int flag = 0;
		bytes_recvd = recv(newsockfd, &cname, sizeof(cname), 0);
		if (bytes_recvd == -1 && flag == 0)
		{
			memset(&cname, 0, sizeof(cname));
			cout<<"Could not ACQUIRE Player Information!"<<endl<<"Trying again..."<<endl;
			continue;
		}
		else
		{
			flag = 1;
			bytes_sent = send(newsockfd, &sname, sizeof(sname), 0);
			if (bytes_sent == -1)
				cout<<"Could not SEND Player Data!"<<"Trying Again..."<<endl;
			else
				cout<<cname<<" has joined the game."<<endl;
		}
	}while(bytes_recvd == -1 || bytes_sent == -1);

	cout<<"Creating Game. Please wait..."<<endl;
	sleep(2);
	cout<<endl<<"Game created!"<<endl<<endl<<"Doing a toss...";
	srand(time(NULL));
	toss = rand() % 2;
	sleep(1);
	sprintf(&toss_buffer, "%d", toss);
	bytes_sent = send(newsockfd, &toss_buffer, sizeof(toss_buffer), 0);
	if (bytes_sent == -1)
	{
		perror("TOSS BUFFER not sent!");
		return 1;
	}

	if (toss == 0)
	{
		cout<<endl<<"You WON the toss!"<<endl;
		do
    	{
    		cout<<sname<<" Enter Your Choice (X or O): ";
			cin>>serv_choice;
			if (serv_choice == 'X' || serv_choice == 'x')
			{
	    		serv_choice = 'X';
	    		cli_choice = 'O';
	    		inp_true = 1;
				cout<<endl<<cname<<" gets O."<<endl<<endl<<"Lets Play!"<<endl<<endl;
			}
			else if (serv_choice == 'O' || serv_choice == 'o' || serv_choice == '0')
			{
			    serv_choice = 'O';
        		cli_choice = 'X';
	    		inp_true = 1;
	    		cout<<endl<<cname<<" gets X."<<endl<<endl<<"Lets Play!"<<endl<<endl;
			}
			else
			{
	    		cout<<"\nInvalid Choice! Please Choose Again..."<<endl;
	    		inp_true = 0;
			}
    	}while(inp_true == 0);

		memset(&choice_buffer, 0, sizeof(choice_buffer));
		choice_buffer[0] = serv_choice;
		choice_buffer[1] = cli_choice;

		bytes_sent = send(newsockfd, &choice_buffer, sizeof(choice_buffer), 0);
		if (bytes_sent == -1)
		{
			perror("CHOICE BUFFER could not be sent!");
			return 1;
		}
	}
	else
	{
		cout<<endl<<cname<<" WON the toss."<<endl;
		cout<<cname<<" is choosing. Please wait..."<<endl<<endl;

		memset(&choice_buffer, 0, sizeof(choice_buffer));
		bytes_recvd = recv(newsockfd, &choice_buffer, sizeof(choice_buffer), 0);
		if (bytes_recvd == -1)
		{
			perror("CHOICE BUFFER not received!");
			return 1;
		}
		else
		{
			serv_choice = choice_buffer[0];
			cli_choice = choice_buffer[1];
			cout<<sname<<" has chosen "<<serv_choice<<endl<<endl<<"You will play with "<<cli_choice<<endl;
			cout<<endl<<"Lets Play!"<<endl<<endl;
		}
	}

	if (serv_choice == 'X')
	{
		inp = 1;
		cout<<"You  will play first."<<endl<<endl;
	}
	else
	{
		inp = 2;
		cout<<cname<<" will play first."<<endl<<endl;
	}

	init();
	cout<<endl<<"Starting Game..."<<endl;
	sleep(3);
	display();

	while (count < 9)
	{
		memset(&co_ordinates_buffer, 0, sizeof(co_ordinates_buffer));

		if (inp % 2 != 0 )
		{
			cout<<endl<<"Your turn. Enter co-ordinates separated by a space : ";
			cin>>x>>y;
			ni = input(serv_choice, x, y);
			if (ni == 0)
			{
				inp ++;
				sprintf(&co_ordinates_buffer[0], "%d", x);
				sprintf(&co_ordinates_buffer[1], "%d", y);
				cout<<endl<<"Updating Matrix..."<<endl;

				bytes_sent = send(newsockfd, &co_ordinates_buffer, sizeof(co_ordinates_buffer), 0);
				if (bytes_sent == -1)
				{
					perror("CO-ORDINATES BUFFER not sent!");
					return 1;
				}
			}
		}
		else
		{
			cout<<endl<<cname<<"'s turn. Please wait..."<<endl;
			bytes_recvd = recv(newsockfd, &co_ordinates_buffer, sizeof(co_ordinates_buffer), 0 );
			if (bytes_recvd == -1)
			{
				perror("CO-ORDINATES BUFFER not recieved!");
				return 1;
			}
			x = co_ordinates_buffer[0] - '0';
			y = co_ordinates_buffer[1] - '0';
			ni = input(cli_choice, x, y);
			if (ni == 0)
			{
				inp ++;
				cout<<endl<<cname<<" has played. Updating Matrix..."<<endl;
			}
		}

		count ++;
		sleep(2);
		system("clear");
		display();

		if (count >=5)
		{
			nc = check();
			if (nc == 'f')
				continue;
			else if (serv_choice == nc)
			{
				cout<<endl<<"Congrats! You have won!!!"<<endl<<cname<<" lost."<<endl;

				Player player;
				player.name = sname;
				player.win = 1;
				repo.Update(player);

				break;
			}
			else if (cli_choice == nc)
			{
				cout<<endl<<"You loose."<<endl<<cname<<" has won."<<endl;
				break;
			}
		}
	}

	if (nc == 'f')
		cout<<endl<<"Game ends in a draw."<<endl;

	cout<<endl<<"Thank You for playing Tic-tac-Toe"<<endl;
	close(newsockfd);
	close(sockfd);
	return 0;
}

static bool _connected = false;
static MongoDB::Connection _mongo;

Repo::Repo(string host, int port)
{
    _host = host;
    _port = port;

    if (!_connected)
	{
		try
		{
			_mongo.connect(_host, _port);
			_connected = true;
			cout << "Connected to [" << _host << ':' << _port << ']' << endl;
		}
		catch (ConnectionRefusedException& e)
		{
			cout << "Couldn't connect to " << e.message() << ". " << endl;
		}
	}
};

void Repo::Create(Document::Ptr document)
{
    if (!_connected)
    {
        return;
    }

    Database db(DATABASE_NAME);
    SharedPtr<InsertRequest> insertRequest = db.createInsertRequest("Player");
    insertRequest->documents().push_back(document);

    _mongo.sendRequest(*insertRequest);
}

void Repo::Read(void)
{
    if (!_connected)
    {
        return;
    }

    string collection = DATABASE_NAME + ".Player";
    QueryRequest request(collection);

	ResponseMessage response;

	_mongo.sendRequest(request, response);

	if ( response.documents().size() > 0 )
	{
        for (int i = 0 ; i < response.documents().size() ; i++)
        {
            Poco::MongoDB::Document::Ptr doc = response.documents()[i];

            try
            {
                string name = doc->get<string>("name");
                string id = doc->get("_id")->toString();
                cout << i+1 << ". " << name << endl;
            }
            catch(Poco::NotFoundException& nfe)
            {
                cout << nfe.message() + " not found.";
            }
        }
	}
	else
	{
		perror("No recent players.");
	}
}

Player *Exist(char *name)
{
	if (!_connected)
	{
		return new Player;
	}

	string collection = DATABASE_NAME + ".Player";
	QueryRequest request(collection);

	ResponseMessage response;

	_mongo.sendRequest(request, response);

	if ( response.documents().size() > 0 )
	{
		for (int i = 0 ; i < response.documents().size() ; i++)
		{
			Player *player = new Player;
			Poco::MongoDB::Document::Ptr doc = response.documents()[i];

			try
			{
				string playerName = doc->get<string>("name");

				if (playerName == name)
				{
					player->name = playerName;

					int win, lose, draw;
					if (doc->exists("win"))
					{
						win = doc->get<int>("win");
						player->win = win;
					}
					if (doc->exists("lose"))
					{
						lose = doc->get<int>("lose");
						player->lose = lose;
					}
					if (doc->exists("draw"))
					{
						draw = doc->get<int>("draw");
						player->draw = draw;
					}

					return player;
				}
			}
			catch(Poco::Exception& ex)
			{
				cout << ex.message();
			}
		}
	}
	else
	{
		perror("No recent players.");
	}

	return new Player;
}

void Repo::Update(Player player)
{
	if (!_connected)
	{
		return;
	}

	Database db(DATABASE_NAME);
	SharedPtr<UpdateRequest> updateRequest = db.createUpdateRequest("Player");

	Document& selector = updateRequest->selector();
	Document& update = updateRequest->update();

	selector.add("name", std::string(player.name));
	update.addNewDocument("$set").add("win", int(player.win));
	update.addNewDocument("$set").add("lose", int(player.lose));
	update.addNewDocument("$set").add("draw", int(player.draw));

	_mongo.sendRequest(*updateRequest);

	selector.clear();
	update.clear();
}

void Repo::Delete(char *name)
{
	if (!_connected)
	{
		return;
	}
	string collection = DATABASE_NAME + ".Player";
	DeleteRequest request(collection);
	//request.selector().add("name", string(name));

	_mongo.sendRequest(request);
}

Repo::~Repo(void)
{
    if (_connected)
	{
		_mongo.disconnect();
		_connected = false;
		cout << "Disconnected from [" << _host << ':' << _port << ']' << endl;
	}
}
