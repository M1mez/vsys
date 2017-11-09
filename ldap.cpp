#include <string>
#include <iostream>
#include <unistd.h>

#define LDAP_DEPRECATED 1
#include <ldap.h>

using namespace std;

#define LDAP_SERVER "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define LDAP_SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define ANON_USER NULL
#define ANON_PW NULL




int exit(char *attr[3], int success);


int loginLDAP(char *inputName, char* PW)
{

	LDAP *ld, *ld2;
	LDAPMessage *result, *e;
	int rc;
	char* attr[3];
	string dn, searchFilter;

	attr[0] = strdup("uid");
	attr[1] = strdup("cn");
	attr[2] = NULL;

	//LDAP STANDBY CONNECTION
	if((ld = ldap_init(LDAP_SERVER, LDAP_PORT)) == NULL) return exit(attr, EXIT_FAILURE);


	
	//CONNECT TO LDAP (NOT YET ANONYMOUSLY)
	string user = "uid=" + string(inputName) + ",ou=People," + LDAP_SEARCHBASE;
	rc = ldap_simple_bind_s(ld, NULL, NULL);
	//rc = ldap_simple_bind_s(ld, user.c_str(), PW);
	if(rc != LDAP_SUCCESS) return exit(attr, EXIT_FAILURE);



	//CONNECTED, NOW CHECK IF USER EXISTS
 	searchFilter = "(uid=" + string(inputName) + ")";
 	rc = ldap_search_s(ld, LDAP_SEARCHBASE, SCOPE, searchFilter.c_str(), attr, 0, &result);
 	if(rc != LDAP_SUCCESS) {
	    cout << "Username or Password incorrect!" << endl;
	if(rc != LDAP_SUCCESS) return exit(attr, EXIT_FAILURE);
 	}


 	//CONNECT TO LDAP WITH USER AND PW
 	e = ldap_first_entry(ld, result);
 	dn = ldap_get_dn(ld, e);
 	if(dn.find(inputName) != string::npos)
 	{
 		ld2 = ldap_init(LDAP_SERVER, LDAP_PORT);
 		rc = ldap_simple_bind_s(ld2, dn.c_str(), PW);
 	}
 	if(rc != LDAP_SUCCESS) {
	    cout << "Username or Password incorrect!" << endl;
 		return exit(attr, EXIT_FAILURE);
 	}


 	//DISCONNECT FROM LDAP
 	//ldap_msgfree(e);
	ldap_msgfree(result);
	ldap_unbind(ld2);
    ldap_unbind(ld);

	return exit(attr, EXIT_SUCCESS);
}

int exit(char *attr[3], int success){
	free (attr[0]);
	free (attr[1]);
	if (attr[2]) free (attr[2]);
	return success;
}