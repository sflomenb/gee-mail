
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gcrypt.h>
#include "database.hh"
#include <math.h>       /* pow */
#include <random>
#include <openssl/sha.h>
#include <sstream>


using namespace std;

class GeeMail {
public:
	//default constructor
	GeeMail() {
		database = new Database();
	}
	~GeeMail();
	void start();
	int registerUser();
	int signIn();
	int getAction(int max);
	string getUserInput();
	bool verifyUser(string username, string password);
	void printEmails(vector<vector<string> > emails);
	void printEmail(vector<string> email);
	string getUserInputNotNum();
private:
	Database *database;
};

void GeeMail::start() {
	// inti
	random_device rd;     // only used once to initialise (seed) engine
	mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	bool validated = false;
	string username = "";
	string password = "";
	int p, g, Sa, t = 0;


	cout << "welcome come to Gee-Mail" << endl;
	cout << "Do you want to sign-in or register?" << endl;
	cout << "if you want to sign-in, enter 1,  or 2 for register or 0 to quit" << endl;
	
	int action = -1;
	while(action != 0) {
		action = getAction(2);

		if (action == 1) {
			while (!validated) {
				// sign in
				cout << "enter username: ";
	
				username = getUserInputNotNum();
				cout << "enter password: ";
				password = getUserInputNotNum();
	
				// check user's password against db
				validated = database->validateCredentials(username, password);
				if (!validated) {
					cout << "Invalid credentials!" << endl;
				}
			}
		} else {
			// register
			cout << "enter username: ";
			username = getUserInputNotNum();
			cout << "enter password: ";
			password = getUserInputNotNum();
			// add user to database
			cout << "registering" << endl;
			// generate p, g, t
			std::uniform_int_distribution<int> uni(1, 10 - 1); // guaranteed unbiased
			p = uni(rng);
			g = uni(rng);
			Sa = uni(rng);
			t = pow(g, Sa) ;
			t = t % p;
			cout << p << g << Sa << t << endl;
			cout << "your secret key is: " << Sa << ", please reamember it" << endl;
			database->addUser(username, password, p, g, t, Sa);
		}
	
		// user validated
		cout << "validated" << endl;
	
		cout << "enter 1 for reading message and 2 for write message or 0 to quit" << endl;
		action = getAction(2);
		int secretKey = 0;
		if (action == 1) { // read email
			// get keys
			int correctSecretKey = database->getSecretKey(username);
			// get email
			vector < vector<string> > emails;
			emails = database->getEmail(username, database->getSharedKey(username, correctSecretKey));
			// list email
			int number_of_email = emails.size();
			cout << "you have " << number_of_email << " email(s)" << endl;
			cout << "enter the secretKey in order to read the email" << endl;
			secretKey = getAction(100000);
	
			// check secret key
			while (secretKey != correctSecretKey) {
				cout << "wrong secretKey, please enter again:" << endl;
				secretKey = getAction(100000);
			}
			printEmails(emails);
	
		}
		/// account 2 secrete key is 8
		if (action == 2) {
			// write message
			cout << "to: ";
			string receiver = getUserInputNotNum();
			cout << "enter titles: ";
			string title = getUserInputNotNum();
			cout << "enter email body: ";
			string body = getUserInputNotNum();
	
			// send email
			int correctSecretKey = database->getSecretKey(receiver);

			if (database->sendEmail(username, receiver, title, body,  database->getSharedKey(receiver, correctSecretKey))) {
				cout << "send email successfully" << endl;
			} else {
				cout << "send email fail" << endl;
			}
		}
		
	}
	
	exit(0);

	// ask to read or write email
}

int GeeMail::getAction(int max) {

	int coordinate = 0;
	string input = "";
	while (true) {
		getline(cin, input);

		// This code converts from string to number safely.
		stringstream myStream(input);
		if (myStream >> coordinate) {
			if (coordinate <= max && coordinate != 0) {
				break;
			}
			else if(coordinate == 0) {
				exit(0);
			}
		}
		cout << "Invalid input" << endl;
	}
	return coordinate;
}

string GeeMail::getUserInput() {
	string input = "";
	string result = "";
	while (true) {
		getline(cin, input);

		stringstream myStream(input);
		if (myStream >> result) {
			if (result.length() != 0) {
				break;
			}
		}
		cout << "Invalid input " << endl;
		cout << "enter again: ";

	}
	return result;
}

string GeeMail::getUserInputNotNum() {
	string input = "";
	getline(cin, input);
	return input;
}

// string GeeMail::hash(string password) {
// 	/*char* hashValue = (char*)malloc(33);
// 	size_t index;
// 	size_t txtLength = password.length();
// 	char * hashBuffer = (char*)malloc(33);
// 	char * textBuffer = (char*) malloc(txtLength + 1);
// 	memset(hashBuffer, 0, 33);

// 	// to use strncpy, convert string to const char *
// 	const char* conv_password = password.c_str();
// 	strncpy(textBuffer, conv_password, txtLength);

// 	gcry_md_hash_buffer(
// 	    GCRY_MD_SHA256, // gcry_cipher_hd_t
// 	    hashBuffer,    // void *
// 	    textBuffer,    // const void *
// 	    txtLength);   // size_t
// 	cout << "hashBuffer" << hashBuffer << endl;
// 	char* buf_ptr = hashValue;
// 	for (index = 0; index < 32; index++) {
// 		buf_ptr += sprintf(buf_ptr, "%02x", hashBuffer[index]);
// 	}
// 	*(buf_ptr + 1) = '\0';
// 	// cout << "hash is " << hashValue << endl;
// 	return hashValue;*/
	
//     std::string myString = "Original String";
//     unsigned char *str=new unsigned char[myString.length()+1];
//     strcpy((char *)str,myString.c_str());
//     unsigned char hash[SHA_DIGEST_LENGTH]; // == 20
//     SHA1(str, sizeof(str) - 1, hash);
    
//     for(int i = 0; i < 5; i++) {
//         std::cout << hash[i] << std::endl;
//     }
    
//     std::stringstream s;
//     s << hash;
    
//     std::string str_my_txt = s.str();
    
//     std::cout << str_my_txt << std::endl;
    
//     return str_my_txt;
    
	
// }

void GeeMail::printEmail(vector<string> email) {
	for (auto item : email) {
		cout << item << "\t ";
	}
}


void GeeMail::printEmails(std::vector<vector<string> > emails) {
	cout << "Number \t from| \t title| \t body|" << endl;
	int count  = 1;
	for (auto email : emails) {
		cout << count << "\t";
		printEmail(email);
		cout << endl;
		count ++;
	}
}


GeeMail::~GeeMail() {

}


