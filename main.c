// jeezy peezy thats a lotta includes
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
// alrighty fake function here we go!
#define CTRL_KEY(k) ((k) & 0x1f)// sets 3 upper bits in the byte to 0's

//global data
struct termios orig_term;
enum state_t state;

// define enum
enum state_t {
  SELECT,
  CMD,
  EDIT
}

// lol made it a todo bc i want the glow
// TODO: MAIN FUNCTION
int main() {
 char c = '\0';// i love whoever made ascii lol
 while (1) {// like the fact that array closures
   flushScreen();
   processInput();
 }
 return 0; 
}

// output (not processing it bc wtf how do you do that)
void flushScreenFinal() {
  write(STDOUT_FILENO, "\x1b[2J", 4);// clear screen
  write(STDOUT_FILENO, "\x1b[H", 3); // move cursor to bottom (H command means move cursor to bottom
}

void flushScreen() {
  flushScreenFinal();// im lazy lol

  drawSide();
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void drawSide() {
  int i;
  for (i = 0; i < 24; i++) write(STDOUT_FILENO, "~\r\n", 3);// feels weird doing this instead of printf
}

// input && processing it
void processInput() {
  char c = readKey();

  switch (c) {
    case CTRL_KEY('q'):
      flushScreenFinal();// it's just one jmp instruction and we're closing anyway so who cares abt the performance
      exit(0);
      break;
  }
}

char readKey() {// wait for a keypress and return it
  int n;
  char c;
  while ((n = read(STDIN_FILENO, &c, 1)) != 1) {
    if (n == -1 && errno != EAGAIN) ohnoivebeenslain("error in read");
  }
  return c;
}

// boring functions
void ohnoivebeenslain(const char* s) {
  flushScreenFinal();// for my thoughts on performance, see comment under 'q' case of processInput()
  
  perror(s);
  exit(1);
}


void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term) == -1)
    ohnoivebeenslain("tcsetattr() failed");
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_term);
  atexit(disableRawMode);

  struct termios r = orig_term;
  r.c_iflag &= (BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  r.c_oflag &= ~(OPOST);
  r.c_cflag |= (CS8);
  r.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  r.c_cc[VMIN] = 0;
  r.c_cc[VTIME] = 3;


  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &r)) ohnoivebeenslain("tcsetattr() failed again lol");
}

