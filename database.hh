
#include <string>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <math.h>       /* pow */
#include <iostream>
#include <iomanip>
#include <functional>
#include <sstream>

#include <crypto++/aes.h>
#include <crypto++/modes.h>
#include <crypto++/filters.h>

using namespace std;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

class Database {
public:
	//default constructor
	Database() {
		initialization();
	}
	~Database()noexcept(false);
	void initialization();
	int addUser(string username, string password, int p, int g, int t, int Sa);
	string getUsePassword(string username);
	void getAllUser();
	bool validateCredentials(string username, string password);
	vector < vector<string> > getEmail(string receiver, int shareKey);
	bool sendEmail(string sender, string receiver, string title, string body, int shareKey);
	string encryptMessage(string message, int shared_key);
	string decryptMessage(string ciphertext, int shared_key);
	int getSharedKey(string receiver, int secretKey);
	int getSecretKey(string username);
	string hashPassword(string password);
private:
	sqlite3* db;
};



void Database::initialization() {
	int rc;
	rc = sqlite3_open("new.db", &db);
	if ( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	};

}

void Database::getAllUser() {
	char *zErrMsg = 0;

	string sqlString = "select * from  USERS;";
	cout << sqlString << endl;
	char *sql = &sqlString[0u];
	int rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if ( rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "user create successfully\n");
	}
	// sqlite3_close(db);
}

int Database::getSharedKey(string receiver, int secretKey) {
	// open connection
	char *zErrMsg = 0;
	int rc;

	// convert string to const char*
	const char* const_receiver = receiver.c_str();

	//prepare the statement
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, "select p, g, t from users where name = ?1;", -1, &stmt, NULL);       /* 1 */
	sqlite3_bind_text(stmt, 1, const_receiver, -1, SQLITE_STATIC);
	//executing the statement
	int p, g, t = 0;
	while (SQLITE_ROW == (rc = sqlite3_step(stmt))) {	// has row
		p = sqlite3_column_int(stmt, 0);
		g = sqlite3_column_int(stmt, 1);
		t = sqlite3_column_int(stmt, 2);
	}

	if (rc != SQLITE_DONE ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_finalize(stmt);

	// calculate T
	// cout << "share key:" << endl;
	int shareKey = pow(t, secretKey);
	// cout << "shareKey temp:" << shareKey << endl;
	shareKey = shareKey % p;
	// cout << "p:" << p << "t: " << t << endl;
	// cout << "shareKey:" << shareKey << endl;

	return shareKey;
}

int Database::addUser(string username, string password, int p, int g, int t, int Sa) {
	// open connection
	char *zErrMsg = 0;
	int rc;

	string hashedPW = hashPassword(password);
	cout << "hashedPW"<<hashedPW<< endl;
	// convert string to const char*
	const char* const_username = username.c_str();
	const char* conv_password = hashedPW.c_str();
	//prepare the statement
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, "insert into users (name, password, p, g, t, sa) values (?1, ?2,?3,?4,?5, ?6);", -1, &stmt, NULL);       /* 1 */
	sqlite3_bind_text(stmt, 1, const_username, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, conv_password, -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, p);
	sqlite3_bind_int(stmt, 4, g);
	sqlite3_bind_int(stmt, 5, t);
	sqlite3_bind_int(stmt, 6, Sa);
	const char* sql = sqlite3_sql(stmt);
	rc = sqlite3_step(stmt); //executing the statement
	if (rc != SQLITE_DONE ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "create user successfully\n");
	}
	sqlite3_finalize(stmt);
}


bool Database::validateCredentials(string username, string password) {
	// open connection
	char *zErrMsg = 0;
	int rc;

	string hashedPW = hashPassword(password);
	// convert string to const char*
	const char* const_username = username.c_str();

	//prepare the statement
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, "select password from users where name = ?1;", -1, &stmt, NULL);       /* 1 */
	sqlite3_bind_text(stmt, 1, const_username, -1, SQLITE_STATIC);
	const char* sql = sqlite3_sql(stmt);
	rc = sqlite3_step(stmt); //executing the statement

	if (SQLITE_ROW == rc) {
		string correctPassword;
		correctPassword.append(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		// cout << correctPassword << endl;
		// if (correctPassword == password) {
		if (!correctPassword.compare(hashedPW)) {
			return true;
		}
	}

	if (rc != SQLITE_DONE ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("\nSELECT successfully completed\n");
	}
	sqlite3_finalize(stmt);
	return false;
}

vector < vector<string> > Database::getEmail(string receiver, int shareKey) {
// open connection
	char *zErrMsg = 0;
	int rc;

	// convert string to const char*
	const char* const_receiver = receiver.c_str();

	//prepare the statement
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, "select sender, title, message from mail where receiver = ?1;", -1, &stmt, NULL);       /* 1 */
	sqlite3_bind_text(stmt, 1, const_receiver, -1, SQLITE_STATIC);
	//executing the statement
	string value;
	std::vector < vector< string > > emails;
	while (SQLITE_ROW == (rc = sqlite3_step(stmt))) {	// has row
		std::vector<string> email;
		for (int col = 0; col < 3; col ++) {
			if (col == 2) { // decrypt  message
				string temp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
				value = this->decryptMessage(temp, shareKey);
			} else {
				value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
			}
			email.push_back(value);
		}
		emails.push_back(email);
	}

	if (rc != SQLITE_DONE ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_finalize(stmt);
	return emails;
}


bool Database::sendEmail(string sender, string receiver, string title, string body, int secretKey) {
	bool success = false;

	// open connection
	char *zErrMsg = 0;
	int rc;

	// get shared key
	int shareKey = this->getSharedKey(receiver, secretKey);
	cout << "secretKey" << secretKey << endl;
	cout << "sharekey" << shareKey << endl;
	// encrypt message
	string encryptedMessage = this->encryptMessage(body, shareKey);
	cout << "enc: " << encryptedMessage << endl;
	// convert string to const char*
	const char* const_sender = sender.c_str();
	const char* const_receiver = receiver.c_str();
	const char* const_title = title.c_str();
	const char* const_body = encryptedMessage.c_str();

	//prepare the statement
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, "insert into mail (sender, receiver, title, message) values (?1, ?2,?3,?4);", -1, &stmt, NULL);       /* 1 */
	sqlite3_bind_text(stmt, 1, const_sender, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, const_receiver, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, const_title, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, const_body, -1, SQLITE_STATIC);
	rc = sqlite3_step(stmt); //executing the statement
	if (rc != SQLITE_DONE ) {
		success = false;
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		success = true;
		fprintf(stdout, "create user successfully\n");
	}
	sqlite3_finalize(stmt);
	return success;
}

int Database::getSecretKey(string username) {
	// open connection
	char *zErrMsg = 0;
	int rc;

	// convert string to const char*
	const char* const_username = username.c_str();

	//prepare the statement
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, "select sa from users where name = ?1;", -1, &stmt, NULL);       /* 1 */
	sqlite3_bind_text(stmt, 1, const_username, -1, SQLITE_STATIC);
	//executing the statement
	int Sa = 0;
	while (SQLITE_ROW == (rc = sqlite3_step(stmt))) {	// has row
		Sa = sqlite3_column_int(stmt, 0);
	}

	if (rc != SQLITE_DONE ) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_finalize(stmt);

	// calculate T
	return Sa;
}

/*
string Database::encryptMessage(string message, int key) {

	char key_char = (char) key;
	string output = message;

	for (int i = 0; i < message.size(); i++)
		output[i] = message[i] ^ key_char;

	return output;
}
*/

string Database::encryptMessage(string message, int shared_key) {
	
	//Key and IV setup
    //AES encryption uses a secret key of a variable length (128-bit, 196-bit or 256-   
    //bit). This key is secretly exchanged between two parties before communication   
    //begins. DEFAULT_KEYLENGTH= 16 bytes
    byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( key, shared_key, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

    //
    // String and Sink setup
    //
    std::string plaintext = message;
    std::string ciphertext;
    std::string decryptedtext;

    //
    // Dump Plain Text
    //
    // std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
    // std::cout << plaintext;
    // std::cout << std::endl << std::endl;

    //
    // Create Cipher Text
    //
    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() + 1 );
    stfEncryptor.MessageEnd();

    //
    // Dump Cipher Text
    //
    // std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;
    
    cout << "\n\nciphertext=" << ciphertext << "!!!\n" << endl;
    
    return ciphertext;

    // for( int i = 0; i < ciphertext.size(); i++ ) {

    //     std::cout << "0x" << std::hex << (0xFF & static_cast<byte>(ciphertext[i])) << " ";
    // }

    // std::cout << std::endl << std::endl;
	
}

string Database::decryptMessage(string ciphertext, int shared_key) {
byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memset( key, shared_key, CryptoPP::AES::DEFAULT_KEYLENGTH );
    memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

	string decryptedtext;
	
	CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
    stfDecryptor.MessageEnd();
    
    cout << "\n\ndecryptedtext=" << decryptedtext << "!!!\n" << endl;
    return decryptedtext;

}

string Database::hashPassword(string password){
    std::hash<std::string> str_hash;
	stringstream ss;
    ss << str_hash(password);
    return ss.str();
}

Database::~Database() noexcept(false) {
	sqlite3_close(db);
}


