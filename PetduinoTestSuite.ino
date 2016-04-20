#include <LedControl.h>
#include <Petduino.h>

Petduino pet = Petduino();

#define TEST_STATE_MATRIX 0
#define TEST_STATE_PIEZO 1
#define TEST_STATE_LED 2
#define TEST_STATE_BTN 3
#define TEST_STATE_THERM 4
#define TEST_STATE_PHO 5
#define TEST_STATE_DONE 6

byte numbers[][8] = {
  {
    B00000010,
    B00000101,
    B00000101,
    B00000101,
    B00000101,
    B00000101,
    B00000101,
    B00000010
  },
  {
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00000010,
    B00000010
  },
  {
    B00000010,
    B00000101,
    B00000001,
    B00000001,
    B00000010,
    B00000100,
    B00000100,
    B00000111
  },
  {
    B00000010,
    B00000101,
    B00000001,
    B00000010,
    B00000001,
    B00000001,
    B00000101,
    B00000010
  },
  {
    B00000101,
    B00000101,
    B00000101,
    B00000111,
    B00000001,
    B00000001,
    B00000001,
    B00000001
  },
  {
    B00000111,
    B00000100,
    B00000100,
    B00000100,
    B00000110,
    B00000001,
    B00000001,
    B00000110,
  },
  {
    B00000011,
    B00000100,
    B00000100,
    B00000110,
    B00000101,
    B00000101,
    B00000101,
    B00000010,
  },
  {
    B00000111,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
  },
  {
    B00000111,
    B00000101,
    B00000101,
    B00000111,
    B00000101,
    B00000101,
    B00000101,
    B00000111,
  },
  {
    B00000111,
    B00000101,
    B00000101,
    B00000111,
    B00000001,
    B00000001,
    B00000001,
    B00000001,
  }
};

void writeNumber(int number)
{
  // make sure it's not negative
  if ( number < 0 ) {
    number = 0;
  }
  
  // make sure the number is less than 100
  number = number % 100;

  // isolate the ones and tens positions
  int ones = number % 10;
  int tens = (number - ones) / 10;

  byte screen[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  int i;
  for ( i = 0; i < 8; i++ ) {
    screen[i] = (numbers[tens][i] << 4) |
                numbers[ones][i];
  }

  pet.drawImage(screen);
}

void setup() {
  // put your setup code here, to run once:
  pet.begin();

  pet.setState(TEST_STATE_MATRIX);
}

byte screen_ok[8] = {
  B01001000,
  B10101000,
  B10101001,
  B10101010,
  B10101100,
  B10101100,
  B10101010,
  B01001001
};

byte arrow_right[8] = {
  B00000010,
  B00000111,
  B00000010,
  B00000010,
  B00000010,
  0, 0, 0
};

byte arrow_left[8] = {
  arrow_right[0] << 4,
  arrow_right[1] << 4,
  arrow_right[2] << 4,
  arrow_right[3] << 4,
  arrow_right[4] << 4,
  0, 0, 0
};

void testMatrix() {
  int i, j;
  byte buf[8] = {0, 0, 0, 0, 0, 0, 0};
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      buf[i] = 1 << (7-j);
      pet.drawImage(buf);
      delay(20);
    }
    buf[i] = 1 << j;
  }
  // at the end of the routine, draw the now-cleared buffer to clear the screen
  pet.drawImage(buf);
}

void testPiezo() {
  int i;

  for (i = 0; i < 5; i++) {
    pet.playTone(880, 250);
    delay(250);
    pet.playTone(440, 250);
    delay(250);
  }
}

void testLed() {
  int i;

  for (i = 0; i < 5; i++) {
    pet.setLed(true);
    delay(200);
    pet.setLed(false);
    delay(200);
  }
}

void testBtn() {
  pet.drawImage(arrow_left);
  while (!pet.isBtn1Pressed());

  pet.drawImage(arrow_right);
  while (!pet.isBtn2Pressed());

  pet.clearScreen();
}

void testThermometer() {
  float temperature;
  
  while (true) {
    temperature = pet.getTemperature();
    writeNumber(temperature);
    delay(200);
    if (pet.isBtn1Pressed() || pet.isBtn2Pressed()) {
      break;
    }
  }
  
  pet.clearScreen();
}

void testPhotoresistor() {
  // basically the theremin
  byte screen[8] = {0};
  int i;
  
  while (true) {
    // shift screen one column to the left
    for (int i = 0; i < 8; i++) {
      screen[i] <<= 1;
    }
    // get light level
    int light = map(pet.getLightLevel(), 0, 1023, 8, 0);
    for (i = 7; i >= light; i--) {
      screen[i] |= 1;
    }
    pet.drawImage(screen);
    if (pet.isBtn1Pressed() || pet.isBtn2Pressed()) {
      break;
    }
    delay(20);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  pet.update();

  switch (pet.getState()) {
    case TEST_STATE_MATRIX:
      testMatrix();
      pet.setState(TEST_STATE_PIEZO);
      break;
    case TEST_STATE_PIEZO:
      testPiezo();
      pet.setState(TEST_STATE_LED);
      break;
    case TEST_STATE_LED:
      testLed();
      pet.setState(TEST_STATE_BTN);
    break;
      case TEST_STATE_BTN:
      testBtn();
      pet.setState(TEST_STATE_THERM);
      break;
    case TEST_STATE_THERM:
      testThermometer();
      pet.setState(TEST_STATE_PHO);
      break;
    case TEST_STATE_PHO:
      testPhotoresistor();
      pet.setState(TEST_STATE_DONE);
      break;
    case TEST_STATE_DONE:
      pet.drawImage(screen_ok);
      break;
  }
}
