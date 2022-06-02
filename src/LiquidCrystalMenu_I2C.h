#ifndef DISPLAY_MENU
#define DISPLAY_MENU

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

/*
  lcd.print("\xE1"); // gibt ein ä aus
  lcd.print("\xEF"); // gibt ein ö aus
  lcd.print("\xF5"); // gibt ein ü aus
  lcd.print("\xE2"); // gibt ein ß aus
  lcd.print("\xDF"); // gibt ein ° aus
  lcd.print("\xE4"); // gibt ein µ aus
  lcd.print("\xF4"); // gibt ein Ω aus
*/

#ifndef BLINK_INTERVAL
#define BLINK_INTERVAL 500 // [ms] In 'EDIT' mode the number or list item blinks with this interval
#endif

#ifndef SHOW_TIME_INTERVAL
#define SHOW_TIME_INTERVAL 200 // [ms] Update interval for each row with show data type
#endif

// Enumeration with navigation actions of the menu. The action is active with on cycle
enum eMenuAction
{
    MA_NONE=0, // No action is performed
    MA_UP, // In BROWSE mode move cursor up. In EDIT mode increment number or select next element on a list 
    MA_DOWN, // In BROWSE mode move cursor down. In EDIT mode decrement number or select last element on a list 
    MA_CHANGE_MODE, // In BROWSE mode enter EDIT mode or execute event. In EDIT mode return to BROWSE mode
    MA_EXTENDED // In BROWSE mode execute extended event
};

// Enumeration with the type of data shown in a row
enum eRowType
{
    RT_NONE=0, // Shows text
    RT_NUMBER, // Shows text and number with unit which can be edited
    RT_LIST, // Shows text and text from a list where the selected element of the list can be changed
    RT_EVENT, // Shows only text. Additionally a function is called with 'MA_CHANGE_MODE' action
    RT_SHOW_NUMBER, // Shows text and a number which is updated with a defined time
    RT_SHOW_LIST, // Shows text and text from a list which is updated  with a defined time
    RT_NUMBER_EVENT, // Same functionality as RT_NUMBER but a function is called after editing the number
    RT_LIST_EVENT, // Same functionality as RT_LIST but a function is called after changing the selected element
    RT_NUMBER_EXTENDED_EVENT, // Same functionality as RT_NUMBER_EVENT but with 'DA_PRESS_LONG' an additional function is called
    RT_LIST_EXTENDED_EVENT // Same functionality as RT_LIST_EVENT but with 'DA_PRESS_LONG' an additional function is called
};

// Structur with the relevant data when using a row type with a number
// This structure must be assigned to the 'sDisplayData' structure
struct sNumber
{
    float* number; // Pointer to the number variable
    const char* end; // Text shown after the number
    byte decimals; // The number of decimals shown of the number
    float increments; // In 'EDIT' mode it is the increment/decrement in the number of every 'MA_UP'/'MA_DOWN' action. In show row type it is ignored
    float max; // After the number reaches this value it is not incremented any more. In show row type it is ignored
    float min; // After the number reaches this value it is not decremented any more. In show row type it is ignored
};

// Structur with the relevant data when using a row type with a list
// This structure must be assigned to the 'sDisplayData' structure
struct sList
{
    const char** list; // Array containing the text of each element in the list
    byte size; // Size of the list
    byte* element; // Pointer to the variable containing the selected element
};

// Structur with the relevant data when using a row type with an extended event
// This structure must be assigned to the 'sDisplayData' structure
struct sExtended
{
    bool* status; // 'status' changes it's value when ussing the 'MA_EXTENDED' action
    const char* true_text; // Text shown at the end of the row, right adjusted, after the number and it's ending
    const char* false_text; // Text shown at the end of the row, right adjusted, after the number and it's ending
    void (*event)(byte data_pos); // Function to be called by the row types with an extended event when 'MA_EXTENDED' action is active
};

// Structure containing the neccesary information for a row in the display menu
// Depending on the row type some elements aren't neccesary. For this elements a 'nullptr' must be assigned
// The program dosen't check if the data was correctly configured
struct sRowData
{
    eRowType type; // Row type
    const char* start_text; // Text at the beginning of the row
    sNumber* number; // Structure containing the neccesary information when using a row type with a number
    sList* list; // Structure containing the neccesary information when using a row type with a list
    void (*event)(byte data_pos); // Function to be called by the row types with an event
    sExtended* extended; // Structure containing the neccesary information when using a row type with an extended event
};

class DisplayMenu
{
    public:
        DisplayMenu(byte address, // I2C address of the Display
                        byte max_characters, // Width of the diplay
                        byte max_rows, // Height of the display
                        sRowData* display_data, // Array where each element represents a row in the menu, in order
                        byte data_size); // Size (number of rows) of the menu
        void begin(void); // Must be called once at the beginning of the program
        void action(eMenuAction action, bool active); // Sets changes in the menu with the given action. Shown data is not updated
        void update_row(byte pos);
        void update_value(byte pos);
        void update_extended(byte pos);
    private:
        enum eDisplayStatus
        {
            BROWSE,
            EDIT
        };

        LiquidCrystal_I2C* lcd;
        sRowData* display_data;
        byte address;
        byte max_characters;
        byte max_rows;
        byte data_size;
        byte cursor_pos;
        byte rows_pos;
        eDisplayStatus display_status;
        float number_temp;
        byte list_pos_temp;
        bool blink_active;
        bool blink_status;
        unsigned long blink_last;
        unsigned long show_last;
        bool show_value;
        bool temporal_value;

        bool update_value_flag;

        void _update_rows();
        void _update_cursor();
        void _update_value(byte pos);
        void _update_extended(byte pos);
        void _clean_row(byte row);
};

#endif