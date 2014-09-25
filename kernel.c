#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define MAGENTA 0x5
#define BROWN 0x6
#define LIGHT_GREY 0x7
#define DARK_GREY 0x8
#define LIGHT_BLUE 0x9
#define LIGHT_GREEN 0xA
#define LIGHT_CYAN 0xB
#define LIGHT_RED 0xC
#define LIGHT_MAG 0xD
#define LIGHT_BROWN 0xE
#define WHITE 0xF

int space = 0;//used to store the character space currently in
char *vidptr = (char*)0xb8000;//video mem begins here.
int fColor = 0x07;//used to store the current font color 
int capsEn = 0;
char *address;

void showCursor();
void clearLine();

void gotoxy(int x, int y){//places the cursor in the given coordinates
	space = y*160+x*2;
	showCursor();
}

void scroll(){//shifts all characters 1 line up
	if(space>3998){
		int temp = 160, i;
		gotoxy(0, 24);
		while(temp<=3999){
			vidptr[temp - 160] = vidptr[temp];
			temp++;
		}
		clearLine();
	}
}

void setColor(int a, int b){//sets global variable fColor
	fColor = (a<<4) | b; 
}

void outportb (unsigned short _port, unsigned char _data)//puts value to the specified port
{
    	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

unsigned char inportb (unsigned short _port)//gets value from the specified port value
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void showCursor(){//where cursor should appear
	outportb(0x3D4, 14);
	outportb(0x3D5, (space/2) >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, (space/2));
}

void printChar(char str){//used to print strings acknowledging \t and \n
	if(str == '\n'){//when \n is seen
		space = (space/160) * 160 + 160;
	}
	else if(str == '\t'){//when \t is seen
		space += 10;
	}
	else{
		vidptr[space] = str;
		vidptr[space+1] = fColor;
		space += 2;
	}
	scroll();
	showCursor();
}

void print(char *str){//used to print strings acknowledging \t and \n
	int j = 0;
	while(str[j] != '\0') {
		printChar(str[j]);
		++j;
	}
}

void printColor(int colorA, int colorB, char *str){//use to print a specific text in a specific color then return to the pervious color
	int temp = fColor;
	setColor(colorA, colorB);
	print(str);
	fColor = temp;
}

void clearLine(){//clears the current line where the cursor is in
	int i = 0;
	int j = space = space / 160 * 160;
	while(i < 160) {
		//blank character
		vidptr[j] = ' ';
		//attribute-byte: light grey on black screen	
		vidptr[j+1] = 0x04; 		
		j = j + 2;
		i += 2;
	}
	showCursor();
}

void clear(){// clears the screen
	int j = 0;
	while(j < 80 * 25 * 2) {
		//blank character
		vidptr[j] = ' ';
		//attribute-byte: light grey on black screen	
		vidptr[j+1] = 0x04; 		
		j = j + 2;
	}
	space = 0;
	showCursor();
}

void delay(int d){//produces delay
	int i, j;
	for(i = 0; i < d; i++){
		for(j = 0; j < d; j++){
		}
	}
}

void load(){//produces loading screen 
	setColor(BLACK,WHITE);
	delay(5000);
	int i;
	printChar('\n');
	while(1){
		print("LOADING LATCHOS");
		for(i=0; i<5; i++)
		{
			print(" . ");
			delay(5000);
		}
		clearLine();
	}
}

int strlen(char *str){
	int i = 0;
	while(str[i]!='\0'){
		i++;
	}
	return i;
}

void buildString(char *str, char a){//, int i){//not working properly when used in scan
	str[strlen(str)] = a;
	str[strlen(str)+1] = '\0';
}

void backSpace(int temp){
	if(space!=temp){
		space-=2;
		vidptr[space] = ' ';
		vidptr[space+1] = 0x04; 		
		showCursor();
	}
}

char keyboardConvLow(int a){//converts keyboard codes to actual characters
	switch(a){
		case 16: return 'q';
		case 17: return 'w';
		case 18: return 'e';
		case 19: return 'r';
		case 20: return 't';
		case 21: return 'y';
		case 22: return 'u';
		case 23: return 'i';
		case 24: return 'o';
		case 25: return 'p';

		case 30: return 'a';
		case 31: return 's';
		case 32: return 'd';
		case 33: return 'f';
		case 34: return 'g';
		case 35: return 'h';
		case 36: return 'j';
		case 37: return 'k';
		case 38: return 'l';

		case 44: return 'z';
		case 45: return 'x';
		case 46: return 'c';
		case 47: return 'v';
		case 48: return 'b';
		case 49: return 'n';
		case 50: return 'm';

	}
}

char keyboardConvUp(int a){//converts keyboard codes to actual characters
	switch(a){
		case 16: return 'Q';
		case 17: return 'W';
		case 18: return 'E';
		case 19: return 'R';
		case 20: return 'T';
		case 21: return 'Y';
		case 22: return 'U';
		case 23: return 'I';
		case 24: return 'O';
		case 25: return 'P';

		case 30: return 'A';
		case 31: return 'S';
		case 32: return 'D';
		case 33: return 'F';
		case 34: return 'G';
		case 35: return 'H';
		case 36: return 'J';
		case 37: return 'K';
		case 38: return 'L';

		case 44: return 'Z';
		case 45: return 'X';
		case 46: return 'C';
		case 47: return 'V';
		case 48: return 'B';
		case 49: return 'N';
		case 50: return 'M';

	}
}

void scan(char *str){//scans and handles keyboard hits 
	int temp = space;
	char c;
	do{

		if(inportb(0x60)!=c){ //PORT FROM WHICH WE READ
		    c = inportb(0x60);
		    if(c>0)
			{
			    if( ((c<26)&&(c>15)) || ((c<39)&&(c>29)) || ((c<51)&&(c>43))){//if letters are pressed
				if(capsEn)
			    		printChar(keyboardConvUp(c)); //print on screen
				else
			    		printChar(keyboardConvLow(c));
			    }
			    else if(c == 14)//backspace
			        backSpace(temp);
			    else if(c == 28){//when enter is pressed
				str = "";
				int temp2 = space;
				int k = temp;
				/*for(temp = temp; temp < temp2; temp+=2){//This Does Not F*****G WORK!!!
					buildString(str, vidptr[temp],(temp-k)/2);
					str[(temp-k)/2] = vidptr[temp];
					str[((temp-k)/2)+1] = '\0';
				}*/
				printChar('\n');
				print(address);
				temp = space;
			    }
			    else if(c == 1){//escape
				clearLine();
				print(address);
				temp = space;
			    }
			    else if(c == 57)//space
			        printChar(' ');
			    else if(c == 58)//caps lock
				capsEn = !capsEn;
			}
		    }
	}while(c!=1&&c!=28);//esc or enter
}

void kmain(void)
{
	int d = 5000;
	address = "home:";
	char *clipboard = "";
	clear();
	/*print("                   | |   | |   | |   | |   | |   | |   | |   | |\n");
	delay(d);
	print("                   | |   | |   | |   | |   | |   | |   | |   | |\n");
	delay(d);
	print("                ___| |___| |___| |___| |___| |___| |___| |___| |___\n");
	delay(d);
	print("               |                                                   |\n");
	delay(d);
	print("               |                                                   |\n");
	delay(d);
	print("               |                    **********                     |\n");
	delay(d);
        print("               |                   * **** *** *                    |\n");
	delay(d);
	print("               |                ***  *  * *    ***                 |\n");
	delay(d);
	print("               |                 LA  *  *T***  CH                  |\n");
	delay(d);
	print("               |                ***  *  *   *  ***                 |\n");
	delay(d);
        print("               |                   * **** *** *                    |\n");
	delay(d);
	print("               |                    **********                     |\n");
	delay(d);
	print("               |                                                   |\n");
	delay(d);
	print("               |___   ___   ___   ___   ___   ___   ___   ___   ___|\n");
	delay(d);
	print("                   | |   | |   | |   | |   | |   | |   | |   | |\n");
	delay(d);
	print("                   | |   | |   | |   | |   | |   | |   | |   | |\n");
	delay(d);
	print("                   | |   | |   | |   | |   | |   | |   | |   | |\n");*/
	delay(d);

	setColor(BLACK, WHITE);
	print(address);
	//delay(d);
	//load();
	//gotoxy(0,24);
	//clear();
	scan(clipboard);
	
	//test if buildString works
	clipboard = "hello";
	buildString(clipboard,'a');
	print(clipboard);
        print(address);
	//and it works!!!
	return;
}

