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
int ind = 0;
char *address;
//char *buffer;

char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',//55
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',//66
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',//70
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		

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
	if(str == '\n')//when \n is seen
		space = (space/160) * 160 + 160;
	else if(str == '\t')//when \t is seen
		space += 10;
	else if(str == '\"'){
		vidptr[space] = '"';
		vidptr[space+1] = fColor;
		space += 2;
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

void printLn(char *str){
	print(str);
	printChar('\n');
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
		vidptr[j+1] = fColor; 		
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
		vidptr[j+1] = fColor; 		
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
	while(str[i]!='\0')
		i++;
	return i;
}

void buildString(char *str, char a){//, int i){//not working properly when used in scan
	str[strlen(str)] = a;
	str[strlen(str)+1] = '\0';
}

void buildStringI(char *str, char a, int b){//not working properly when used in scan
	str[b] = a;
	str[b+1] = '\0';
}

void backSpace(int temp){
	if(space!=temp){
		space-=2;
		ind--;
		vidptr[space] = ' ';
		vidptr[space+1] = 0x04; 		
		showCursor();
	}
}

void command(char *str){
	if(str[0] == 's')
		if(str[1] == 'a')
			if(str[2] == 'y'){
				if(str[3] == ' '){
					int i = 4;
					int j = 0;
					while(str[i]!='\0'){
						str[j] = str[i];
						i++;
						j++;
					}
	  				str[j] = '\0';
					printLn(str);
				}
				else
					printLn("Error: unrecognizable format, format should be \"say string\"");
			}
	if(str[0] == 'c')
		if(str[1] == 'l')
			if(str[2] == 's')
				clear();
	if(str[0] == 'm')
		if(str[1] == 'a')
			if(str[2] == 'r')
			if(str[3] == 'q')
			if(str[4] == 'u')
			if(str[5] == 'e')
			if(str[6] == 'e'){
				int i = 8;
				int j = 0;
				while(str[i]!='\0'){
					str[j] = str[i];
					i++;
					j++;
				}
  				str[j] = '\0';
				printLn(str);
			}
	int j = 0;
	while(str[j]!='\0'){
		str[j] = '\0';
		j++;
	}
}

void scan(char *str){//scans and handles keyboard hits
	int temp = space;
	char c;
	do{
		if(inportb(0x60)!=c){ //PORT FROM WHICH WE READ
			c = inportb(0x60);
			if(c>0){
				if( c<=13 || ((c<28)&&(c>15)) || ((c<42)&&(c>29)&&c!=40) || ((c<54)&&(c>43)) || c==55 || c==66 || c == 70 || c == 43){//if characters are pressed
					if( ((c<26)&&(c>15)) || ((c<39)&&(c>29)) || ((c<51)&&(c>43))){//if letters are pressed
						if(capsEn){
					    		printChar(kbdus[c]-32); //print on screen
							buildStringI(str,kbdus[c]-32,ind);
						}
						else{
					    		printChar(kbdus[c]);
							buildStringI(str,kbdus[c],ind);
						}
					} else{
					    	printChar(kbdus[c]); //print on screen
						buildStringI(str,kbdus[c],ind);
					}
					ind++;
				}
				else if(c == 14){//backspace
					backSpace(temp);
					str[ind] = '\0';
					}
				else if(c == 28){//when enter is pressed
					int temp2 = space;
					int k = temp;
					printChar('\n');
					temp = space;
					ind = 0;
				}
				else if(c == 1){//escape
					clearLine();
					print(address);
					temp = space;
					ind = 0;
				}
				else if(c == 57){//space
					printChar(' ');
					buildStringI(str,' ', ind);
					ind++;
				}
				else if(c == 58)//caps lock
					capsEn = !capsEn;
				else if(c == 54 || c == 42)
					print("shift is pressed\n");
				else
					print("A key is pressed\n");
			}
		}
		outportb(0x60, 0);
	}while(c!=1&&c!=28);//esc or enter
}



void kmain(void)
{
	int d = 5000;
	address = "LatchOS>";
	char *clipboard = "";
	//buffer = "";
	clear();
	print("                   | |   | |   | |   | |   | |   | |   | |   | |\n");
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
	print("                   | |   | |   | |   | |   | |   | |   | |   | |\n");
	delay(d);

	setColor(BLACK, WHITE);
	while(1){
		print(address);
		scan(clipboard);
		command(clipboard);
		print(clipboard);
	}
	return;
}

