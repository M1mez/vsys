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



typedef struct freeThis {
	char *attr[3];
	LDAPMessage *result;
	LDAPMessage *e;
} free_t;


int exit(free_t *all, int success){
	free (all->attr[0]);
	free (all->attr[1]);
	//free(all->PW);
	if (all->result) ldap_msgfree(all->result);
	if (all->attr[2]) free (all->attr[2]);
	delete all;
	return success;
}


int loginLDAP(char *inputName, char* PW)
{

	LDAP *ld, *ld2;
	//LDAPMessage *result, *e;
	int rc;
	//char* attr[3];
	string dn, searchFilter;
	free_t *all = new free_t;

	all->attr[0] = strdup("uid");
	all->attr[1] = strdup("cn");
	all->attr[2] = NULL;



	//all->PW = getpass("Password: ");

	//LDAP STANDBY CONNECTION
	if((ld = ldap_init(LDAP_SERVER, LDAP_PORT)) == NULL) return exit(all, EXIT_FAILURE);


	
	//CONNECT TO LDAP (NOT YET ANONYMOUSLY)
	string user = "uid=" + string(inputName) + ",ou=People," + LDAP_SEARCHBASE;
	//rc = ldap_simple_bind_s(ld, NULL, NULL);
	rc = ldap_simple_bind_s(ld, user.c_str(), PW);
	if(rc != LDAP_SUCCESS) return exit(all, EXIT_FAILURE);



	//CONNECTED, NOW CHECK IF USER EXISTS
 	searchFilter = "(uid=" + string(inputName) + ")";
 	rc = ldap_search_s(ld, LDAP_SEARCHBASE, SCOPE, searchFilter.c_str(), all->attr, 0, &all->result);
 	if(rc != LDAP_SUCCESS) {
	    cout << "Username or Password incorrect!" << endl <<"Press ENTER to continue..." << endl;
	    getchar();
	if(rc != LDAP_SUCCESS) return exit(all, EXIT_FAILURE);
 	}


 	//CONNECT TO LDAP WITH USER AND PW
 	all->e = ldap_first_entry(ld, all->result);
 	dn = ldap_get_dn(ld, all->e);
 	if(dn.find(inputName) != string::npos)
 	{
 		ld2 = ldap_init(LDAP_SERVER, LDAP_PORT);
 		rc = ldap_simple_bind_s(ld2, dn.c_str(), PW);
 	}
 	if(rc != LDAP_SUCCESS) {
	    cout << "Username or Password incorrect!" << endl <<"Press ENTER to continue..." << endl;
	    getchar();
 		return exit(all, EXIT_FAILURE);
 	}


 	//DISCONNECT FROM LDAP
 	//ldap_msgfree(e);
	//ldap_msgfree(result);
	ldap_unbind(ld2);
    ldap_unbind(ld);

	return exit(all, EXIT_SUCCESS);
}
