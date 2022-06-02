#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "LiquidCrystalMenu_I2C.h"

DisplayMenu::DisplayMenu(byte address, byte max_characters, byte max_rows, sRowData* display_data, byte data_size)
{
    this->address = address;
    this->max_characters = max_characters;
    this->max_rows = max_rows;
    this->display_data = display_data;
    this->data_size = data_size;

    if(display_data[0].type == RT_NONE) this->cursor_pos = 1;
    else this->cursor_pos = 0;
    this->rows_pos = 0;

    this->display_status = BROWSE;
    this->blink_active = false;
    this->blink_status = false;
    this->blink_last = 0;
    this->show_value = true;
    this->show_last = 0;
}

void DisplayMenu::begin(void)
{
    static LiquidCrystal_I2C lcd_aux(this->address, this->max_characters, this->max_rows);
    this->lcd = &lcd_aux;
    (*this->lcd).init();
    (*this->lcd).backlight();
    (*this->lcd).clear();
    (*this->lcd).setCursor(0,0);
    this->_update_rows();
}

void DisplayMenu::action(eMenuAction action, bool active)
{
    if(active)
    {
        switch(action)
        {
            case MA_UP:
                switch(this->display_status)
                {
                    case BROWSE:
                        if((this->cursor_pos == 1 && display_data[0].type == RT_NONE) || this->cursor_pos == 0)
                        {
                            this->cursor_pos = this->data_size - 1;
                            if(this->data_size > this->max_rows)
                            {
                                this->rows_pos = this->data_size - this->max_rows;
                                this->_update_rows();
                            }
                            this->_update_cursor();
                        }
                        else
                        {
                            this->cursor_pos--;
                            if(this->rows_pos - this->cursor_pos == 1)
                            {
                                if(this->cursor_pos == 1 && display_data[0].type == RT_NONE) this->rows_pos = 0;
                                else this->rows_pos--;
                                this->_update_rows();
                            }
                            else this->_update_cursor();
                        }
                        break;
                    case EDIT:
                        switch(this->display_data[this->cursor_pos].type)
                        {
                            case RT_NUMBER:
                            case RT_NUMBER_EVENT:
                            case RT_NUMBER_EXTENDED_EVENT:
                                this->number_temp += this->display_data[this->cursor_pos].number->increments;
                                if(this->number_temp > this->display_data[this->cursor_pos].number->max)
                                    this->number_temp = this->display_data[this->cursor_pos].number->max;
                                break;
                            case RT_LIST:
                            case RT_LIST_EVENT:
                            case RT_LIST_EXTENDED_EVENT:
                                if(this->list_pos_temp == this->display_data[this->cursor_pos].list->size-1)
                                    this->list_pos_temp = 0;
                                else
                                    this->list_pos_temp++;
                                break;
                        }
                        _update_value(this->cursor_pos);
                    _clean_row(this->cursor_pos);
                        break;
                }
                break;
            case MA_DOWN:
                switch(this->display_status)
                {
                    case BROWSE:
                        if(this->cursor_pos == this->data_size - 1)
                        {
                            if(display_data[0].type == RT_NONE) this->cursor_pos = 1;
                            else this->cursor_pos = 0;
                            this->rows_pos = 0;
                            this->_update_cursor();
                            if(this->data_size > this->max_rows)
                                this->_update_rows();
                        }
                        else
                        {
                            this->cursor_pos++;
                            if(this->cursor_pos - this->rows_pos == this->max_rows)
                            {
                                this->rows_pos++;
                                this->_update_rows();
                            }
                            else this->_update_cursor();
                        }
                        break;
                    case EDIT:
                        switch(this->display_data[this->cursor_pos].type)
                        {
                            case RT_NUMBER:
                            case RT_NUMBER_EVENT:
                            case RT_NUMBER_EXTENDED_EVENT:
                                this->number_temp -= this->display_data[this->cursor_pos].number->increments;
                                if(this->number_temp < this->display_data[this->cursor_pos].number->min)
                                    this->number_temp = this->display_data[this->cursor_pos].number->min;
                                break;
                            case RT_LIST:
                            case RT_LIST_EVENT:
                            case RT_LIST_EXTENDED_EVENT:
                                if(this->list_pos_temp == 0)
                                    this->list_pos_temp = this->display_data[this->cursor_pos].list->size-1;
                                else
                                    this->list_pos_temp--;
                                break;
                        }
                        _update_value(this->cursor_pos);
                        _clean_row(this->cursor_pos);
                        break;
                }
                break;
            case MA_CHANGE_MODE:
                switch(this->display_status)
                {
                    case BROWSE:
                        switch(this->display_data[this->cursor_pos].type)
                        {
                            case RT_NUMBER:
                            case RT_NUMBER_EVENT:
                            case RT_NUMBER_EXTENDED_EVENT:
                                this->display_status = EDIT;
                                this->number_temp = *this->display_data[this->cursor_pos].number->number;
                                this->blink_active = true;
                                break;
                            case RT_LIST:
                            case RT_LIST_EVENT:
                            case RT_LIST_EXTENDED_EVENT:
                                this->display_status = EDIT;
                                this->list_pos_temp = *this->display_data[this->cursor_pos].list->element;
                                this->blink_active = true;
                                break;
                            case RT_EVENT:
                                this->display_data[this->cursor_pos].event(this->cursor_pos);
                                break;
                        }
                        break;
                    case EDIT:
                        this->display_status = BROWSE;
                        this->blink_active = false;
                        this->blink_status = false;
                        switch(this->display_data[this->cursor_pos].type)
                        {
                            case RT_NUMBER:
                            case RT_NUMBER_EVENT:
                            case RT_NUMBER_EXTENDED_EVENT:
                                *this->display_data[this->cursor_pos].number->number = this->number_temp;
                                break;
                            case RT_LIST:
                            case RT_LIST_EVENT:
                            case RT_LIST_EXTENDED_EVENT:
                                *this->display_data[this->cursor_pos].list->element = this->list_pos_temp;
                                break;
                                
                        }
                        switch(this->display_data[this->cursor_pos].type)
                        {  
                            case RT_NUMBER_EVENT:
                            case RT_LIST_EVENT:
                            case RT_NUMBER_EXTENDED_EVENT:
                            case RT_LIST_EXTENDED_EVENT:
                                this->display_data[this->cursor_pos].event(this->cursor_pos);
                                break;
                        }
                        _update_value(this->cursor_pos);
                        _clean_row(this->cursor_pos);
                        break;
                }
                break;
            case MA_EXTENDED:
                if(this->display_status == BROWSE)
                {
                    switch(this->display_data[this->cursor_pos].type)
                    {
                        case RT_NUMBER_EXTENDED_EVENT:
                        case RT_LIST_EXTENDED_EVENT:
                            *this->display_data[this->cursor_pos].extended->status = !*this->display_data[this->cursor_pos].extended->status;
                            update_extended(this->cursor_pos);
                            _clean_row(this->cursor_pos);
                            this->display_data[this->cursor_pos].extended->event(this->cursor_pos);
                            break;
                    }
                }
        }

        if(this->blink_active)
        {
            if(millis() - this->blink_last > BLINK_INTERVAL)
            {
                this->blink_last = millis();
                this->blink_status = !this->blink_status;
                _update_value(this->cursor_pos);
                _clean_row(this->cursor_pos);
            }
        }

        if(millis() - this->show_last > SHOW_TIME_INTERVAL)
        {
            this->show_last = millis();
            for(int i = this->rows_pos ; i < this->rows_pos + this->max_rows ; i++)
            {
                if(this->display_data[i].type == RT_SHOW_NUMBER || this->display_data[i].type == RT_SHOW_LIST)
                {
                    if(this->display_data[i].event != 0)
                        this->display_data[i].event(this->cursor_pos);
                    _update_value(i);
                    _clean_row(i);
                }
            }
        }
    }
}

void DisplayMenu::_update_rows()
{
    (*this->lcd).clear();
    for(int i = this->rows_pos ; i < this->rows_pos + this->max_rows ; i++)
    {
        if(i == this->data_size) break;
        update_row(i);
    }
    this->_update_cursor();
}

void DisplayMenu::_update_cursor()
{
    for(int i = this->rows_pos ; i < this->rows_pos + this->max_rows ; i++)
    {
        (*this->lcd).setCursor(0, i - this->rows_pos);
        if(i == this->cursor_pos) (*this->lcd).print(">");
        else (*this->lcd).print(" ");
    }
}

void DisplayMenu::update_value(byte pos_data)
{
    this->_update_value(pos_data);
    this->_clean_row(pos_data);
}

void DisplayMenu::update_extended(byte pos_data)
{
    this->_update_extended(pos_data);
    this->_clean_row(pos_data);
}

void DisplayMenu::update_row(byte pos_data)
{
    if(pos_data >= this->rows_pos && pos_data <= this->rows_pos + this->max_rows)
    {
        (*this->lcd).setCursor(1, pos_data - this->rows_pos);
        (*this->lcd).print(this->display_data[pos_data].start_text);
        update_value(pos_data);
        update_extended(pos_data);
        _clean_row(pos_data);
    }
}

void DisplayMenu::_update_value(byte pos_data)
{
    String value = "";
    int i;
    byte erase_lenght;

    if(pos_data == this->cursor_pos)
    {
        this->show_value = !this->blink_active || this->blink_status;
        if(this->display_status == EDIT) this->temporal_value = true;
        else this->temporal_value = false;
    }
    else
    {
        this->show_value = true;
        this->temporal_value = false;
    }

    byte start_lenght = strlen(this->display_data[pos_data].start_text);
    (*this->lcd).setCursor(start_lenght+1, pos_data - this->rows_pos);
    switch(this->display_data[pos_data].type)
    {
        case RT_NUMBER:
        case RT_SHOW_NUMBER:
        case RT_NUMBER_EVENT:
        case RT_NUMBER_EXTENDED_EVENT:
            if(this->temporal_value) value = String(this->number_temp, this->display_data[pos_data].number->decimals);
            else value = String(*this->display_data[pos_data].number->number, this->display_data[pos_data].number->decimals);
            if(value[0] == ' ') value = String(value[1]);
            if(this->show_value)
            {
                (*this->lcd).print(value);
                (*this->lcd).print(this->display_data[pos_data].number->end);
            }
            else
            {
                for(i=0; i < value.length(); i++)
                    (*this->lcd).print(" ");
            }
            break;
        case RT_LIST:
        case RT_SHOW_LIST:
        case RT_LIST_EVENT:
        case RT_LIST_EXTENDED_EVENT:
            if(this->temporal_value) value = this->display_data[pos_data].list->list[this->list_pos_temp];
            else value = this->display_data[pos_data].list->list[*this->display_data[pos_data].list->element];
            if(this->show_value)
            {
                (*this->lcd).print(value);
            }
            else
            {
                for(i=0; i < value.length(); i++)
                    (*this->lcd).print(" ");
            }
            break;
    }
}

void DisplayMenu::_update_extended(byte pos_data)
{
    String value = "";

    if(pos_data >= this->rows_pos && pos_data <= this->rows_pos + this->max_rows)
    {
        switch(this->display_data[pos_data].type)
        {
            case RT_NUMBER_EXTENDED_EVENT:
            case RT_LIST_EXTENDED_EVENT:
                (*this->lcd).setCursor(this->max_characters-1, pos_data - this->rows_pos);
                (*this->lcd).rightToLeft();
                if(*this->display_data[pos_data].extended->status) value = this->display_data[pos_data].extended->true_text;
                else value = this->display_data[pos_data].extended->false_text;
                for(int i=value.length()-1;i>=0;i--)
                    (*this->lcd).print(value[i]);
                (*this->lcd).leftToRight();
                break;
        }
    }
}

void DisplayMenu::_clean_row(byte pos_data)
{
    byte start, end;
    String aux_str = "";

    start = strlen(this->display_data[pos_data].start_text);

    switch(this->display_data[pos_data].type)
    {
        case RT_NUMBER:
        case RT_SHOW_NUMBER:
        case RT_NUMBER_EVENT:
        case RT_NUMBER_EXTENDED_EVENT:
            if(this->temporal_value) aux_str = String(this->number_temp, this->display_data[pos_data].number->decimals);
            else aux_str = String(*this->display_data[pos_data].number->number, this->display_data[pos_data].number->decimals);
            if(aux_str[0] == ' ') aux_str = String(aux_str[1]);
            start += aux_str.length();
            start += strlen(this->display_data[pos_data].number->end);
            break;
        case RT_LIST:
        case RT_SHOW_LIST:
        case RT_LIST_EVENT:
        case RT_LIST_EXTENDED_EVENT:
            if(this->temporal_value) start += strlen(this->display_data[pos_data].list->list[this->list_pos_temp]);
            else start += strlen(this->display_data[pos_data].list->list[*this->display_data[pos_data].list->element]);
            break;
    }

    switch(this->display_data[pos_data].type)
    {
        case RT_NUMBER_EXTENDED_EVENT:
        case RT_LIST_EXTENDED_EVENT:
            if(*this->display_data[pos_data].extended->status) end = this->max_characters - strlen(this->display_data[pos_data].extended->true_text);
            else end = this->max_characters - strlen(this->display_data[pos_data].extended->false_text);
            break;
        case RT_NONE:
        case RT_NUMBER:
        case RT_SHOW_NUMBER:
        case RT_NUMBER_EVENT:
        case RT_LIST:
        case RT_SHOW_LIST:
        case RT_LIST_EVENT:
        case RT_EVENT:
            end = this->max_characters;
            break;
    }

    start++;
    if(start != end)
    {
        aux_str = "";
        (*this->lcd).setCursor(start, pos_data - this->rows_pos);
        for(start;start<end;start++)
        {
            aux_str += " ";
        }
        (*this->lcd).print(aux_str);
    }
}