
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
int shiftEn = 0;
int ind = 0;
int backspace = 0;
int timer_ticks = 0;
char *address;
char *buffer = "";
int *marquee;
/* Defines a GDT entry. We say packed, because it prevents the
*  compiler from doing things that it thinks is best: Prevent
*  compiler "optimization" by packing */
struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

/* Special pointer which includes the limit: The max bytes
*  taken up by the GDT, minus 1. Again, this NEEDS to be packed */
struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;        /* Our kernel segment goes here! */
    unsigned char always0;     /* This will ALWAYS be set to 0! */
    unsigned char flags;       /* Set using the above table! */
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

/* Our GDT, with 3 entries, and finally our special GDT pointer */
struct gdt_entry gdt[3];
struct gdt_ptr gp;

/* Declare an IDT of 256 entries. Although we will only use the
*  first 32 entries in this tutorial, the rest exists as a bit
*  of a trap. If any undefined IDT entry is hit, it normally
*  will cause an "Unhandled Interrupt" exception. Any descriptor
*  for which the 'presence' bit is cleared (0) will generate an
*  "Unhandled Interrupt" exception */
struct idt_entry idt[256];
struct idt_ptr idtp;

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

char kbdusShift1[14] = {
	0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+' };

char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "","","","","","","","","","","","","","","","","","","","","","","","","","",
    "Reserved",
    "Reserved"
};	

void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};	

/* This will be a function in start.asm. We use this to properly
*  reload the new segment registers */
extern void gdt_flush(void);

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load(void);

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();
extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();
extern void stop();
extern void go();

void showCursor();
void clearLine();
void print();
void outportb(unsigned short _port, unsigned char _data);
void marqueeMov();
void initMarquee();
void printChar(char a);
void rmvFrmMrqStck(int *marquee);

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

/* Should be called by main. This will setup the special GDT
*  pointer, set up the first 3 entries in our GDT, and then
*  finally call gdt_flush() in our assembler file in order
*  to tell the processor where the new GDT is and update the
*  new segment registers */
void gdt_install()
{
    /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (int)&gdt;

    /* Our NULL descriptor */
    gdt_set_gate(0, 0, 0, 0, 0);

    /* The second entry is our Code Segment. The base address
    *  is 0, the limit is 4GBytes, it uses 4KByte granularity,
    *  uses 32-bit opcodes, and is a Code Segment descriptor.
    *  Please check the table above in the tutorial in order
    *  to see exactly what each value means */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* The third entry is our Data Segment. It's EXACTLY the
    *  same as our code segment, but the descriptor type in
    *  this entry's access byte says it's a Data Segment */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Flush out the old GDT and install the new changes! */
    gdt_flush();
}

/* Use this function to set an entry in the IDT. Alot simpler
*  than twiddling with the GDT ;) */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
   idt[num].base_lo = base & 0xFFFF;
   idt[num].base_hi = (base >> 16) & 0xFFFF;

   idt[num].sel     = sel;
   idt[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt[num].flags   = flags /* | 0x60 */;
}

unsigned char *memsetv1(void *dest, char val, int count)
{
    /* Add code here to set 'count' bytes in 'dest' to 'val'.
    *  Again, return 'dest' */
	int d0, d1;
	asm volatile("rep\n\t"
		     "stosb"
		     : "=&c" (d0), "=&D" (d1)
		     : "a" (val), "1" (dest), "0" (count)
		     : "memory");
	return dest;
}

/*void isrs_install()
{
}*/

/* Installs the IDT */
void idt_install()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (int)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memsetv1(&idt, 0, idtp.limit+1);

    
    idt_set_gate(0, (unsigned)_isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)_isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)_isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)_isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)_isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)_isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)_isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)_isr7, 0x08, 0x8E);
    idt_set_gate(8, (unsigned)_isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)_isr9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned)_isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)_isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)_isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)_isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)_isr14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)_isr15, 0x08, 0x8E);
    idt_set_gate(16, (unsigned)_isr16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)_isr17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)_isr18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)_isr19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)_isr20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)_isr21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)_isr22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)_isr23, 0x08, 0x8E);
    idt_set_gate(24, (unsigned)_isr24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)_isr25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)_isr26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)_isr27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)_isr28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)_isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)_isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)_isr31, 0x08, 0x8E);
    /* Add any new ISRs to the IDT here using idt_set_gate */

    /* Points the processor's internal register to the new IDT */
    idt_load();
}

void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

/*void irq_remap(void)
{
}*/

void irq_install()
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);

    idt_set_gate(32, (unsigned)_irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)_irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)_irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)_irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)_irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)_irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)_irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)_irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned)_irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)_irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)_irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)_irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)_irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)_irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)_irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)_irq15, 0x08, 0x8E);
}

void irq_handler(struct regs *r)
{
    /* This is a blank function pointer */
    void (*handler)(struct regs *r);

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - 32];
    if (handler)
    {
        handler(r);
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40)
    {
        outportb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outportb(0x20, 0x20);
}

void fault_handler(struct regs *r)
{
    /* Is this a fault whose number is from 0 to 31? */
    if (r->int_no < 32)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
	/*print(exception_messages[r->int_no]);
        print(" Exception. System Halted!\n");*/
        for (;;);
    }
}

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    timer_ticks++;

    /* Every 18 clocks (approximately 1 second), we will
    *  display a message on the screen */
    if (timer_ticks % 18 == 0)
    {
	timer_ticks = 0;
	marqueeMov();
        //print("One second has passed\n");
	//place marquee here
    }
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install()
{
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
    initMarquee();
}

/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

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
		int j = 0;
		while(marquee[j]!=-1){
			marquee[j] = (marquee[j]/160-1)*160+marquee[j]%160;
			j++;
		}
		if((marquee[0]%2==0)&&(marquee[0]<0))
			rmvFrmMrqStck(marquee);
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

void cli(){
    	__asm__ __volatile__ ("cli");
}

void sti(){
    	__asm__ __volatile__ ("sti");
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
		char temp = 34;
		vidptr[space] = temp;
		vidptr[space+1] = fColor;
		space += 2;
	}
	else if(str == '\''){
		char temp = 39;
		vidptr[space] = temp;
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

void initMarquee(){
	marquee[0] = -1;
}

void addMarquee(int *num, char *a){
	int y = space/160+1;
	int last = 0;
	int ctr = 0;
	int temp;
	if(y>=24){
		y = 24;
		printChar('\n');
	}
	while(num[last]!=-1)
		last++;
	temp = num[last] = (y-1)*160;
	num[last+1] = -1;
	while(a[ctr]!='\0'){
		vidptr[temp] = a[ctr];
		vidptr[temp+1] = fColor;
		temp+=2;
		ctr++;
	}
	gotoxy(0,y);
}

void rmvFrmMrqStck(int *num){
	int j = 0;
	if(num[j]!=-1)
		while(num[j]!=-1){
			num[j] = num[j+1];
			j++;
		}
}

void marqueeMov(){
	int j = 0;
	char a;
	int c;
	char last;
	int lastC;
	int ctr;
	while(marquee[j]!=-1){
		a = vidptr[marquee[j]];
		c = vidptr[marquee[j]+1];
		vidptr[marquee[j]] = vidptr[marquee[j]+158];
		vidptr[marquee[j]+1] = vidptr[marquee[j]+159];
		ctr = 2;
		while(ctr < 160){
			last = vidptr[marquee[j]+ctr];
			lastC = vidptr[marquee[j]+ctr+1];
			vidptr[marquee[j]+ctr] = a;
			vidptr[marquee[j]+ctr+1] = c;
			a = last;
			c = lastC;
			ctr+=2;
		}
		j++;
	}	
	
}

void delAllMrq(){
	int j = 0;
	while(marquee[j]!=-1)
		rmvFrmMrqStck(marquee);
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
			if(str[2] == 's'){
				clear();
				delAllMrq();
			}
	if(str[0] == 'm')
		if(str[1] == 'a')
			if(str[2] == 'r')
			if(str[3] == 'q')
			if(str[4] == 'u')
			if(str[5] == 'e')
			if(str[6] == 'e'){
				if(str[7] == ' '){
					int i = 8;
					int j = 0;
					while(str[i]!='\0'){
						str[j] = str[i];
						i++;
						j++;
					}
	  				str[j] = '\0';
					addMarquee(marquee, str);
				}
				else
					printLn("Error: unrecognizable format, format should be \"say string\"");
			}
	int j = 0;
	while(str[j]!='\0'){
		str[j] = '\0';
		j++;
	}
}

void keyboard_handler(struct regs *r)
{
    unsigned char c;

    c = inportb(0x60);
    if (c & 0x80){
	if(c == 182 || c == 170)
		shiftEn = !shiftEn;
    }
    else{
	c = inportb(0x60);
	if(c>0){
		if( c<=13 || ((c<28)&&(c>15)) || ((c<42)&&(c>29)&&c!=40) || ((c<54)&&(c>43)) || c==55 || c==66 || c == 70 || c == 43){//if characters are pressed
			if( ((c<26)&&(c>15)) || ((c<39)&&(c>29)) || ((c<51)&&(c>43))){//if letters are pressed
				if((capsEn&&(!shiftEn))||(shiftEn&&(!capsEn))){
			    		printChar(kbdus[c]-32); //print on screen
					buildStringI(buffer,kbdus[c]-32,ind);
				}
				else{
			    		printChar(kbdus[c]);
					buildStringI(buffer,kbdus[c],ind);
				}
			} else{
				if(shiftEn){
					if(c<14){
				    		printChar(kbdusShift1[c]);
						buildStringI(buffer,kbdusShift1[c],ind);
					} else{
						char temp;
						if(c == 26)
							temp = '{';
						else if(c == 27)
							temp = '}';
						else if(c == 43)
							temp = '|';
						else if(c == 41)
							temp = '~';
						else if(c == 39)
							temp = ':';
						else if(c == 40)
							temp = 34;
						else if(c == 51)
							temp = '<';
						else if(c == 52)
							temp = '>';
						else if(c == 53)
							temp = '?';
					    	printChar(temp); //print on screen
						buildStringI(buffer,temp,ind);
					}
				}
				else{
				    	printChar(kbdus[c]); //print on screen
					buildStringI(buffer,kbdus[c],ind);
				}
			}
			ind++;
		}
		else if(c == 14){//backspace
			backSpace(backspace);
			buffer[ind] = '\0';
			}
		else if(c == 28){//when enter is pressed
			printChar('\n');
			ind = 0;
			command(buffer);
			print(buffer);
			print(address);
			backspace = space;
		}
		else if(c == 1){//escape
			clearLine();
			print(address);
			backspace = space;
			ind = 0;
		}
		else if(c == 57){//space
			printChar(' ');
			buildStringI(buffer,' ', ind);
			ind++;
		}
		else if(c == 58)//caps lock
			capsEn = !capsEn;
		else if(c == 54 || c == 42)
			shiftEn = !shiftEn;
	}
	outportb(0x60, 0);
   }
}

void keyboard_install(){
    irq_install_handler(1, keyboard_handler);
}

void loading_screen(){
	int d = 5000;
	cli();
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
	print(address);
	backspace = space;
	sti();
}

void kmain(void)
{
	gdt_install();
	idt_install();
	irq_install();
	timer_install();
	keyboard_install();
	timer_phase(500);
	//timer_phase(5000);//initialize timer's frequency
	address = "LatchOS>";
	setColor(BLACK, WHITE);
	clear();
	loading_screen();
	char *clipboard = "";
	//buffer = "";

	/*while(1){
		print(address);
		scan(clipboard);
		command(clipboard);
		print(clipboard);
	}*/
	timer_wait(10000);
	return;
}

