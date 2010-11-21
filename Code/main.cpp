using namespace std;
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
//#include "functions.h"

# define fcmd "Tables/command"
# define freg "Tables/register"
//# define fadd="addressingmodes"

struct memory {//To map memory location to variable name
		char data[17], var[10];
	};

void scanfile(char *codefile, char* outfile) {//scans codefile and produces outfile
	fstream f1, f2, f3;
	int i, j, k, u, len=0, flag;
	char  line[50] , str[35], fin[25], name[10] , opcode[5], src[10], dest[10], mem[20], reg1[4], reg2[4];
	int memf;//Flag to mark whether instruction uses memory addressing mode (0=no, 1=yes)
	memory table[10];//Table which contains variable names given by user
	f2.open(codefile,ios::in);//reading from codefile
	f3.open(outfile,ios::out | ios::ate);//writing to outfile
	while(strcmp(line,"11111111")) {//"11111111" is our EOF flag
		memf=0;
		f2.get(line,17,'\n');//reading one instruction from codefile
//cout <<"\nLine gotten from input: "<<line<<endl;
		if(strcmp(line,"11111111")) {
			for(i=0;i<4;i++) {
				opcode[i]=line[i];//copying the operation opcode
			}
			opcode[i]='\0';
			if (line[i]=='0') {
				for(j=0;j<3;j++,i++) {
					src[j]=line[i];//copying the source opcode (register and indirect addressing modes)
				}
				src[j]='\0';
				for (j=0;j<3;i++,j++) {
					reg1[j]=line[i];//copying register opcode
				}
				reg1[j]='\0';
			}
			else if (line[i+5]=='0' && line[i]=='1') {
				for(j=0;j<6;j++,i++) {
					src[j]=line[i];//copying the source opcode (memory and immediate addressing modes)
				}
				src[j]='\0';
				f2.get(mem,17,'\n');
				memf=1;
				flag=0;
				for (k=0;k<len;k++) {
					if (!strcmp(table[k].data,mem)) {//checking to see if memory location is present in lookup table
						for(int u=0; u<strlen(mem); u++) mem[u]=' ';//clearing
						strcpy(mem,table[k].var);//if it is present, 
						flag=1;
					}
				}
				if (flag==0) {
					entername:
					cout <<"Enter variable name for location "<<mem<<": ";
					scanf("%s",name);
					for (u=0; u<len; u++) {
						if(!strcmp(name, table[k].var)) {
							cout<<"\n Name already exists, try again, please.\n";
							goto entername;
						}
					}
					strcpy(table[k].var, name);
					strcpy(table[k].data,mem);
					for(int u=0; u<strlen(mem); u++) mem[u]=' ';//clearing
					strcpy(mem, table[k].var);//moving name from table location to mem
					len++;
				}
			}
			else {//immediate
				for(j=0;j<6;j++,i++) {
					src[j]=line[i];
				}
				src[j]='\0';
				f2.get(mem,17,'\n');
				memf=1;
			}
			if (strcmp(opcode,"0110")==0) {//NOT does not have a destination
				strcpy(dest,"\0");
			}
			else {
				if (line[i]=='0') {//register or indirect addressing modes		
					for(j=0;j<3;j++,i++) {
						dest[j]=line[i];//copying the destination opcode
					}
					dest[j]='\0';
					for (j=0;j<3;i++,j++) {
						reg2[j]=line[i];//copying register opcode
					}
					reg2[j]='\0';
				}	
				else if (line[i+5]=='0' && line[i]=='1'){//memory addressing mode
					if (memf==1) {//blocking memory-to-memory code
						cout <<"Incorrect addressing mode \n";
						exit(1);
					} 
					for(j=0;j<5;j++,i++) {
						dest[j]=line[i];
					}
					dest[j]='\0';
					f2.get(mem,17,'\n');
					flag=0;
					for (k=0;k<len;k++) {
						if (!strcmp(table[k].data,mem)) {
							strcpy(mem,table[k].var);//getting variable name from table and replacing it in mem
							flag=1;
						}
					}				
					if (flag==0) {
						entername2:
						cout <<"Enter variable name for location "<<mem<<": ";
						scanf("%s",name);
						for (u=0; u<len; u++) {
							if(!strcmp(name, table[k].var)) {
								cout<<"\n Name already exists, try again, please.\n";
								goto entername2;
							}
						}
						strcpy(table[k].var, name);
						strcpy(table[k].data,mem);
						for(int u=0; u<strlen(mem); u++) mem[u]=' ';
						strcpy(mem, table[k].var);
						len++;
					}
				}	
				else {
					cout <<"Destination cannot be immediate \n";//no it can not be
					exit(1);
				}
			}
			i=0;
			strcpy(str,"");
			strcpy(fin,"");
			f1.open(fcmd,ios::in);
			while(!f1.eof()) {
				f1.getline(fin,25);//getting the command mnemonic along with its opcode
				if (strncmp(fin,opcode,4)==0) {
					for (i=0,j=5;fin[j]!='\0';j++,i++) {
						str[i]=fin[j];
					}
					str[i]=' '; 
					i++;
					break;
				} 
			}
			f1.close();
			strcpy(fin,"");
			if (src[0]=='1') {
				if (src[5]=='1') {//immediate
					str[i]='#';
					i++;
				}
				for (j=0;j<16;i++,j++) 
					str[i]=mem[j];
				if (src[5]=='1') {
					str[i]='b';
					i++;
				}
				str[i]=' ';
				i++;
			}
			else {
				if (src[2]=='1') {//indirect
					str[i]='@';
					i++;
				}
				f1.open(freg,ios::in);
				while(!f1.eof()) {
					f1.getline(fin,25);//getting the register mnemonic along with its opcode
					if (strncmp(fin,reg1,3)==0) {
						for (j=4;fin[j]!='\0';j++,i++) {
							str[i]=fin[j];
						}
						break;
					}
				}
			f1.close();
			}
			if (strcmp(opcode,"0110")) {//if the command is not NOT
				str[i]=',';
				i++;
			}
			str[i]=' ';
			i++;			
			strcpy(fin,"");
			 if (dest[0]=='1') {
				for (j=0;mem[j]!='\0';i++,j++) 
					str[i]=mem[j];
				str[i]=' ';
				i++;
			}
			else {
				if (dest[2]=='1') {
					str[i]='@';//indirect	                
					i++;
				}
				f1.open(freg,ios::in);
				while(!f1.eof()) {
				        f1.getline(fin,25);
				        if (strncmp(fin,reg2,3)==0) {
				                for (j=4;fin[j]!='\0';j++,i++) {
				                        str[i]=fin[j]; 
				                }
				                break;
				        }
				}
				f1.close();
			str[i]='\n'; 
			for (i=0;str[i]!='\n';i++) {//write character by character to file (to bypass \0 escape sequence)
				if (str[i]==' ' && str[i+1]==' ') //removing extra spaces
					i++;
				else
					f3.put(str[i]);
			}
		f3.put('\n');
		}
		}
	}
	f3.close();
	f2.close();
}


main() {	
	char *codefile = new(char); //the input machine code file
	char *outfile = new(char); //the output assembly code file
	cout <<"Welcome to the prototype interactive disassembler\nby Anirudh R (07CO05) and Anushree Sridhar (07CO10)\n";
	cout<<"Enter the name of the file to be disassembled:\n";
	scanf("%s",codefile);
	if (access(codefile,F_OK)) {
		cout <<"Input file cannot be detected \n";
		exit(1);
	}
	cout<<"Enter the name of the file to write to:\n";
	scanf("%s",outfile);
	scanfile(codefile,outfile);
	cout <<"\n";
}
