#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encrypt.h"

#define MAX_NAME_LENGTH 20
#define MAX_USER_LENGTH 20
#define MAX_PASS_LENGTH 20
#define MAX_SERVICES 100
#define MAX_ENCRYPT_LENGTH  20

#define DEFAULT "\x1B[0m"
#define PURPLE "\x1B[35m"
#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"

#define DEBUG
struct service{
	char name[MAX_NAME_LENGTH+1];
	char pass[MAX_PASS_LENGTH+1];
};  
typedef struct service service_t;

void conceal_input(){
	system("stty -echo");
}

void deconceal_input(){
	system("stty echo");
}

int check_empty(char str[], int len){
	int char_num;
	int empty = 1;
	for(char_num = 0; char_num < len; char_num++){
		if(str[char_num] != ' '){
			empty = 0;
		}
	}
	return empty;
}
int fix_multi_line(char str[], int len, int max_len){ /* Addresses problems of fgets having multi-line inputs*/
	int too_large = 1;
	int char_num;
	for(char_num = 0; char_num < len; char_num++){
		if(str[char_num] == '\n' && too_large){
			str[char_num] = '\0';
			if(char_num < max_len){
				too_large = 0;
			}
		}
	}
	
	return too_large;
}
int get_input(char str[], int max_len){ /*Implements fgets to get user input */

	char input[max_len+1]; /* Uses +1 to allow for a newline character */
	fgets(input, max_len+1, stdin);
	int too_large = fix_multi_line(input, strlen(input), max_len+1);
	if(too_large){
		/* Clear input buffer */
		int buffer_char;
		int end_of_buffer = 0;
		while (!end_of_buffer){ 
			buffer_char = getchar();
			if(buffer_char == '\n' || buffer_char == EOF){
				end_of_buffer = 1;
			}
		}
	}
	if(strlen(input) == 0 || check_empty(input,strlen(input))){
		get_input(input, max_len); /*Call this function again*/
	}
	input[max_len-1] = '\0';
	strncpy(str,input,max_len);
	return too_large;
}
void get_enter(){
	char empty[3];
	fgets(empty, 3, stdin);
	int too_large = fix_multi_line(empty, 3, 3);
	if(too_large){
		/* Clear input buffer */
		int buffer_char;
		int end_of_buffer = 0;
		while (!end_of_buffer){ 
			buffer_char = getchar();
			if(buffer_char == '\n' || buffer_char == EOF){
				end_of_buffer = 1;
			}
		}
	}
}

int check_login(char user[], char password[]){
    char db_name[MAX_USER_LENGTH+5];
    strncpy(db_name, user, strlen(user));
	db_name[strlen(user)] = '\0';
	strncat(db_name, ".txt", 5);
	
	FILE* db_file = fopen(db_name, "r"); /*Open file as read text */
	
	if(db_file == NULL){ /*If file doesn't exist */
		printf("Read error\n");
		return 2;
	}
	
	rewind(db_file); /*Point back to start of file */
	
	/* Go through each line of the file */
	char passwordmain[MAX_PASS_LENGTH];
    fscanf(db_file, "%s", passwordmain);
	fclose(db_file);
    if(strcmp(passwordmain,password) == 0){
	    return 1;
    }
    else return 0;
} 
   

int user_exists(char username[]){
	
	char db_name[MAX_USER_LENGTH+5];
	strncpy(db_name, username, strlen(username));
	db_name[strlen(username)] = '\0';
	strncat(db_name, ".txt", 5);
	
	#ifdef DEBUG
	printf(PURPLE "Looking for DB: %s\n"DEFAULT , db_name);
	#endif
	
	FILE* db_file; /*Open file as read text */
	
	if(!(db_file = fopen(db_name, "r"))){ /*If file doesnt exist */
		#ifdef DEBUG
		printf(PURPLE "Not found\n"DEFAULT);
		#endif
		return 0;
	}
	else{
		#ifdef DEBUG
		printf(PURPLE "Found\n"DEFAULT);
		#endif
		fclose(db_file);
		return 1;
	}
	return 1;
}

void new_service(char master_password[], char username[]){
	char db_name[MAX_USER_LENGTH+5];
	strncpy(db_name, username, strlen(username));
	db_name[strlen(username)] = '\0';
	strncat(db_name, ".txt", 5);
	
	FILE* db_file = fopen(db_name, "w"); /*Open file as read text */
	if(db_file == NULL){ /*If file doesnt exist*/
		printf("Write error\n");
		return;
	}
	fprintf(db_file, "%s\n", master_password);
	fclose(db_file);
}


int get_user(char current_user[]){
	int success = 0;
	while(!success){
		printf(DEFAULT
		"Enter username, or enter new a new user to create an account: (type EXIT to exit program)\n");
		char user_input[MAX_USER_LENGTH+1];
		get_input(user_input, MAX_USER_LENGTH+1); 
		#ifdef DEBUG
		printf(PURPLE "Input: %s\n" DEFAULT, user_input);
		#endif
		int new_user = 0;
		int old_user = 0;
		/*See if username already exists by checking db file names */
		if(!strcmp("EXIT", user_input)){
			return 0;
		}
		if(!user_exists(user_input)){ /*If user doesnt exist*/
			printf("User %s does not exist. Do you want to create new user? (Y/N)\n", user_input);
			int valid_bool = 0;
			while(!valid_bool){
				char input[2];
				get_input(input, 2);
				
				if(input[0] == 'Y'){
					strncpy(current_user, user_input, strlen(user_input));
					current_user[strlen(user_input)] = '\0';
					new_user = 1;
					valid_bool = 1;
				}
				else if(input[0] == 'N'){
					/*This results in returning to outer while loop*/
					valid_bool = 1;
				}
				else{
					printf(RED "Invalid input, please try Y or N\n" DEFAULT);
				}
			}
		}
		else{
			strncpy(current_user, user_input, strlen(user_input));
			current_user[strlen(user_input)] = '\0';
			old_user = 1;
		}
		char password[MAX_PASS_LENGTH+1];
		if(new_user){
			int valid_pwd = 0;
			while(!valid_pwd){
				char pwd1[MAX_PASS_LENGTH+1];
				printf(DEFAULT"Enter new password:\n");
				conceal_input();
				get_input(pwd1, MAX_PASS_LENGTH+1);
				deconceal_input();
				
				char pwd2[MAX_PASS_LENGTH+1];
				printf("Confirm password:\n");
				conceal_input();
				get_input(pwd2, MAX_PASS_LENGTH+1);
				deconceal_input();
				
				if(!strcmp(pwd1, pwd2)){ /*If passwords match*/
					strncpy(password, pwd1, strlen(pwd1)); /*Copy new password to "get_user" scope*/
					valid_pwd = 1;
                    new_service(pwd1, user_input); /*Creates new databse file*/
				    printf(GREEN "Account creation successful! Press <ENTER> to continue.\n");
				    get_enter();
                    success = 1;
                    return 2;
				}
				else{
					printf(RED "Passwords do not match.\n");
				}
			}
		}
		else if(old_user){
			char pass_input[MAX_PASS_LENGTH+1];
			printf(DEFAULT "Enter password:\n");
			conceal_input();
			get_input(pass_input, MAX_PASS_LENGTH+1);
			deconceal_input();
			success = check_login(user_input, pass_input);
			if(success){
				printf(GREEN "Login succesful! Press <ENTER> to continue.\n");
				get_enter();
				success = 1;
				return 1; /* 1 for old user, 2 for new user*/
			}
			else{
				printf(RED "Login unsuccesful. Please try again.\n");
			}
		}
	}
	return 0;
}

int input_service(service_t services[], int num_services){
	if(num_services < MAX_SERVICES){
		service_t new_service;
		printf("Enter service name>\n");	
		char pass_input[MAX_NAME_LENGTH+1];
		get_input(pass_input, MAX_NAME_LENGTH+1);
		strncpy(new_service.name, pass_input, strlen(pass_input));
		new_service.name[strlen(pass_input)]='\0';	
        char newpass[9];
        int i = 0;         
        for(i=0; i < 9; i++){        
            newpass[i] = rand()%10;
        }		
        printf("Your new password is: ");
        for(i=0; i < 9; i++){        
            printf("%d", newpass[i]);
        } 
		}
}

void print_service(service_t service){
	printf("%s %s",service.name, service.pass);
}

void display_services(service_t services[], int num_services){
	int service_num;
	for(service_num = 0; service_num < num_services; service_num++){
		print_service(services[service_num]); /*Print it */
		printf("\n");
	}
	printf("Press <ENTER> to return to command page.\n");
	get_enter();
}

int get_password(service_t services[], int num_services){
	printf("Enter name of desired service>");
	char name[MAX_NAME_LENGTH+1];
	get_input(name, MAX_NAME_LENGTH+1);
	
	int service_num;
	int found = 0;
	for(service_num = 0; service_num  < num_services; service_num++){
		if(!strcmp(services[service_num].name, name)){
			found = 1;
			char decrypted[MAX_PASS_LENGTH+1];
			char key[20];
			decrypt(services[service_num].pass, strlen(services[service_num].pass), key, decrypted);
			printf(GREEN "Successfully decrypted.\n");
			printf(DEFAULT "%s\n", decrypted);
			printf("Press <ENTER> to continue\n");
			get_enter();
		}
	}
	if(!found){
		printf(RED "Service not found.\n");
		printf("Press <ENTER> to continue\n");
		get_enter();
	}
	return 1;
}



void save_services(char username[], service_t services[], int num_services){
	char db_name[MAX_USER_LENGTH+5];
	strncpy(db_name, username, strlen(username));
	db_name[strlen(username)] = '\0';
	strncat(db_name, ".txt", 5);
	
	FILE* db_file = fopen(db_name, "w"); /*Open file as read text */
	if(db_file == NULL){ /*If file doesnt exist*/
		printf("Write error\n");
		return;
	}
	int service_num;
	for(service_num = 0; service_num < num_services; service_num++){
		service_t sv = services[service_num];
		fprintf(db_file, "%s %s\n", sv.name, sv.pass);
	}
	fclose(db_file);
}

int load_services(char username[], service_t services[], int num_services){
	char db_name[MAX_USER_LENGTH+5];
	strncpy(db_name, username, strlen(username));
	db_name[strlen(username)] = '\0';
	strncat(db_name, ".txt", 5);
	
	FILE* db_file = fopen(db_name, "r"); /*Open file as read text */
	
	if(db_file == NULL){ /*If file doesn't exist */
		printf("Read error\n");
		return num_services;
	}
	
	/*Get number of lines in file*/
	int num_lines = 0;
	while( !feof(db_file) ){
		char next_char = fgetc(db_file);
		if(next_char == '\n'){
			num_lines++;
		}
	}
	
	rewind(db_file); /*Point back to start of file */
	
	/* Go through each line of the file */
	int new_num_services = 0;
	int line_num;
	for(line_num = 0; line_num < MAX_SERVICES; line_num++){
		if(line_num != 0 && line_num < num_lines){ /*If there is still data left in file*/
			service_t sv;
			fscanf(db_file, "%s %s", sv.name, sv.pass);
			services[line_num] = sv;
			new_num_services++;
		}
		else{
			break;
		}
	}
	fclose(db_file);
	return new_num_services;
}

int get_instruction (void){
	int valid = 0;
	while(!valid){ /*Keep asking for input until a valid one is given")*/
		printf(DEFAULT
		"1. Create new password.\n"
		"2. Display all stored passwords.\n"
		"3. Get specific password.\n"
		"4. Logout.\n"
		"5. Exit the program.\n"
		"Please enter a choice (Number between 1 & 5) :\n");
		char command[2];
		get_input(command, 2); 

		int command_num = atoi(command);
		if(command_num > 0 && command_num < 6){ /*Check command_num is between 1 and 5*/
			valid = 1;
			return command_num;
		}
		else{
			printf(RED "Invalid choice\n");
		}
	}
	return 1;
}

int main(){
	system("clear");
	int num_services = 0;
	service_t services[MAX_SERVICES];
	int run = 1;
	printf("================================\n");
	printf("Welcome to password manager 2.0\n");
	printf("================================\n");
	int first_run = 1;
	while(run){
		if(!first_run){
			system("clear");
		}
		first_run = 0;
		
		char username[MAX_USER_LENGTH+1]; 		
        int status = get_user(username);
		/*STATUSES: 0 = exit, 1 = old user, 2 = new user */
		
		#ifdef DEBUG
		system("clear");
		printf(PURPLE "Logged in with status %d\n" DEFAULT, status);
		#endif
		if(status == 0){ 
			run = 0;
			break;
		}
		int logged_in = 1;
		while(logged_in){
			
			#ifndef DEBUG
				system("clear");
			#endif
			num_services = load_services(username, services, num_services);
			int command = get_instruction();
			#ifdef DEBUG
			printf("Command #: %d", command);
			#endif
			
			switch(command){
				case 1:
					num_services = input_service(services, num_services);
					save_services(username, services, num_services);
				break;
				
				case 2:
					display_services(services, num_services);
				break;
				
				case 3:
					get_password(services, num_services);
				break;
				
				case 4:
					logged_in = 0;
					printf("Logging out.\n");
				break;
				
				case 5:
					run = 0;
				break;
				
				default:
					printf("Error: invalid command entered, exiting program.");
					return 1;
				break;
			}
		}
	}

	return 0;
}
