#include <Arduino.h>
#include <RotaryEncoder.h>
#include <LiquidCrystalMenu_I2C.h>

// eRowType = RT_NUMBER
float number1 = 0;
sNumber s_number1 = {&number1, "ml/h", 1, 0.5, 80, 1};

// eRowType = RT_LIST
byte pos_list1 = 0;
const char* list1[2] = {"Opt A", "Opt B"};
sList s_list1 = {list1, 2, &pos_list1};

// eRowType = RT_EVENT
void event(byte data_pos)
{
	Serial.println("This is an event execution of a 'RT_EVENT'");
}

// eRowType = RT_SHOW_NUMBER
float number2 = 10;
sNumber s_number2 = {&number2, "ms", 1, 0, 0, 0};

// eRowType = RT_SHOW_LIST
byte pos_list2 = 0;
const char* list2[3] = {"Opt A", "Opt B", "Opt C"};
sList s_list2 = {list2, 3, &pos_list2};

// eRowType = RT_NUMBER_EVENT
float number3 = 0.22;
sNumber s_number3 = {&number3, "ml/h", 2, 0.01, 1, 0};

void event_number3(byte data_pos)
{
	Serial.print("New value of number 3: ");
	Serial.println(number3);
}

// eRowType = RT_LIST_EVENT
byte pos_list3 = 0;
const char* list3[3] = {"Opt A", "Opt B", "Opt C"};
sList s_list3 = {list3, 3, &pos_list3};

void event_list3(byte data_pos)
{
	Serial.print("New selected item of list 3: ");
	Serial.println(list3[pos_list3]);
}

// eRowType = RT_NUMBER_EXTENDED_EVENT
float number4 = 100;
bool number4_extended_status = false;
sNumber s_number4 = {&number4, "ml/h", 0, 10, 1000, 0};
sExtended s_extended_number4 = {&number4_extended_status, "ON", "OFF", extended_event_number4};

void event_number4(byte data_pos)
{
	Serial.print("New value of number 4: ");
	Serial.println(number4);
}

void extended_event_number4(byte data_pos)
{
	Serial.print("New status of number 4: ");
	if(number4_extended_status) Serial.println("ON");
	else Serial.println("OFF");
}

// eRowType = RT_LIST_EXTENDED_EVENT
byte pos_list4 = 0;
bool list4_extended_status = false;
const char* list4[3] = {"Opt A", "Opt B", "Opt C"};
sList s_list4 = {list4, 3, &pos_list4};
sExtended s_extended_list4 = {&list4_extended_status, "ON", "OFF", extended_event_list4};

void event_list4(byte data_pos)
{
	Serial.print("New selected item of list 4: ");
	Serial.println(list4[pos_list4]);
}

void extended_event_list4(byte data_pos)
{
	Serial.print("New status of list 4: ");
	if(list4_extended_status) Serial.println("ON");
	else Serial.println("OFF");
}

// Menu
// {eRowType type, const char* start_text, sNumber* number, sList* list, void (*event)(void), sExtended* extended}
sRowData MenuData[10] = {	{RT_NONE, "-------Title-------", nullptr, nullptr, nullptr, nullptr},
							{RT_NUMBER, "Number 1: ", &s_number1, nullptr, nullptr, nullptr},
							{RT_LIST, "List 1: ", nullptr, &s_list1, nullptr, nullptr},
							{RT_EVENT, "Execute event", nullptr, nullptr, event, nullptr},
							{RT_SHOW_NUMBER, "Number 2: ", &s_number2, nullptr, nullptr, nullptr},
							{RT_SHOW_LIST, "List 2: ", nullptr, &s_list2, nullptr, nullptr},
							{RT_NUMBER_EVENT, "Number 3: ", &s_number3, nullptr, event_number3, nullptr},
							{RT_LIST_EVENT, "List 3: ", nullptr, &s_list3, event_list3, nullptr},
							{RT_NUMBER_EXTENDED_EVENT, "Nr 4: ", &s_number4, nullptr, event_number4, &s_extended_number4},
							{RT_LIST_EXTENDED_EVENT, "List 4: ", nullptr, &s_list4, event_list4, &s_extended_list4}};

DisplayMenu menu(0x27, 20, 4, MenuData, 10);

// Rotary Encoder
#define CLK_PIN 3 // Defines input for the clock pin
#define DT_PIN 10 // Defines input for the direction pin
#define BTN_PIN 11 // Defines input for the button pin

void encoderISR(void); // Function that calls the clk_isr method of the RotaryEncoder class
RotaryEncoder rot_enc(CLK_PIN, DT_PIN, BTN_PIN, &encoderISR); // Declaration of the RotaryEncoder object

// This function should alwas look like this
void encoderISR(void)
{
    rot_enc.clk_isr();
}


void setup()
{
	menu.begin();
	menu.update_rows();
}

void loop()
{
	byte action_read;
	eMenuAction action = MA_NONE;

    rot_enc.check();
    switch (rot_enc.action)
	{
		case RE_CW:
			action = MA_DOWN;
			break;
		case RE_CCW:
			action = MA_UP;
			break;
        case RE_SHORT:
			action = MA_CHANGE_MODE;
            break;
		case RE_LONG:
			action = MA_EXTENDED;
			break;
	}

	menu.action(action);
}