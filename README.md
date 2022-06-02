# Liquid Crystal Menu through I2C

Contains a class to parametrize and control a menu for a Liquid Crystal display through I2C.
After initializing the object `DisplayMenu` with its parameters, the `begin` and `update_rows` methods must be called once to start the display.
Then, a call to the `action` method must be done cyclically in a loop.
This method is used to traverse the menu with a cursor and edit parameters given the action passed as an argument.
This action must be present for one cycle only to take effect.
The possible actions are defined in the `eMenuAction` enumeration.
In the declaration of the `DisplayMenu` object an array of data type `sRowData` must be passed, apart from other parameters.
This array contains the rows in order and all the information that will be displayed.
Each row can be of a different type defined by the `eRowType` enumeration.
The different type of rows allow to show/edit numbers or lists and execute user defined events (functions).
Also, another methods (`update_row`, `update_value` and `update_extended`) allow to force an update on the displayed data in the case a parameter of a row is changed.

## The `DisplayMenu` class constructor

```cpp
DisplayMenu(byte address,
            byte max_characters,
            byte max_rows,
            sRowData* display_data,
            byte data_size);
```

* `address`: The I2C address of the display (default: 0x27)
* `max_characters`: The number of characters in one row of the display
* `max_rows`: The number of rows of the display
* `display_data`: Pointer to an array of type `sRowData`
* `data_size`: Length of the `display_data` array

## The `sRowData` structure

Depending on which type of row is defined only the relevant parameters must be set in the variable of `sRowData` type.
The parameters not used need to be initialized as a `nullptr`.
The reason for this is to save memory space.

```cpp
struct sRowData
{
    eRowType type;
    const char* start_text;
    sNumber* number;
    sList* list;
    void (*event)(byte data_pos);
    sExtended* extended;
};
```

* `type`: Type of row defined in the `eRowType` enumeration
* `start_text`: Text at the beginning of the row. Needed for al type of rows
* `number`: Structure containing the necessary information when using a type of row with a number
* `list`: Structure containing the necessary information when using a type of row with a list
* `event`: Function to be called by a type of row with an event
* `extended`: Structure containing the necessary information when using a type of row with an extended event

### The `sNumber` structure

This structure is necessary when using a type of row with a number.

```cpp
struct sNumber
{
    float* number;
    const char* end;
    byte decimals;
    float increments;
    float max;
    float min;
};
```

* `number`: Pointer to the number variable shown and/or edited
* `end`: Text shown after the number
* `decimals`: Number of decimals shown of the number
* `increments`: Increment/Decrement step of the number
* `max`: Maximum limit of the number
* `min`: Minimum limit of the number

### The `sList` structure

This structure is necessary when using a type of row with a list.

```cpp
struct sList
{
    const char** list;
    byte size;
    byte* item;
};
```

* `list`: Array containing the text of each element in the list
* `size`: Size of the list
* `element`: Pointer to the variable pointing to the selected element

### The `sExtended` structure

This structure is necessary when using a type of row with an extended mode.

```cpp
struct sExtended
{
    bool* status;
    const char* true_text;
    const char* false_text;
    void (*event)(byte data_pos);
};
```

* `status`: Pointer to the variable containing the status of the extended mode
* `true_text`: Text shown at the end of the row when `status` = True
* `false_text`: Text shown at the end of the row when `status` = False
* `event`: Function to be called when the `status` value changes

## The `eMenuAction` enumeration

The actions, passed in the `action` method, allow to change to/from `BROWSE` mode, `EDIT` mode, increment and decrement a number, select another item from a list or execute normal events and extended events.

```cpp
enum eMenuAction
{
    MA_NONE = 0,
    MA_UP,
    MA_DOWN,
    MA_CHANGE_MODE,
    MA_EXTENDED
};
```

* `MA_NONE`: No change is performed. It must be passed continuously when no other action is sended
* `MA_UP`: in `BROWSE` mode it moves the cursor up. In `EDIT` mode it increments the number or selects the next item on a list
* `MA_DOWN`: in `BROWSE` mode it moves the cursor down. In `EDIT` mode it decrements the number or selects the previous item on a list
* `MA_CHANGE_MODE`: in `BROWSE` mode it enters in `EDIT` mode or executes event. In `EDIT` mode it returns to `BROWSE` mode
* `MA_EXTENDED`: in `BROWSE` mode it executes extended event

## The `eRowType` enumeration

The types of rows that can be shown in the display are defined in the enumeration `eRowType`.

```cpp
enum eRowType
{
    RT_NONE=0,
    RT_NUMBER,
    RT_LIST,
    RT_EVENT,
    RT_SHOW_NUMBER,
    RT_SHOW_LIST,
    RT_NUMBER_EVENT,
    RT_LIST_EVENT,
    RT_NUMBER_EXTENDED_EVENT,
    RT_LIST_EXTENDED_EVENT
};
```

* `RT_NONE`: Shows only text
* `RT_NUMBER`: Shows text and number which can be edited
* `RT_LIST`: Shows text and element from list where the selected element of the list can be changed
* `RT_EVENT`: Shows only text and an event can be executed
* `RT_SHOW_NUMBER`: Shows text and a number which is updated with a defined time
* `RT_SHOW_LIST`: Shows text and element from a list which is updated with a defined time
* `RT_NUMBER_EVENT`: Same functionality as `RT_NUMBER` but an event is executed after editing the number
* `RT_LIST_EVENT`: Same functionality as `RT_LIST` but an event is executed after changing the element
* `RT_NUMBER_EXTENDED_EVENT`: Same functionality as `RT_NUMBER_EVENT` but with extended mode
* `RT_LIST_EXTENDED_EVENT`: Same functionality as `RT_LIST_EVENT` but with extended mode

### The `RT_NONE` row type

For this row type the needed parameters in `sRowData` are `type` and `start_text`.
A `nullptr` must be assigned to the rest of the parameters.
While moving the cursor up or down (`MA_UP` and `MA_DOWN` actions) and while in `BROWSE` mode, this row is skipped.

### The `RT_NUMBER` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text` and `number`.
A `nullptr` must be assigned to the rest of the parameters.
When entering `EDIT` mode (`MA_CHANGE_MODE` action) and while standing on this row, the number starts to blink with a defined interval.
Now the value can be edited with the `MA_UP` and `MA_DOWN` actions.
The variable with the number is only updated with the new value after leaving the `EDIT` mode (`MA_CHANGE_MODE` action).

### The `RT_LIST` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text` and `list`.
A `nullptr` must be assigned to the rest of the parameters.
When entering `EDIT` mode (`MA_CHANGE_MODE` action) and while standing on this row, the element starts to blink with a defined interval.
Now the element can be changed with the `MA_UP` and `MA_DOWN` actions.
The variable which points to the element is only updated with the new value after leaving the `EDIT` mode (`MA_CHANGE_MODE` action).

### The `RT_EVENT` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text` and `event`.
A `nullptr` must be assigned to the rest of the parameters.
While standing on this row the event is executed given the `MA_CHANGE_MODE` action.

### The `RT_SHOW_NUMBER` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text` and `number`.
A `nullptr` must be assigned to the rest of the parameters. No action can be performed on this row type.
It only updates the actual value of the number variable with a defined interval.

### The `RT_SHOW_LIST` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text` and `list`.
A `nullptr` must be assigned to the rest of the parameters. No action can be performed on this row type.
It only updates the actual element pointed by the pointer variable with a defined interval.

### The `RT_NUMBER_EVENT` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text`, `number` and `event`.
A `nullptr` must be assigned to the rest of the parameters.
It has the same behavior as `RT_NUMBER` but additionally, when leaving the `EDIT` mode (`MA_CHANGE_MODE` action), an event is executed.

### The `RT_LIST_EVENT` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text`, `list` and `event`.
A `nullptr` must be assigned to the rest of the parameters.
It has the same behavior as `RT_LIST` but additionally, when leaving the `EDIT` mode (`MA_CHANGE_MODE` action), an event is executed. 

### The `RT_NUMBER_EXTENDED_EVENT` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text`, `number`, `event` and `extended`.
A `nullptr` must be assigned to the rest of the parameters.
It has the same behavior as `RT_NUMBER_EVENT` but additionally with the `MA_EXTENDED` action the status is changed and an event is executed.

### The `RT_LIST_EXTENDED_EVENT` row type

For this row type the needed parameters in `sRowData` are `type`, `start_text`, `list`, `event` and `extended`.
A `nullptr` must be assigned to the rest of the parameters.
It has the same behavior as `RT_LIST_EVENT` but additionally with the `MA_EXTENDED` action the status is changed and an event is executed.

## Methods of the `DisplayMenu` object

### The `begin` method

This method must be called once to initialize the display. After this the object and its other methods can be used.

```cpp
void begin(void);
```

### The `action` method

This method must be called cyclically in a loop. It is responsible for managing the display menu.
Two arguments need to be passed. The action needs to be passed for one cycle only to take effect.
If no action is present, then the `MA_NONE` action must be passed constantly.

```cpp
void action(eMenuAction action);
```

### The `update_rows` method

This method refreshes all the rows of the display menu.

```cpp
void update_rows();
```

### The `update_row` method

Sometimes it is necessary to change the parameters of a row and to refresh the complete row the `update_row` method can be called.
As argument the position of the row to be updated must passed (the position in the array passed in the initialization of the object).

```cpp
void update_row(byte pos);
```

### The `update_value` method

Sometimes it is necessary to change the value (number or list) of a row and to refresh this value the `update_value` method can be called.
As argument the position of the row to be updated must passed (the position in the array passed in the initialization of the object).

```cpp
void update_value(byte pos);
```

### The `update_extended` method

Sometimes it is necessary to change the extended value of a row and to refresh this the `update_extended` method can be called.
As argument the position of the row to be updated must passed (the position in the array passed in the initialization of the object).

```cpp
void update_extended(byte pos);
```

### The `reset_position` method

This method resets the position of the cursor and rows to the original values.
This can be used when working with multiple menus.

```cpp
void reset_position();
```

## Timing definitions

The interval time of the blink and show functionalities are defined by some definitions.
These definitions can be redefined if necessary.

Example of redefinition of the timings (here with default values):
```cpp
#define BLINK_INTERVAL 500
#define SHOW_TIME_INTERVAL 200
```