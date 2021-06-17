#include <StandardCplusplus.h>
#include <iterator>
#include <map>
#include <stdio.h>
#include <stdlib.h>

//typedef struct bst_node {
//  struct bst_node* left;
//  struct bst_node* right;
//  char letter;
//} Node;

int dot = 200; // 1/5 of a second, 1 unit
int dash = 3 * dot; // 3 units
int part_space = dot; // 1 unit between parts of a letter
int letter_space = 3 * dot; // 3 units between letters in the same word
int word_space = 7 * dot; // 7 units between words
int LED_pin = 4;
int button_pin = 2;
bool button_status = digitalRead(button_pin);
String message = "";
String morse_message = "";
String code = "";
int str_len;
unsigned long signal_len;
unsigned long t1;
unsigned long t2;
//Node* root_node;
int wait;
std::map<char, String> alphabet;

//void insert(char* c, Node **root, char val) {
//  if (*root == 0) {
//    *root = malloc(sizeof(Node));
//    (*root)->left = 0;
//    (*root)->right = 0;
//  }
//  if (c[0] == '.') {
//    insert(c+1, &((*root)->left), val);
//  }
//  else if (c[0] == '-') {
//    insert(c+1, &((*root)->right), val);
//  }
//  else {
//    (*root)->letter = val;
//  }
//}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(LED_pin, OUTPUT);

  //populate map with keys (letters and numbers) and values (morse codes)
  alphabet['A'] = ".-";
  //alphabet['a'] = ".-";
  alphabet['B'] = "-...";
  //alphabet['b'] = "-...";
  alphabet['C'] = "-.-.";
  //alphabet['c'] = "-.-.";
  alphabet['D'] = "-..";
  //alphabet['d'] = "-..";
  alphabet['E'] = ".";
  //alphabet['e'] = ".";
  alphabet['F'] = "..-.";
  //alphabet['f'] = "..-.";
  alphabet['G'] = "--.";
  //alphabet['g'] = "--.";
  alphabet['H'] = "....";
  //alphabet['h'] = "....";
  alphabet['I'] = "..";
  //alphabet['i'] = "..";
  alphabet['J'] = ".---";
  //alphabet['j'] = ".---";
  alphabet['K'] = "-.-";
  //alphabet['k'] = "-.-";
  alphabet['L'] = ".-..";
  //alphabet['l'] = ".-..";
  alphabet['M'] = "--";
  //alphabet['m'] = "--";
  alphabet['N'] = "-.";
  //alphabet['n'] = "-.";
  alphabet['O'] = "---";
  //alphabet['o'] = "---";
  alphabet['P'] = ".--.";
  //alphabet['p'] = ".--.";
  alphabet['Q'] = "--.-";
  //alphabet['q'] = "--.-";
  alphabet['R'] = ".-.";
  //alphabet['r'] = ".-.";
  alphabet['S'] = "...";
  //alphabet['s'] = "...";
  alphabet['T'] = "-";
  //alphabet['t'] = "-";
  alphabet['U'] = "..-";
  //alphabet['u'] = "..-";
  alphabet['V'] = "...-";
  //alphabet['v'] = "...-";
  alphabet['W'] = ".--";
  //alphabet['w'] = ".--";
  alphabet['X'] = "-..-";
  //alphabet['x'] = "-..-";
  alphabet['Y'] = "-.--";
  //alphabet['y'] = "-.--";
  alphabet['Z'] = "--..";
  //alphabet['z'] = "--..";
  alphabet['0'] = "-----";
  alphabet['1'] = ".----";
  alphabet['2'] = "..---";
  alphabet['3'] = "...--";
  alphabet['4'] = "....-";
  alphabet['5'] = ".....";
  alphabet['6'] = "-....";
  alphabet['7'] = "--...";
  alphabet['8'] = "---..";
  alphabet['9'] = "----.";
  alphabet[' '] = ".......";
  alphabet['\n'] = "......-";

//  char buff[6];
//  String morse;
//  char lett;
//  Node *root = 0;
//  for (std::map<char, String>::iterator it=alphabet.begin(); it!=alphabet.end(); ++it) {
//    lett = it->first;
//    morse = it->second;
//    morse.toCharArray(buff, 6);
//    char *pointer = buff;
//    insert(pointer, &root, lett);
//  }
//  root_node = root;
  Serial.println("Receiving: ");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()> 0) {
    message = Serial.readString();
    transmit();
  }
  else {
  receive();
  }
}

void transmit() {
  eng2code();
  Serial.println(morse_message);
  str_len = morse_message.length();
  for (int i=0;i<str_len;i++) {
    if (morse_message[i] == '.') {
      digitalWrite(LED_pin, HIGH);
      delay(dot);
      digitalWrite(LED_pin, LOW);
    }
    else if (morse_message[i] == '-') {
      digitalWrite(LED_pin, HIGH);
      delay(dash);
      digitalWrite(LED_pin, LOW);
    }
    wait = get_delay(i);
    delay(wait);
  }
  message = "";
  morse_message = "";
  Serial.println("Receiving: ");
  
}

int get_delay(int i) {
  if (i == str_len - 1) {
    return 0;
  }
  else if (morse_message[i+1] != ' ') {
    return part_space;
  }
  else if (morse_message[i+1] == ' ' && morse_message[i+2] != ' ') {
    return letter_space;
  }
  else {
    return word_space;
  }
}

void receive() 
{
NextDotDash:  
  while (digitalRead(button_pin) == HIGH) {}
  t1 = millis();                            //time at button press
  digitalWrite(LED_pin, HIGH);               //LED on while button pressed
  while (digitalRead(button_pin) == LOW) {}
  t2 = millis();                            //time at button release
  digitalWrite(LED_pin, LOW);                //LED off on button release
  signal_len = t2 - t1;                     //time for which button is pressed
  if (signal_len > 50)                      //to account for switch debouncing
  {
    code += readio();                       //function to read dot or dash
  }
  while ((millis() - t2) < 500)           //if time between button press greater than 0.5sec, skip loop and go to next alphabet
  {     
    if (digitalRead(button_pin) == LOW)
    {
      goto NextDotDash;
    }
  }
  code2eng();                          //function to decipher code into alphabet
}

char readio() {
  if (signal_len > 50 && signal_len < 400) {
    return '.';
  }
  else if (signal_len > 400) {
    return '-';
  }
}

void code2eng() {
  //for each combo of dots and dashes in the code word, go through the BST until you hit the letter, and print it to the Serial monitor
//  str_len = code.length();
//  Node* current_node = root_node;
//  for (int i=0;i<str_len;i++) {
//    if (code[i] == '-') {
//      //go right in the tree
//      current_node = current_node->right;
//      if (i == str_len - 1) {
//        Serial.print(current_node->letter);
//        current_node = root_node;
//      }
//    }
//    else if (message[i] == '.') {
//      //go left in the tree
//      current_node = current_node->left;
//      if (i == str_len - 1) {
//        Serial.print(current_node->letter);
//        current_node = root_node;
//      }
//    }
//    else {
//      //end of message
//      code = "";
//      break;
//    }
//  }
  char lett;
  String morse;
  for (std::map<char, String>::iterator it=alphabet.begin(); it!=alphabet.end(); ++it) {
    lett = it->first;
    morse = it->second;
    if (code == morse) {
      Serial.print(lett);
      code = "";
      break;
    }
  }
  code = "";
}

void eng2code() {
  Serial.print("\n");
  Serial.print("Transmitting: ");
  Serial.println(message);
  str_len = message.length();
  for (int i=0;i<str_len;i++) {
    if (message[i] != ' ') {
      morse_message += alphabet[message[i]];
      if (i != str_len - 1) {
        morse_message += " ";
      }
    }
    else {
      morse_message += "  ";
    }
  }
}

