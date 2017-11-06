#include <ldap.h>


using namespace std;

#define LDAP_SERVER "ldap.technikum-wien.at"
#define LDAP_PORT 389
#define LDAP_SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define ANON_USER NULL
#define ANON_PW NULL


int loginLDAP(string inputName)
{

	LDAP *ld, *ld2;
	LDAPMessage *result, *e;
	int rc = 0;
	int failCounter = 0;
	char* attr[3];
	string dn;

	attr[0] = strdup("uid");
	attr[1] = strdup("cn");
	attr[2] = NULL;
	
	string searchFilter;


	char* PW = getpass("Password: ");


	if((ld = ldap_init(LDAP_SERVER, LDAP_PORT)) == NULL)
	{

		perror("ldap_initialize");
		return EXIT_FAILURE;

	}
	cout << "ldap_init geschafft" << endl;

	string user = "uid=" + inputName + ",ou=People," + LDAP_SEARCHBASE;

	rc = ldap_simple_bind_s(ld, user.c_str(), PW);

	if(rc != LDAP_SUCCESS)
 	{
		cout << "bind failed" << endl;
		fprintf(stderr, "ERROR: %s\n", ldap_err2string(rc));
		return EXIT_FAILURE;
		
 	}

 	searchFilter = "(uid=" + inputName + ")";

 	rc = ldap_search_s(ld, LDAP_SEARCHBASE, SCOPE, searchFilter.c_str(), attr, 0, &result);

 		if(rc != LDAP_SUCCESS)
 	{
		cout << "search failed" << endl;
		fprintf(stderr, "ERROR: %s\n", ldap_err2string(rc));
		return EXIT_FAILURE;
 	}

 	printf("Total results: %d\n", ldap_count_entries(ld, result));

 	e = ldap_first_entry(ld, result);
 	
 	cout << "DN: " << (dn = ldap_get_dn(ld, e)) << endl;


 	if(dn.find(inputName) != string::npos)
 	{
 		cout << "passt!" << endl;

 		ld2 = ldap_init(LDAP_SERVER, LDAP_PORT);

 		rc = ldap_simple_bind_s(ld2, dn.c_str(), PW);

 		cout << "rc: " << rc << endl;

 		if(rc != 0)
 		{
 			cout << "rc failed!" << endl;
 		}
 		else
 		{
 			cout << "rc worked!" << endl;
 		}
 	}

	ldap_msgfree(result);
	ldap_unbind(ld2);
    ldap_unbind(ld);
    cout << "alles gelöscht" << endl;
	return EXIT_SUCCESS;
}