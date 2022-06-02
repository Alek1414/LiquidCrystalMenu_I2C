#include <Arduino.h>
#include "LiquidCrystalMenu_I2C.h"

// remove refresh method
// Add multiple line option

#define ROWS 10

// eRowType = RT_NUMBER
float number1 = 0;
sNumber s_number1 = {&number1, "ml/h", 1, 0.5, 80, 1};

// eRowType = RT_LIST
byte pos_list1 = 0;
const char* list1[2] = {"Opt A", "Opt B"};
sList s_list1 = {list1, 2, &pos_list1};

// eRowType = EVENT
void event(byte data_pos)
{
	Serial.println("event");
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
	Serial.println("event_number3");
}

// eRowType = RT_LIST_EVENT
byte pos_list3 = 0;
const char* list3[3] = {"Opt A", "Opt B", "Opt C"};
sList s_list3 = {list3, 3, &pos_list3};

void event_list3(byte data_pos)
{
	Serial.println("event_list3");
}

// eRowType = RT_NUMBER_EXTENDED_EVENT
float number4 = 100;
bool number4_extended_status = false;
sNumber s_number4 = {&number4, "ml/h", 0, 10, 1000, 0};
sExtended s_extended_number4 = {&number4_extended_status, "ON", "OFF", extended_event_number4};

void event_number4(byte data_pos)
{
	Serial.println("event_number4");
}

void extended_event_number4(byte data_pos)
{
	Serial.println("extended_event_number4");
}

// eRowType = RT_LIST_EXTENDED_EVENT
byte pos_list4 = 0;
bool list4_extended_status = false;
const char* list4[3] = {"Opt A", "Opt B", "Opt C"};
sList s_list4 = {list4, 3, &pos_list4};
sExtended s_extended_list4 = {&list4_extended_status, "ON", "OFF", extended_event_list4};

void event_list4(byte data_pos)
{
	Serial.println("event_list4");
}

void extended_event_list4(byte data_pos)
{
	Serial.println("extended_event_list4");
}

// Menu
// {eRowType type, const char* start_text, sNumber* number, sList* list, void (*event)(void), sExtended* extended}
sRowData MenuData[ROWS] = {{RT_NONE, "-------Title-------", nullptr, nullptr, nullptr, nullptr},
								{RT_NUMBER, "Number 1: ", &s_number1, nullptr, nullptr, nullptr},
								{RT_LIST, "List 1: ", nullptr, &s_list1, nullptr, nullptr},
								{RT_EVENT, "Execute event", nullptr, nullptr, event, nullptr},
								{RT_SHOW_NUMBER, "Number 2: ", &s_number2, nullptr, nullptr, nullptr},
								{RT_SHOW_LIST, "List 2: ", nullptr, &s_list2, nullptr, nullptr},
								{RT_NUMBER_EVENT, "Number 3: ", &s_number3, nullptr, event_number3, nullptr},
								{RT_LIST_EVENT, "List 3: ", nullptr, &s_list3, event_list3, nullptr},
								{RT_NUMBER_EXTENDED_EVENT, "Nr 4: ", &s_number4, nullptr, event_number4, &s_extended_number4},
								{RT_LIST_EXTENDED_EVENT, "List 4: ", nullptr, &s_list4, event_list4, &s_extended_list4}};

DisplayMenu menu(0x27, 20, 4, MenuData, ROWS);


void setup()
{
	Serial.begin(115200);

	menu.begin();
}

void loop()
{
	byte action_read;
	eMenuAction action = MA_NONE;

	if(Serial.available())
	{
		action_read = Serial.read();

		switch (action_read)
		{
			case 'w':
				action = MA_UP;
				break;
			case 's':
				action = MA_DOWN;
				break;
			case 'a':
				action = MA_CHANGE_MODE;
				break;
			case 'd':
				action = MA_EXTENDED;
				break;
			default:
				action = MA_NONE;
		}
	}

	unsigned long t = millis();
	menu.action(action, true);
	if(action != MA_NONE) Serial.println(millis()-t);
	action = MA_NONE;
}