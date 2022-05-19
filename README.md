# ********* Under construction ***********


# Liquid Crystal Menu through I2C

Contains a class to define and control a menu for a Liquid Crystal through I2C.
After initializing the object `DisplayMenu` with the defined parameters, the `begin` method must be called once to start the display.
After this, a call to the `action` method must be done cyclicaly in a loop.
This method is used to traverse the menu and edit parameters given the action passed as an argument.
This action must be present for one cycle only to take effect.
The possible actions are defined in the `eMenuAction` enumeration. 

```cpp
enum eMenuAction
{
    DA_NONE = 0, // No action is performed
    DA_UP, // In BROWSE mode move cursor up. In EDIT mode increment number or select next item on a list 
    DA_DOWN, // In BROWSE mode move cursor down. In EDIT mode decrement number or select last item on a list 
    DA_CHANGE_MODE, // In BROWSE mode enter EDIT mode or execute event. In EDIT mode exit EDIT mode
    DA_EXTENDED // In BROWSE mode execute extended event
};
```
 
In the declaration of the object an array of data type `sRowData` must be passed.
This array contains the rows in order and all the information that will be displayed.
Each row can be of a different type defined by the `eRowType` enumeration.
Depending on which type of row is defined, different parameters must be declared in `sRowData`.
The different type of rows permit to show/edit numbers or lists and execute events (functions).

## The `sRowData`structure

Depending on which type of row is defined only the used parameters must be set in the variable of `sRowData` type.
The parameters not used need to be initializes as a `nullptr`.
The reason in this is to save memory space.

```cpp
struct sRowData
{
    eRowType type; // Row type
    const char* start_text; // Text at the beginning of the row
    sNumber* number; // Structure containing the neccesary information when using a row type with a number
    sList* list; // Structure containing the neccesary information when using a row type with a list
    void (*event)(byte data_pos); // Function to be called by the row types with an event
    sExtended* extended; // Structure containing the neccesary information when using a row type with an extended event
};
```

### `eRowType type`

### `const char* start_text`

### `sNumber* number`

### `sList* list`

### `void (*event)(byte data_pos)`

### `sExtended* extended`

## Row types

The permited row types are defined in the enumeration `eRowType`.

```cpp
enum eRowType
{
    NONE=0, // Shows text
    NUMBER, // Shows text and number with unit which can be edited
    LIST, // Shows text and text from a list where the selected item of the list can be changed
    EVENT, // Shows only text. Additionally a function is called with 'DA_CHANGE_MODE' action
    SHOW_NUMBER, // Shows text and a number which is updated with a defined time
    SHOW_LIST, // Shows text and text from a list which is updated with a defined time
    NUMBER_EVENT, // Same functionality as NUMBER but a function is called after editing the number
    LIST_EVENT, // Same functionality as LIST but a function is called after changing the selected item
    NUMBER_EXTENDED_EVENT, // Same functionality as NUMBER_EVENT but with 'DA_PRESS_LONG' an additional function is called
};
```

### Type `NONE`

For this row type the needed parameter in `sRowData` is: `start_text`.

### Type `NUMBER`

For this row type the needed parameters in `sRowData` are: `start_text` and `number`.

### Type `LIST`

For this row type the needed parameters in `sRowData` are: `start_text` and `list`.

### Type `EVENT`

For this row type the needed parameter in `sRowData` is: `event`.

### Type `SHOW_NUMBER`

For this row type the needed parameters in `sRowData` are: `start_text` and `number`.

### Type `SHOW_LIST`

For this row type the needed parameters in `sRowData` are: `start_text` and `list`.

### Type `NUMBER_EVENT`

For this row type the needed parameters in `sRowData` are: `start_text`, `number` and `event`.

### Type `LIST_EVENT`

For this row type the needed parameters in `sRowData` are: `start_text`, `list` and `event`.

### Type `NUMBER_EXTENDED_EVENT`

For this row type the needed parameters in `sRowData` are: `start_text`, `number`, `event` and `extended`.








## Example of the object declaration

```cpp
DisplayMenu menu(0x27, 20, 4, MenuData, ROWS);
```

## Detectable actions

The detectable actions of the Rotary Encoder, that are present for one cycle only in the action variable of the object, are the following:

```cpp
enum eRotaryEncoder
{
  RE_NONE = 0,  // No action has been detectet
  RE_CW,        // Rotate Rotary Encoder CW
  RE_CCW,       // Rotate Rotary Encoder CCW
  RE_SHORT,     // Press Rotary Encoder button for a short period
  RE_LONG,      // Press Rotary Encoder button for a long period
  RE_DOUBLE     // Double press Rotary Encoder button
};
```

## Timings for the detection of the actions

The timings for the detection of the actions are set by some defines.
If necesary this defines can be redifened in your code.
This defines are the following:
* `TIME_SHORT` (minimum time for `RE_SHORT` detection [ms])
* `TIME_LONG` (minimum time for `RE_LONG` detection [ms])
* `DOUBLE_DELAY` (maximum time between two pushes of the button for `RE_DOUBLE` detection [ms])
* `DEBOUNCE_TIME` (debounce time for all input signals [ms])

Example of redefinition of the timings (with default values):
```cpp
#define TIME_SHORT 100
#define TIME_LONG 800
#define DOUBLE_DELAY 200
#define DEBOUNCE_TIME 50
```
